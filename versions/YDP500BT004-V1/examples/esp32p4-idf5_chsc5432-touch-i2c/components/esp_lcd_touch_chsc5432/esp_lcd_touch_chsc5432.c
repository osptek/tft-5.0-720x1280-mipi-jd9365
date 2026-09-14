/*
 * SPDX-FileCopyrightText: 2024-2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"

static const char *TAG = "CHSC5432";

/* CHSC5432 registers / commands
 * Actual I2C transaction uses 4-byte prefix: 0x20 0x00 0x00 <reg>
 * We encode it as a 32-bit command: 0x20000000 | reg
 */
#define CHSC5432_REG_TOUCH_DATA     (0x2C)
#define CHSC5432_REG_IC_TYPE        (0x81)
#define CHSC5432_IC_TYPE_VALUE      (0x05)

#define CHSC5432_MAX_POINTS         (5)
#define CHSC5432_TOUCH_DATA_LEN     (28)

/*******************************************************************************
 * Function definitions
 *******************************************************************************/
static esp_err_t esp_lcd_touch_chsc5432_read_data(esp_lcd_touch_handle_t tp);
static bool esp_lcd_touch_chsc5432_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y,
                                          uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
static esp_err_t esp_lcd_touch_chsc5432_del(esp_lcd_touch_handle_t tp);

/* I2C helpers */
static esp_err_t touch_chsc5432_i2c_read(esp_lcd_touch_handle_t tp, uint8_t reg, uint8_t *data, size_t len);

/* Reset */
static esp_err_t touch_chsc5432_reset(esp_lcd_touch_handle_t tp);

/*******************************************************************************
 * Public API functions
 *******************************************************************************/

esp_err_t esp_lcd_touch_new_i2c_chsc5432(const esp_lcd_panel_io_handle_t io,
                                         const esp_lcd_touch_config_t *config,
                                         esp_lcd_touch_handle_t *out_touch)
{
    esp_err_t ret = ESP_OK;

    ESP_RETURN_ON_FALSE(io != NULL, ESP_ERR_INVALID_ARG, TAG, "Touch controller io handle can't be NULL");
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG,
                        "Pointer to the touch controller configuration can't be NULL");
    ESP_RETURN_ON_FALSE(out_touch != NULL, ESP_ERR_INVALID_ARG, TAG,
                        "Pointer to the touch controller handle can't be NULL");

    /* Prepare main structure */
    esp_lcd_touch_handle_t tp = heap_caps_calloc(1, sizeof(esp_lcd_touch_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(tp, ESP_ERR_NO_MEM, err, TAG, "no mem for CHSC5432 controller");

    /* Communication interface */
    tp->io = io;

    /* Only supported callbacks are set */
    tp->read_data = esp_lcd_touch_chsc5432_read_data;
    tp->get_xy = esp_lcd_touch_chsc5432_get_xy;
    tp->del = esp_lcd_touch_chsc5432_del;

    /* Mutex */
    tp->data.lock.owner = portMUX_FREE_VAL;

    /* Save config */
    memcpy(&tp->config, config, sizeof(esp_lcd_touch_config_t));

    /* Prepare pin for touch interrupt */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = (tp->config.levels.interrupt ? GPIO_INTR_POSEDGE : GPIO_INTR_NEGEDGE),
            .pin_bit_mask = BIT64(tp->config.int_gpio_num)
        };
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");

        /* Register interrupt callback */
        if (tp->config.interrupt_callback) {
            esp_lcd_touch_register_interrupt_callback(tp, tp->config.interrupt_callback);
        }
    }

    /* Prepare pin for touch controller reset */
    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t rst_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = BIT64(tp->config.rst_gpio_num)
        };
        ret = gpio_config(&rst_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");
    }

    /* Reset controller */
    ret = touch_chsc5432_reset(tp);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "CHSC5432 reset failed");

    /* Optional: read IC type (failure is non-fatal) */
    uint8_t id = 0;
    if (touch_chsc5432_i2c_read(tp, CHSC5432_REG_IC_TYPE, &id, 1) == ESP_OK) {
        ESP_LOGI(TAG, "IC Type = 0x%02X %s", id, (id == CHSC5432_IC_TYPE_VALUE) ? "(CHSC5432)" : "");
    } else {
        ESP_LOGW(TAG, "IC Type read failed (ignore, touch still works)");
    }

    *out_touch = tp;

err:
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (0x%x)! Touch controller CHSC5432 initialization failed!", ret);
        if (tp) {
            esp_lcd_touch_chsc5432_del(tp);
        }
    }

    return ret;
}

