/*
 * Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This examples captures data from a PDM microphone using a sample
 * rate of 8 kHz and prints the sample values over the USB serial
 * connection.
 */

#include "ei_microphone.h"

#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include "ei_device_raspberry_rp2040.h"
#include "sensor_aq.h"
#include "sensor_aq_mbedtls_hs256.h"
#include "ei_rp2040_fs_commands.h"
#include "ei_config_types.h"

#include "pico/stdlib.h"

#include "PDM.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"

/* Constant defines -------------------------------------------------------- */
typedef struct
{
    int16_t *buffers[2];
    uint8_t buf_select;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;

/* Extern declared --------------------------------------------------------- */
extern ei_config_t *ei_config_get_config();

using namespace ei;

/* Dummy functions for sensor_aq_ctx type */
static size_t ei_write(const void *, size_t size, size_t count, EI_SENSOR_AQ_STREAM *)
{
    ei_printf("Writing: %d\r\n", count);
    return count;
}

static int ei_seek(EI_SENSOR_AQ_STREAM *, long int offset, int origin)
{
    ei_printf("Seeking: %d\r\n", offset);
    return 0;
}

/* Private variables ------------------------------------------------------- */
static signed short *sampleBuffer;
static bool is_uploaded = false;
static int record_status = 0;
static uint32_t headerOffset;
static uint32_t samples_required;
static uint32_t current_sample;
static uint32_t audio_sampling_frequency = 16000;

// static signed short *inference_buffers[2];
static inference_t inference;

static unsigned char ei_mic_ctx_buffer[1024];
static sensor_aq_signing_ctx_t ei_mic_signing_ctx;
static sensor_aq_mbedtls_hs256_ctx_t ei_mic_hs_ctx;
static sensor_aq_ctx ei_mic_ctx = {
    {ei_mic_ctx_buffer, 1024},
    &ei_mic_signing_ctx,
    &ei_write,
    &ei_seek,
    NULL,
};

/* Audio thread setup */
#define AUDIO_THREAD_STACK_SIZE 4096
/* Global variables */

/* Private functions ------------------------------------------------------- */

static void audio_write_callback(uint32_t n_bytes)
{

    ei_rp2040_fs_write_samples((const uint8_t *)sampleBuffer, headerOffset + current_sample, n_bytes);

    ei_mic_ctx.signature_ctx->update(ei_mic_ctx.signature_ctx, (uint8_t *)sampleBuffer, n_bytes);

    current_sample += n_bytes;
    if (current_sample >= (samples_required << 1))
    {
        PDM.end();
        record_status = 0;
        free(sampleBuffer);
    }
}

static void audio_inference_callback(uint32_t n_bytes)
{
    for (int i = 0; i < n_bytes >> 1; i++)
    {
        inference.buffers[inference.buf_select][inference.buf_count++] = sampleBuffer[i];

        if (inference.buf_count >= inference.n_samples)
        {
            inference.buf_select ^= 1;
            inference.buf_count = 0;
            inference.buf_ready = 1;
        }
    }
}

static void pdm_data_ready_callback(void)
{
    int bytesAvailable = PDM.available();

    int bytesRead = PDM.read((char *)&sampleBuffer[0], bytesAvailable);

    if (record_status == 1)
    {
        audio_write_callback(bytesRead);
    }
    else if (record_status == 2)
    {
        audio_inference_callback(bytesRead);
    }
    else
    {
    }
}

static void finish_and_upload(char *filename, uint32_t sample_length_ms)
{

    ei_printf("Done sampling, total bytes collected: %u\n", current_sample * 2);

    ei_printf("[1/1] Uploading file to Edge Impulse...\n");

    ei_printf("Not uploading file, not connected to WiFi. Used buffer, from=%lu, to=%lu.\n", 0, current_sample + headerOffset);

    ei_printf("[1/1] Uploading file to Edge Impulse OK (took %d ms.)\n", 0);

    is_uploaded = true;

    ei_printf("OK\n");
}

static int insert_ref(char *buffer, int hdrLength)
{
#define EXTRA_BYTES(a) ((a & 0x3) ? 4 - (a & 0x3) : (a & 0x03))
    const char *ref = "Ref-BINARY-i16";
    int addLength = 0;
    int padding = EXTRA_BYTES(hdrLength);

    buffer[addLength++] = 0x60 + 14 + padding;
    for (int i = 0; i < strlen(ref); i++)
    {
        buffer[addLength++] = *(ref + i);
    }
    for (int i = 0; i < padding; i++)
    {
        buffer[addLength++] = ' ';
    }

    buffer[addLength++] = 0xFF;

    return addLength;
}

static bool create_header(void)
{

    sensor_aq_init_mbedtls_hs256_context(&ei_mic_signing_ctx, &ei_mic_hs_ctx, ei_config_get_config()->sample_hmac_key);

    sensor_aq_payload_info payload = {
        EiDevice.get_id_pointer(),
        EiDevice.get_type_pointer(),
        1000.0f / static_cast<float>(audio_sampling_frequency),
        {{"audio", "wav"}}};

    int tr = sensor_aq_init(&ei_mic_ctx, &payload, NULL, true);

    if (tr != AQ_OK)
    {
        ei_printf("sensor_aq_init failed (%d)\n", tr);
        return false;
    }

    // then we're gonna find the last byte that is not 0x00 in the CBOR buffer.
    // That should give us the whole header
    size_t end_of_header_ix = 0;
    for (size_t ix = ei_mic_ctx.cbor_buffer.len - 1; ix >= 0; ix--)
    {
        if (((uint8_t *)ei_mic_ctx.cbor_buffer.ptr)[ix] != 0x0)
        {
            end_of_header_ix = ix;
            break;
        }
    }

    if (end_of_header_ix == 0)
    {
        ei_printf("Failed to find end of header\n");
        return false;
    }

    int ref_size = insert_ref(((char *)ei_mic_ctx.cbor_buffer.ptr + end_of_header_ix), end_of_header_ix);

    // and update the signature
    tr = ei_mic_ctx.signature_ctx->update(ei_mic_ctx.signature_ctx, (uint8_t *)(ei_mic_ctx.cbor_buffer.ptr + end_of_header_ix), ref_size);
    if (tr != 0)
    {
        ei_printf("Failed to update signature from header (%d)\n", tr);
        return false;
    }

    end_of_header_ix += ref_size;

    // Write to blockdevice
    tr = ei_rp2040_fs_write_samples((uint8_t *)ei_mic_ctx.cbor_buffer.ptr, 0, end_of_header_ix);

    if (tr != 0)
    {
        ei_printf("Failed to write to header blockdevice (%d)\n", tr);
        return false;
    }

    headerOffset = end_of_header_ix;

    return true;
}

bool ei_microphone_record(uint32_t sample_length_ms, uint32_t start_delay_ms, bool print_start_messages)
{

    start_delay_ms = start_delay_ms < 2000 ? 2000 : start_delay_ms;

    if (print_start_messages)
    {
        ei_printf("Starting in %lu ms... (or until all flash was erased)\n", start_delay_ms);
    }

    EiDevice.delay_ms(2000);
    // vTaskDelay(start_delay_ms);

    if (ei_rp2040_fs_erase_sampledata(0, (samples_required << 1) + 4096) != RP2040_FS_CMD_OK)
    {
        return false;
    }

    create_header();

    record_status = 1;

    // xTaskCreate(capture_samples, "CaptureSamples", 4096, NULL, 5, NULL);
    // vTaskStartScheduler();

    if (print_start_messages)
    {
        ei_printf("Sampling...\n");
    }

    return true;
}

bool ei_microphone_inference_start(uint32_t n_samples, float interval_ms)
{

    inference.buffers[0] = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if (inference.buffers[0] == NULL)
    {
        return false;
    }

    inference.buffers[1] = (int16_t *)malloc(n_samples * sizeof(int16_t));

    if (inference.buffers[1] == NULL)
    {
        free(inference.buffers[0]);
        return false;
    }

    uint32_t sample_buffer_size = (n_samples / 100) * sizeof(int16_t);
    sampleBuffer = (int16_t *)malloc(sample_buffer_size);

    if (sampleBuffer == NULL)
    {
        free(inference.buffers[0]);
        free(inference.buffers[1]);
        return false;
    }

    inference.buf_select = 0;
    inference.buf_count = 0;
    inference.n_samples = n_samples;
    inference.buf_ready = 0;

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_callback);

    PDM.setBufferSize((n_samples / 100) * sizeof(int16_t));

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, 16000))
    {
        ei_printf("Failed to start PDM!");
    }

    /* Calclate sample rate from sample interval */
    audio_sampling_frequency = (uint32_t)(1000.f / interval_ms);

    // set the gain, defaults to 20
    PDM.setGain(127);

    record_status = 2;

    // xTaskCreate(capture_samples, "CaptureSamples", 1024 * 32, NULL, 5, NULL);
    // vTaskStartScheduler();
    return true;
}

