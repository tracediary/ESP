/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#include <stdio.h>
#include <sdkconfig.h>

#ifdef CONFIG_IDF_TARGET_ESP8266
#include "driver/pwm.h"
#include "driver/gpio.h"
#else
#include "driver/ledc.h"
#endif
#include "esp_log.h"

typedef struct rgb {
    uint8_t r;  // 0-100 %
    uint8_t g;  // 0-100 %
    uint8_t b;  // 0-100 %
} rgb_t;


typedef struct hsp {
    uint16_t h;  // 0-360
    uint16_t s;  // 0-100
    uint16_t b;  // 0-100
} hsp_t;

/* LED numbers below are for ESP-WROVER-KIT */
/* Red LED */
#define LEDC_IO_0 (0)
/* Green LED */
#define LEDC_IO_1 (2)
/* Blued LED */
#define LEDC_IO_2 (4)

#define PWM_DEPTH (1023)
#define PWM_TARGET_DUTY 8192

static hsp_t s_hsb_val;
static uint16_t s_brightness;
static bool s_on = false;

static const char *TAG = "lightbulb";

#ifdef CONFIG_IDF_TARGET_ESP8266
//#define PWM_PERIOD    (500)
#define PWM_PERIOD    (1000000)
#define PWM_IO_NUM    3

// pwm pin number
const uint32_t pin_num[PWM_IO_NUM] = {
    LEDC_IO_0,
    LEDC_IO_1,
    LEDC_IO_2
};

// dutys table, (duty/PERIOD)*depth
/**
uint32_t duties[PWM_IO_NUM] = {
    250, 250, 250,
};
**/

uint32_t duties[PWM_IO_NUM] = {
    500000, 500000, 500000,
};

// phase table, (phase/180)*depth
int16_t phase[PWM_IO_NUM] = {
    0, 0, 50,
};

#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_1 1
#define LEDC_CHANNEL_2 2


extern void gpio_isr_handler(void *arg);

#endif

/**
 * @brief transform lightbulb's "RGB" and other parameter
 */
static void lightbulb_set_aim(uint32_t r, uint32_t g, uint32_t b, uint32_t cw, uint32_t ww, uint32_t period)
{
#ifdef CONFIG_IDF_TARGET_ESP8266
    pwm_stop(0x3);
    pwm_set_duty(LEDC_CHANNEL_0, r);
    pwm_set_duty(LEDC_CHANNEL_1, g);
    pwm_set_duty(LEDC_CHANNEL_2, b);
    pwm_start();
#else
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, r);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1, g);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2, b);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_2);
#endif
}

/**
 * @brief transform lightbulb's "HSV" to "RGB"
 */
static bool lightbulb_set_hsb2rgb(uint16_t h, uint16_t s, uint16_t v, rgb_t *rgb)
{
    bool res = true;
    uint16_t hi, F, P, Q, T;

    if (!rgb)
        return false;

    if (h > 360) return false;
    if (s > 100) return false;
    if (v > 100) return false;

    hi = (h / 60) % 6;
    F = 100 * h / 60 - 100 * hi;
    P = v * (100 - s) / 100;
    Q = v * (10000 - F * s) / 10000;
    T = v * (10000 - s * (100 - F)) / 10000;

    switch (hi) {
    case 0:
        rgb->r = v;
        rgb->g = T;
        rgb->b = P;
        break;
    case 1:
        rgb->r = Q;
        rgb->g = v;
        rgb->b = P;
        break;
    case 2:
        rgb->r = P;
        rgb->g = v;
        rgb->b = T;
        break;
    case 3:
        rgb->r = P;
        rgb->g = Q;
        rgb->b = v;
        break;
    case 4:
        rgb->r = T;
        rgb->g = P;
        rgb->b = v;
        break;
    case 5:
        rgb->r = v;
        rgb->g = P;
        rgb->b = Q;
        break;
    default:
        return false;
    }
    return res;
}

/**
 * @brief set the lightbulb's "HSV"
 */