static esp_err_t esp_lcd_touch_chsc5432_read_data(esp_lcd_touch_handle_t tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "Touch controller handle can't be NULL");

    uint8_t data[CHSC5432_TOUCH_DATA_LEN] = {0};
    esp_err_t err = touch_chsc5432_i2c_read(tp, CHSC5432_REG_TOUCH_DATA, data, CHSC5432_TOUCH_DATA_LEN);
    ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

    uint8_t fingers = data[1] & 0x0F;
    if (fingers == 0 || fingers > CHSC5432_MAX_POINTS) {
        portENTER_CRITICAL(&tp->data.lock);
        tp->data.points = 0;
        portEXIT_CRITICAL(&tp->data.lock);
        return ESP_OK;
    }

    if (fingers > CONFIG_ESP_LCD_TOUCH_MAX_POINTS) {
        fingers = CONFIG_ESP_LCD_TOUCH_MAX_POINTS;
    }

    portENTER_CRITICAL(&tp->data.lock);

    tp->data.points = fingers;

    for (int i = 0; i < fingers; i++) {
        /* 正点原子官方竖屏解析格式 */
        uint16_t x = ((data[5 + i * 5] & 0x0F) << 8) | data[2 + i * 5];
        uint16_t y = (((data[5 + i * 5] & 0xF0) >> 4) << 8) | data[3 + i * 5];
        uint16_t strength = data[4 + i * 5];

        /* Clamp to configured max (mirror / swap applied later by esp_lcd_touch) */
        if (x > tp->config.x_max) {
            x = tp->config.x_max;
        }
        if (y > tp->config.y_max) {
            y = tp->config.y_max;
        }

        tp->data.coords[i].x = x;
        tp->data.coords[i].y = y;
        tp->data.coords[i].strength = strength;
        tp->data.coords[i].track_id = i;   /* CHSC5432 does not report track id */
    }

    portEXIT_CRITICAL(&tp->data.lock);

    return ESP_OK;
}

static bool esp_lcd_touch_chsc5432_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y,
                                          uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    ESP_RETURN_ON_FALSE(tp != NULL, false, TAG, "Touch controller handle can't be NULL");
    ESP_RETURN_ON_FALSE(x != NULL, false, TAG, "Pointer to the x coordinates array can't be NULL");
    ESP_RETURN_ON_FALSE(y != NULL, false, TAG, "Pointer to the y coordinates array can't be NULL");
    ESP_RETURN_ON_FALSE(point_num != NULL, false, TAG, "Pointer to number of touch points can't be NULL");
    ESP_RETURN_ON_FALSE(max_point_num > 0, false, TAG, "Array size must be equal or larger than 1");

    portENTER_CRITICAL(&tp->data.lock);

    /* Count of points */
    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);

    for (size_t i = 0; i < *point_num; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }

    /* Invalidate */
    tp->data.points = 0;

    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t esp_lcd_touch_chsc5432_del(esp_lcd_touch_handle_t tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "Touch controller handle can't be NULL");

    /* Reset GPIO pin settings */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.int_gpio_num);
        if (tp->config.interrupt_callback) {
            gpio_isr_handler_remove(tp->config.int_gpio_num);
        }
    }

    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.rst_gpio_num);
    }

    free(tp);

    return ESP_OK;
}

/*******************************************************************************
 * Private API function
 *******************************************************************************/

static esp_err_t touch_chsc5432_reset(esp_lcd_touch_handle_t tp)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "Touch controller handle can't be NULL");

    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset),
                            TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(20));
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset),
                            TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(100));
    } else {
        /* No reset pin: just wait a bit for the controller to settle */
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    return ESP_OK;
}

/**
 * @brief Read register from CHSC5432
 *
 * The controller expects a 4-byte command: {0x20, 0x00, 0x00, reg}
 * We encode it as a 32-bit LCD command (big-endian).
 */
static esp_err_t touch_chsc5432_i2c_read(esp_lcd_touch_handle_t tp, uint8_t reg, uint8_t *data, size_t len)
{
    ESP_RETURN_ON_FALSE(tp != NULL, ESP_ERR_INVALID_ARG, TAG, "Touch controller handle can't be NULL");
    ESP_RETURN_ON_FALSE(data != NULL, ESP_ERR_INVALID_ARG, TAG, "Data pointer can't be NULL");

    /* 0x20 0x00 0x00 reg  ->  0x20000000 | reg */
    int lcd_cmd = (int)(0x20000000U | (uint32_t)reg);

    return esp_lcd_panel_io_rx_param(tp->io, lcd_cmd, data, len);
}