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
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <stdio.h>

#include "ei_device_espressif_esp32.h"

#include "ei_at_handlers.h"
#include "ei_classifier_porting.h"
#include "ei_run_impulse.h"

#include "ei_analogsensor.h"
#include "ei_inertial_sensor.h"

EiDeviceInfo *EiDevInfo = dynamic_cast<EiDeviceInfo *>(EiDeviceESP32::get_device());
static ATServer *at;

/* Private variables ------------------------------------------------------- */
#define GPIO_OUTPUT_WIO_IO2 GPIO_NUM_27
#define GPIO_OUTPUT_LED_GREEN GPIO_NUM_12
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_WIO_IO2) | (1ULL << GPIO_OUTPUT_LED_GREEN))
/* Public functions -------------------------------------------------------- */

extern "C" int app_main()
{
    gpio_pad_select_gpio(GPIO_NUM_21);
    gpio_reset_pin(GPIO_NUM_21);

    gpio_pad_select_gpio(GPIO_NUM_22);
    gpio_reset_pin(GPIO_NUM_22);

    gpio_set_direction(GPIO_NUM_21, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_22, GPIO_MODE_OUTPUT);

    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    gpio_config(&io_conf);

    /* Initialize Edge Impulse sensors and commands */

    EiDeviceESP32 *dev = static_cast<EiDeviceESP32 *>(EiDeviceESP32::get_device());

    ei_printf(
        "Hello from Edge Impulse Device SDK.\r\n"
        "Compiled on %s %s\r\n",
        __DATE__,
        __TIME__);

    /* Setup the inertial sensor */
    if (ei_inertial_init() == false)
    {
        ei_printf("Inertial sensor initialization failed\r\n");
    }

    /* Setup the analog sensor */
    if (ei_analog_sensor_init() == false)
    {
        ei_printf("ADC sensor initialization failed\r\n");
    }

    at = ei_at_init(dev);
    ei_printf("Type AT+HELP to see a list of commands.\r\n");
    at->print_prompt();

    dev->set_state(eiStateFinished);

    while (1)
    {
        gpio_set_level(GPIO_OUTPUT_WIO_IO2, 1);
        //gpio_set_level(GPIO_OUTPUT_LED_GREEN, 1);
        /* handle command comming from uart */
        //ei_start_impulse(true, false);
        char data = ei_get_serial_byte();

        while (data != 0xFF)
        {
            at->handle(data);
            data = ei_get_serial_byte();
        }
    }
}