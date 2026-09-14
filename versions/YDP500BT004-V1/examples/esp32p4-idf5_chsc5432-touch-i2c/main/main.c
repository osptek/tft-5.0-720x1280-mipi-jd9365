/*
 * SPDX-FileCopyrightText: Copyright 2026 OSPTEK
 * SPDX-License-Identifier: CC-BY-4.0
 *
 * https://github.com/osptek
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_chsc5432.h"

static const char *TAG = "CHSC5432_DEMO";

/* ===================== 根据实际硬件修改 ===================== */
#define I2C_MASTER_SCL_IO           8
#define I2C_MASTER_SDA_IO           7
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000

#define TOUCH_RST_GPIO              GPIO_NUM_NC   // 有复位脚就改成实际 GPIO
#define TOUCH_INT_GPIO              GPIO_NUM_NC   // 有中断脚就改成实际 GPIO

#define TOUCH_X_MAX                 720
#define TOUCH_Y_MAX                 1280
#define MAX_TOUCH_POINTS            5
/* ========================================================== */

void app_main(void)
{
    /* 1. 创建 I2C 总线（新驱动） */
    i2c_master_bus_config_t bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t i2c_bus = NULL;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &i2c_bus));

    /* 2. 创建 Touch 专用的 panel_io */
    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CHSC5432_CONFIG();
    tp_io_config.scl_speed_hz = I2C_MASTER_FREQ_HZ;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &tp_io_config, &tp_io_handle));

    /* 3. 配置触摸参数 */
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = TOUCH_X_MAX,
        .y_max = TOUCH_Y_MAX,
        .rst_gpio_num = TOUCH_RST_GPIO,
        .int_gpio_num = TOUCH_INT_GPIO,
        .levels = {
            .reset = 0,          // 复位有效电平（0=低有效）
            .interrupt = 0,      // 中断有效电平
        },
        .flags = {
            .swap_xy = 0,        // 是否交换 XY
            .mirror_x = 0,       // 是否镜像 X
            .mirror_y = 0,       // 是否镜像 Y
        },
        // .interrupt_callback = NULL,  // 如需中断可在此注册
    };

    /* 4. 创建 CHSC5432 触摸驱动实例 */
    esp_lcd_touch_handle_t tp = NULL;
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_chsc5432(tp_io_handle, &tp_cfg, &tp));
    ESP_LOGI(TAG, "Touch ready (up to %d points), start reading...", MAX_TOUCH_POINTS);

    /* 5. 轮询读取坐标 */
    while (1) {
    // 从控制器读取最新数据到内部缓存
    esp_lcd_touch_read_data(tp);

    esp_lcd_touch_point_data_t points[MAX_TOUCH_POINTS] = {0};
    uint8_t point_num = 0;

    // 使用新 API 获取触摸点数据
    esp_err_t ret = esp_lcd_touch_get_data(tp, points, &point_num, MAX_TOUCH_POINTS);
    if (ret == ESP_OK && point_num > 0) {
        for (int i = 0; i < point_num; i++) {
            ESP_LOGI(TAG, "Point[%d]: X=%4d  Y=%4d  Strength=%3d  TrackID=%d",
                     i,
                     points[i].x,
                     points[i].y,
                     points[i].strength,
                     points[i].track_id);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(30));
   }
}