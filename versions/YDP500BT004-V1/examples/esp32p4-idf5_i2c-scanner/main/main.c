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
#include "esp_log.h"

static const char *TAG = "i2c_scanner";

#define I2C_MASTER_SCL_IO           8      // 修改为你的 SCL 引脚
#define I2C_MASTER_SDA_IO           7      // 修改为你的 SDA 引脚
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000  // 扫描建议用 100kHz

void i2c_scan(void)
{
    i2c_master_bus_config_t i2c_bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_MASTER_NUM,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config, &bus_handle));

    ESP_LOGI(TAG, "I2C Scanner started...");
    ESP_LOGI(TAG, "     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    for (int i = 0; i < 128; i += 16) {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++) {
            uint8_t address = i + j;

            // 跳过保留地址
            if (address < 0x08 || address > 0x77) {
                printf("   ");
                continue;
            }

            esp_err_t ret = i2c_master_probe(bus_handle, address, 50);  // 50ms 超时
            if (ret == ESP_OK) {
                printf("%02x ", address);
            } else {
                printf("-- ");
            }
        }
        printf("\n");
    }

    ESP_LOGI(TAG, "I2C scan done.");
    // 如果需要，可以删除 bus
    // i2c_del_master_bus(bus_handle);
}

void app_main(void)
{
    i2c_scan();
}
