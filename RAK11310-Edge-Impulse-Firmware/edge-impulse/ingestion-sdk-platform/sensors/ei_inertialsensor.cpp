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
#include <stdint.h>
#include <stdlib.h>

#include "ei_inertialsensor.h"

#include "ei_device_raspberry_rp2040.h"
#include "sensor_aq.h"

// LIS3DH Libraries
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// By default this device is on bus address 0x18

/* Constant defines -------------------------------------------------------- */

//#define i2c_default  1;
#define PICO_DEFAULT_I2C_SDA_PIN 2
#define PICO_DEFAULT_I2C_SCL_PIN 3

#define CONVERT_G_TO_MS2 9.80665f

const int ADDRESS = 0x18;
const uint8_t CTRL_REG_1 = 0x20;
const uint8_t CTRL_REG_4 = 0x23;
const uint8_t TEMP_CFG_REG = 0xC0;

float x_accel, y_accel, z_accel;

static float imu_data[INERTIAL_AXIS_SAMPLED];

bool ei_inertial_sensor_init(void)
{

    stdio_init_all();

    // printf("Hello, LIS3DH! Reading raw data from registers...\n");

    // This example will use I2C1 on the SDA and SCL pins (2, 3 on a Pico)
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    ///////////////////////////////////////////////////////////

    uint8_t buf[2];

    // Turn normal mode and 1.344kHz data rate on
    buf[0] = CTRL_REG_1;
    buf[1] = 0x97;
    i2c_write_blocking(i2c1, ADDRESS, buf, 2, false);

    // Turn block data update on (for temperature sensing)
    buf[0] = CTRL_REG_4;
    buf[1] = 0x80;
    i2c_write_blocking(i2c1, ADDRESS, buf, 2, false);

    // Turn auxillary ADC on
    buf[0] = TEMP_CFG_REG;
    buf[1] = 0xC0;
    i2c_write_blocking(i2c1, ADDRESS, buf, 2, false);
    ei_add_sensor_to_fusion_list(inertial_sensor);
    return true;
}

void lis3dh_calc_value(uint16_t raw_value, float *final_value, bool isAccel)
{
    // Convert with respect to the value being temperature or acceleration reading
    float scaling;
    float senstivity = 0.004f; // g per unit

    if (isAccel == true)
    {
        scaling = 64 / senstivity;
    }
    else
    {
        scaling = 64;
    }

    // raw_value is signed
    *final_value = (float)((int16_t)raw_value) / scaling;
}

void lis3dh_read_data(uint8_t reg, float *final_value, bool IsAccel)
{
    // Read two bytes of data and store in a 16 bit data structure
    uint8_t lsb;
    uint8_t msb;
    uint16_t raw_accel;
    i2c_write_blocking(i2c1, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c1, ADDRESS, &lsb, 1, false);

    reg |= 0x01;
    i2c_write_blocking(i2c1, ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c1, ADDRESS, &msb, 1, false);

    raw_accel = (msb << 8) | lsb;

    lis3dh_calc_value(raw_accel, final_value, IsAccel);
}

float *ei_fusion_inertial_sensor_read_data(int n_samples)
{

    lis3dh_read_data(0x28, &x_accel, true);
    lis3dh_read_data(0x2A, &y_accel, true);
    lis3dh_read_data(0x2C, &z_accel, true);

    imu_data[0] = x_accel * CONVERT_G_TO_MS2;
    imu_data[1] = y_accel * CONVERT_G_TO_MS2;
    imu_data[2] = z_accel * CONVERT_G_TO_MS2;

    return imu_data;
}
