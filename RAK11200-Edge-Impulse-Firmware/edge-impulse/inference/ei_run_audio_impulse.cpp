/* Edge Impulse ingestion SDK
 * Copyright (c) 2022 EdgeImpulse Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* Include ----------------------------------------------------------------- */
#include "model-parameters/model_metadata.h"
#include "driver/gpio.h"

#define GPIO_OUTPUT_LED_GREEN GPIO_NUM_12
#define GPIO_OUTPUT_LED_BLUE GPIO_NUM_2
#define GPIO_OUTPUT_RELAY GPIO_NUM_18

#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_LED_GREEN) | (1ULL << GPIO_OUTPUT_LED_BLUE))

#if defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/dsp/numpy.hpp"
#include "ei_microphone.h"
#include "ei_device_espressif_esp32.h"
#include "ei_run_impulse.h"

typedef enum
{
    INFERENCE_STOPPED,
    INFERENCE_WAITING,
    INFERENCE_SAMPLING,
    INFERENCE_DATA_READY
} inference_state_t;

static int print_results;
static uint16_t samples_per_inference;
static inference_state_t state = INFERENCE_STOPPED;
static uint64_t last_inference_ts = 0;
static bool continuous_mode = false;
static bool debug_mode = false;
static float samples_circ_buff[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
static int samples_wr_index = 0;
static int keyword = 0;
static int out_status = 0;
unsigned long previousMillis = 0; // will store last time keyword was said

static void display_results(ei_impulse_result_t *result)
{
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
              result->timing.dsp, result->timing.classification, result->timing.anomaly);
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++)
    {
        ei_printf("    %s: \t%f\r\n", result->classification[ix].label, result->classification[ix].value);
    }

    if (result->classification[0].value > 0.20 && keyword == 0)
    {

        // zero-initialize the config structure.
        /*gpio_config_t io_conf = {};
        // set as output mode
        io_conf.mode = GPIO_MODE_OUTPUT;
        // bit mask of the pins that you want to set,e.g.GPIO18/19
        io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
        gpio_config(&io_conf);*/
        gpio_set_level(GPIO_OUTPUT_LED_GREEN, 1);
        keyword = 1;
        // ei_sleep(500);
    }
    if (result->classification[2].value > 0.20 && keyword == 1 && out_status == 1) // Turn off the light
    {
        keyword = 0;
        gpio_set_level(GPIO_OUTPUT_LED_BLUE, 0);
        gpio_set_level(GPIO_OUTPUT_LED_GREEN, 0);
        gpio_set_level(GPIO_OUTPUT_RELAY, 0);
        out_status = 0;
        // ei_sleep(500);
    }
    if (result->classification[3].value > 0.20 && keyword == 1 && out_status == 0) // Turn on the light
    {
        keyword = 0;
        gpio_set_level(GPIO_OUTPUT_RELAY, 1);
        gpio_set_level(GPIO_OUTPUT_LED_BLUE, 1);
        gpio_set_level(GPIO_OUTPUT_LED_GREEN, 0);
        out_status = 1;
        // ei_sleep(500);
    }

    unsigned long currentMillis = ei_read_timer_ms();

    if (currentMillis - previousMillis >= 10000 && keyword == 1)
    {
        // save the last time you said the keyword
        previousMillis = currentMillis;
        gpio_set_level(GPIO_OUTPUT_LED_GREEN, 0);
        keyword = 0;
    }

#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %f\r\n", result->anomaly);
#endif
}