static bool lightbulb_set_aim_hsv(uint16_t h, uint16_t s, uint16_t v)
{
    rgb_t rgb_tmp;
    bool ret = lightbulb_set_hsb2rgb(h, s, v, &rgb_tmp);

    if (ret == false)
        return false;

    lightbulb_set_aim(rgb_tmp.r * PWM_TARGET_DUTY / 100, rgb_tmp.g * PWM_TARGET_DUTY / 100,
            rgb_tmp.b * PWM_TARGET_DUTY / 100, (100 - s) * 5000 / 100, v * 2000 / 100, 1000);

    return true;
}

/**
 * @brief update the lightbulb's state
 */
static void lightbulb_update()
{
    lightbulb_set_aim_hsv(s_hsb_val.h, s_hsb_val.s, s_hsb_val.b);
}

#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_1)






void key_init(void)
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //bit mask of the pins, use GPIO4/5 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *) GPIO_INPUT_IO_1);

}





/**
 * @brief initialize the lightbulb lowlevel module
 */
void lightbulb_init(void)
{

    pwm_init(PWM_PERIOD, duties, PWM_IO_NUM, pin_num);
    pwm_set_channel_invert(0x1 << 0);
    pwm_set_phases(phase);
    pwm_start();
}

/**
 * @brief deinitialize the lightbulb's lowlevel module
 */
void lightbulb_deinit(void)
{
    pwm_stop(0x03);
    pwm_deinit();

}

/**
 * @brief turn on/off the lowlevel lightbulb
 */
int lightbulb_set_on(bool value)
{
    ESP_LOGI(TAG, "lightbulb_set_on : %s", value == true ? "true" : "false");

    if (value == true) {
        s_hsb_val.b = s_brightness;
        s_on = true;
    } else {
        s_brightness = s_hsb_val.b;
        s_hsb_val.b = 0;
        s_on = false;
    }
    lightbulb_update();

    return 0;
}

/**
 * @brief set the saturation of the lowlevel lightbulb
 */
int lightbulb_set_saturation(float value)
{
    ESP_LOGI(TAG, "lightbulb_set_saturation : %f", value);

    s_hsb_val.s = value;
    if (true == s_on)
        lightbulb_update();

    return 0;
}

/**
 * @brief set the hue of the lowlevel lightbulb
 */
int lightbulb_set_hue(float value)
{
    ESP_LOGI(TAG, "lightbulb_set_hue : %f", value);

    s_hsb_val.h = value;
    if (true == s_on)
        lightbulb_update();

    return 0;
}
#if 0
/**
 * @brief set the brightness of the lowlevel lightbulb
 */
int lightbulb_set_brightness(int value)
{
    ESP_LOGI(TAG, "lightbulb_set_brightness : %d", value);

    s_hsb_val.b = value;
    s_brightness = s_hsb_val.b; 
    if (true == s_on)
        lightbulb_update();

    return 0;
}
#endif


void lightbulb_stop(void)
{
    pwm_stop(0);
    s_on = false;
}


void lightbulb_start(void)
{
    pwm_start();
    s_on = true;
}

/**
 * @brief set the powerstate of lightbulb
 */
int lightbulb_set_powerstate(bool value)
{
    ESP_LOGI(TAG, "lightbulb_set_powerstate : %s my log to set git ignore", value == true ? "true" : "false");

    if (true == value)
    {
        lightbulb_start();
    }
    else
    {
        /* code */
        lightbulb_stop();
    }

    return 0;
}

/**
 * @brief set the brightness of lightbulb
 */
int lightbulb_set_brightness(int value)
{
    ESP_LOGI(TAG, "lightbulb_set_brightness : %d,this is111111111111111111", value);
    if (value > 100 || value < 1)
    {
        ESP_LOGE(TAG, "lightbulb_set_brightness value is error");
        return -1;
    }

    for (size_t i = 0; i < PWM_IO_NUM; i++)
    {
        duties[i] = value*10000;
    }

    lightbulb_stop();    
    pwm_set_duties(duties);
    lightbulb_start();

    return 0;
}

bool lightbulb_get_powerstate()
{
    return s_on;
}

int lightbulb_get_brightness()
{
    return duties[1];
}