/**
 * @brief      Wait for a full buffer
 *
 * @return     In case of an buffer overrun return false
 */
bool ei_microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1)
    {
        ei_printf(
            "Error sample buffer overrun. Decrease the number of slices per model window "
            "(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }

    while (inference.buf_ready == 0)
    {
        // vTaskDelay(1);
        EiDevice.delay_ms(1);
    }

    inference.buf_ready = 0;

    return ret;
}

bool ei_microphone_inference_is_recording(void)
{
    return inference.buf_ready == 0;
}

/**
 * @brief      Reset buffer counters for non-continuous inferencing
 */
void ei_microphone_inference_reset_buffers(void)
{
    inference.buf_ready = 0;
    inference.buf_count = 0;
}

/**
 * Get raw audio signal data
 */
int ei_microphone_inference_get_data(size_t offset, size_t length, float *out_ptr)
{
    return ei::numpy::int16_to_float(&inference.buffers[inference.buf_select ^ 1][offset], out_ptr, length);
}

bool ei_microphone_inference_end(void)
{

    PDM.end();
    free(inference.buffers[0]);
    free(inference.buffers[1]);
    free(sampleBuffer);
    return 0;
}

/**
 * Sample raw data
 */
bool ei_microphone_sample_start(void)
{

    int sample_length_blocks;

    ei_printf("Sampling settings:\n");
    ei_printf("\tInterval: %.5f ms.\n", (float)ei_config_get_config()->sample_interval_ms);
    ei_printf("\tLength: %lu ms.\n", ei_config_get_config()->sample_length_ms);
    ei_printf("\tName: %s\n", ei_config_get_config()->sample_label);
    ei_printf("\tHMAC Key: %s\n", ei_config_get_config()->sample_hmac_key);

    char filename[256];
    int fn_r = snprintf(filename, 256, "/fs/%s", ei_config_get_config()->sample_label);
    if (fn_r <= 0)
    {
        ei_printf("ERR: Failed to allocate file name\n");
        return false;
    }
    ei_printf("\tFile name: %s\n", filename);

    /* Calculate sample rate from sample interval */
    audio_sampling_frequency = (uint32_t)(1000.f / ei_config_get_config()->sample_interval_ms);

    samples_required = (uint32_t)(((float)ei_config_get_config()->sample_length_ms) / ei_config_get_config()->sample_interval_ms);

    // Round to even number of samples for word align flash write
    if (samples_required & 1)
    {
        samples_required++;
    }

    current_sample = 0;
    is_uploaded = false;

    sampleBuffer = (int16_t *)malloc(ei_rp2040_fs_get_block_size());
    if (sampleBuffer == NULL)
    {
        return false;
    }

    // configure the data receive callback
    PDM.onReceive(&pdm_data_ready_callback);

    // ei_printf("Sector size: %d\r\n", EiDevice.filesys_get_block_size());
    PDM.setBufferSize(ei_rp2040_fs_get_block_size());

    // initialize PDM with:
    // - one channel (mono mode)
    // - a 16 kHz sample rate
    if (!PDM.begin(1, 16000))
    {
        ei_printf("Failed to start PDM!");
    }

    // set the gain, defaults to 20
    PDM.setGain(127);

    bool r = ei_microphone_record(ei_config_get_config()->sample_length_ms, (((samples_required << 1) / ei_rp2040_fs_get_block_size()) * RP2040_FS_BLOCK_ERASE_TIME_MS), true);
    if (!r)
    {
        return r;
    }

    while (record_status)
    {
        // EiDevice.delay_ms(10);
    };

    int ctx_err = ei_mic_ctx.signature_ctx->finish(ei_mic_ctx.signature_ctx, ei_mic_ctx.hash_buffer.buffer);
    if (ctx_err != 0)
    {
        ei_printf("Failed to finish signature (%d)\n", ctx_err);
        return false;
    }

    // load the first page in flash...
    uint8_t *page_buffer = (uint8_t *)malloc(ei_rp2040_fs_get_block_size());
    if (!page_buffer)
    {
        ei_printf("Failed to allocate a page buffer to write the hash\n");
        return false;
    }

    ei_rp2040_flush_buf(0);
    EiDevice.delay_ms(100);

    int j = ei_rp2040_fs_read_sample_data(page_buffer, 0, ei_rp2040_fs_get_block_size());
    if (j != 0)
    {
        ei_printf("Failed to read first page (%d)\n", j);
        ei_free(page_buffer);
        return false;
    }

    // update the hash
    uint8_t *hash = ei_mic_ctx.hash_buffer.buffer;
    // we have allocated twice as much for this data (because we also want to be able to represent in hex)
    // thus only loop over the first half of the bytes as the signature_ctx has written to those
    for (size_t hash_ix = 0; hash_ix < ei_mic_ctx.hash_buffer.size / 2; hash_ix++)
    {
        // this might seem convoluted, but snprintf() with %02x is not always supported e.g. by newlib-nano
        // we encode as hex... first ASCII char encodes top 4 bytes
        uint8_t first = (hash[hash_ix] >> 4) & 0xf;
        // second encodes lower 4 bytes
        uint8_t second = hash[hash_ix] & 0xf;

        // if 0..9 -> '0' (48) + value, if >10, then use 'a' (97) - 10 + value
        char first_c = first >= 10 ? 87 + first : 48 + first;
        char second_c = second >= 10 ? 87 + second : 48 + second;

        page_buffer[ei_mic_ctx.signature_index + (hash_ix * 2) + 0] = first_c;
        page_buffer[ei_mic_ctx.signature_index + (hash_ix * 2) + 1] = second_c;
    }

    j = ei_rp2040_fs_erase_sampledata(0, ei_rp2040_fs_get_block_size());
    if (j != 0)
    {
        ei_printf("Failed to erase first page (%d)\n", j);
        ei_free(page_buffer);
        return false;
    }

    EiDevice.delay_ms(100);

    j = ei_rp2040_fs_write_samples(page_buffer, 0, ei_rp2040_fs_get_block_size());

    ei_free(page_buffer);

    if (j != 0)
    {
        ei_printf("Failed to write first page with updated hash (%d)\n", j);
        return false;
    }

    ei_rp2040_flush_buf(0);
    EiDevice.delay_ms(100);

    finish_and_upload(filename, ei_config_get_config()->sample_length_ms);

    return true;
}