void ei_run_impulse(void)
{
    switch (state)
    {
    case INFERENCE_STOPPED:
        // nothing to do
        return;
    case INFERENCE_WAITING:
        if (ei_read_timer_ms() < (last_inference_ts + 2000))
        {
            return;
        }
        state = INFERENCE_SAMPLING;
        ei_microphone_inference_reset_buffers();
        break;
    case INFERENCE_SAMPLING:
        // wait for data to be collected through callback
        if (ei_microphone_inference_is_recording())
        {
            return;
        }
        state = INFERENCE_DATA_READY;
        break;
        // nothing to do, just continue to inference provcessing below
    case INFERENCE_DATA_READY:
    default:
        break;
    }

    signal_t signal;

    signal.total_length = continuous_mode ? EI_CLASSIFIER_SLICE_SIZE : EI_CLASSIFIER_RAW_SAMPLE_COUNT;
    signal.get_data = &ei_microphone_inference_get_data;

    // run the impulse: DSP, neural network and the Anomaly algorithm
    ei_impulse_result_t result = {0};
    EI_IMPULSE_ERROR ei_error;
    if (continuous_mode == true)
    {
        ei_error = run_classifier_continuous(&signal, &result, debug_mode);
    }
    else
    {
        ei_error = run_classifier(&signal, &result, debug_mode);
    }
    if (ei_error != EI_IMPULSE_OK)
    {
        ei_printf("Failed to run impulse (%d)", ei_error);
        return;
    }

    if (continuous_mode == true)
    {
        if (++print_results >= (EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW >> 1))
        {
            display_results(&result);
            print_results = 0;
        }
    }
    else
    {
        display_results(&result);
    }

    if (continuous_mode == true)
    {
        state = INFERENCE_SAMPLING;
    }
    else
    {
        ei_printf("Starting inferencing in 2 seconds...\n");
        last_inference_ts = ei_read_timer_ms();
        state = INFERENCE_WAITING;
    }
}

void ei_start_impulse(bool continuous, bool debug, bool use_max_uart_speed)
{
    const float sample_length = 1000.0f * static_cast<float>(EI_CLASSIFIER_RAW_SAMPLE_COUNT) /
                                (1000.0f / static_cast<float>(EI_CLASSIFIER_INTERVAL_MS));

    continuous_mode = continuous;
    debug_mode = debug;

    gpio_pad_select_gpio(GPIO_OUTPUT_LED_BLUE);
    gpio_reset_pin(GPIO_OUTPUT_LED_BLUE);

    gpio_pad_select_gpio(GPIO_OUTPUT_LED_GREEN);
    gpio_reset_pin(GPIO_OUTPUT_LED_GREEN);

    gpio_set_direction(GPIO_OUTPUT_LED_BLUE, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_OUTPUT_LED_GREEN, GPIO_MODE_OUTPUT);

    // summary of inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    ei_printf("\tInterval: %fms.\n", (float)EI_CLASSIFIER_INTERVAL_MS);
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    ei_printf("\tSample length: %f ms.\n", sample_length);
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) /
                                            sizeof(ei_classifier_inferencing_categories[0]));
    ei_printf("Starting inferencing, press 'b' to break\n");

    if (continuous == true)
    {
        samples_per_inference = EI_CLASSIFIER_SLICE_SIZE * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
        // In order to have meaningful classification results, continuous inference has to run over
        // the complete model window. So the first iterations will print out garbage.
        // We now use a fixed length moving average filter of half the slices per model window and
        // only print when we run the complete maf buffer to prevent printing the same classification multiple times.
        print_results = -(EI_CLASSIFIER_SLICES_PER_MODEL_WINDOW);
        run_classifier_init();
        state = INFERENCE_SAMPLING;
    }
    else
    {
        samples_per_inference = EI_CLASSIFIER_RAW_SAMPLE_COUNT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME;
        // it's time to prepare for sampling
        ei_printf("Starting inferencing in 2 seconds...\n");
        last_inference_ts = ei_read_timer_ms();
        state = INFERENCE_WAITING;
    }

    if (ei_microphone_inference_start(continuous_mode ? EI_CLASSIFIER_SLICE_SIZE : EI_CLASSIFIER_RAW_SAMPLE_COUNT, EI_CLASSIFIER_INTERVAL_MS) == false)
    {
        ei_printf("ERR: Failed to setup audio sampling");
        return;
    }

    while (!ei_user_invoke_stop())
    {
        ei_run_impulse();
        ei_sleep(1);
    }

    ei_stop_impulse();
}

void ei_stop_impulse(void)
{
    if (state != INFERENCE_STOPPED)
    {
        ei_printf("Inferencing stopped by user\r\n");
        // EiDevice.set_state(eiStateFinished);
        /* reset samples buffer */
        samples_wr_index = 0;
        ei_microphone_inference_end();
    }
    state = INFERENCE_STOPPED;
}

bool is_inference_running(void)
{
    return (state != INFERENCE_STOPPED);
}

#endif /* defined(EI_CLASSIFIER_SENSOR) && EI_CLASSIFIER_SENSOR == EI_CLASSIFIER_SENSOR_MICROPHONE */