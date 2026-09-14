/*
 * SPDX-FileCopyrightText: 2024-2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP LCD touch: CHSC5432
 */

#pragma once

#include "esp_lcd_touch.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a new CHSC5432 touch driver
 *
 * @note The I2C communication should be initialized before use this function.
 *
 * @param[in]  io LCD/Touch panel IO handle
 * @param[in]  config Touch configuration
 * @param[out] out_touch Touch instance handle
 * @return
 *      - ESP_OK                    on success
 *      - ESP_ERR_NO_MEM            if there is no memory for allocating main structure
 *      - ESP_ERR_INVALID_ARG       if parameter is invalid
 */
esp_err_t esp_lcd_touch_new_i2c_chsc5432(const esp_lcd_panel_io_handle_t io,
                                         const esp_lcd_touch_config_t *config,
                                         esp_lcd_touch_handle_t *out_touch);

/**
 * @brief I2C address of the CHSC5432 controller
 */
#define ESP_LCD_TOUCH_IO_I2C_CHSC5432_ADDRESS    (0x2E)

/**
 * @brief Touch IO configuration structure for CHSC5432
 *
 * CHSC5432 uses a 4-byte command prefix: {0x20, 0x00, 0x00, reg}
 * Therefore lcd_cmd_bits must be 32.
 */
#define ESP_LCD_TOUCH_IO_I2C_CHSC5432_CONFIG()           \
    {                                                    \
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CHSC5432_ADDRESS, \
        .scl_speed_hz = 100000,                          \
        .control_phase_bytes = 1,                        \
        .dc_bit_offset = 0,                              \
        .lcd_cmd_bits = 32,                              \
        .flags =                                         \
        {                                                \
            .disable_control_phase = 1,                  \
        }                                                \
    }

#ifdef __cplusplus
}
#endif