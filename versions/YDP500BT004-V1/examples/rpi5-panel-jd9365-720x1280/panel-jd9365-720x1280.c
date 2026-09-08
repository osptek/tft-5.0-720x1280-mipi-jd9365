// SPDX-License-Identifier: GPL-2.0+
/*
 * DRM driver for Jadard JD9365 720x1280 2-Lane MIPI-DSI panel
 *
 * This example is from the open-source sharing by engineers of Yuying Optoelectronics (鱼鹰光电)
 * on Github.com/osptek. Welcome to provide improvement suggestions.
 *
 * 本例程来源于鱼鹰光电的工程师的开源分享 Github.com/osptek，欢迎提出改进意见
 */

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>
#include <drm/drm_print.h>

#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

struct jadard;

struct jadard_panel_desc {
	const struct drm_display_mode mode;
	unsigned int lanes;
	enum mipi_dsi_pixel_format format;
	int (*init)(struct jadard *jadard);
	unsigned long flags;
};

struct jadard {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	const struct jadard_panel_desc *desc;
	enum drm_panel_orientation orientation;
	struct gpio_desc *reset;
};

static inline struct jadard *panel_to_jadard(struct drm_panel *panel)
{
	return container_of(panel, struct jadard, panel);
}

static int jd9365_720x1280_init(struct jadard *jadard)
{
	struct mipi_dsi_multi_context ctx = { .dsi = jadard->dsi };

        /* Page0 解锁密码页 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x00);
	/* PASSWORD */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE1, 0x93);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE2, 0x65);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE3, 0xF8);
	
	/* 设置双通道 (2-Lane) */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x80, 0x01);

	/* Page1 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x01);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x00, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x01, 0x55);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x03, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x04, 0x55);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x17, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x18, 0xAF);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x19, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1A, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1B, 0xAF);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1C, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1F, 0x7E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x22, 0x4E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x23, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x24, 0xFE);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x25, 0x20);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x27, 0x22);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x35, 0x23);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x37, 0x09);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x38, 0x04);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x40, 0x44);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x41, 0xA0);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x42, 0x81);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x43, 0x18);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x44, 0x1C);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x45, 0x10);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x55, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x57, 0x69);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5A, 0x1B);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5B, 0x1B);

	/* Gamma 2.2 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5D, 0x7F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5E, 0x66);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5F, 0x57);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x60, 0x4B);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x61, 0x47);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x62, 0x39);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x63, 0x3D);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x64, 0x27);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x65, 0x40);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x66, 0x3F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x67, 0x40);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x68, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x69, 0x4E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6A, 0x56);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6B, 0x49);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6C, 0x44);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6D, 0x35);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6E, 0x22);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6F, 0x07);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x70, 0x7F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x71, 0x66);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x72, 0x57);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x73, 0x4B);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x74, 0x47);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x75, 0x39);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x76, 0x3D);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x77, 0x27);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x78, 0x40);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x79, 0x3F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7A, 0x40);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7B, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7C, 0x4E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7D, 0x56);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7E, 0x49);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7F, 0x44);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x80, 0x35);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x81, 0x22);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x82, 0x07);

	/* Page2 GIP配置 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x00, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x01, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x02, 0x42);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x03, 0x40);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x04, 0x48);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x05, 0x4A);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x06, 0x44);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x07, 0x46);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x08, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x09, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0A, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0B, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0C, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0D, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0E, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0F, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x10, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x11, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x12, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x13, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x14, 0x5E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x15, 0x5E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x16, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x17, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x18, 0x43);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x19, 0x41);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1A, 0x49);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1B, 0x4B);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1C, 0x45);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1D, 0x47);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1E, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x1F, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x20, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x21, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x22, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x23, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x24, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x25, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x26, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x27, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x28, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x29, 0x5F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2A, 0x5E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2B, 0x5E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2C, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2D, 0x1E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2E, 0x01);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x2F, 0x03);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x30, 0x07);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x31, 0x05);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x32, 0x0B);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x33, 0x09);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x34, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x35, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x36, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x37, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x38, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x39, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3A, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3B, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3C, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3D, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3E, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x3F, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x40, 0x1E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x41, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x42, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x43, 0x1E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x44, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x45, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x46, 0x06);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x47, 0x04);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x48, 0x0A);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x49, 0x08);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4A, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4B, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4C, 0x12);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4D, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4E, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x4F, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x50, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x51, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x52, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x53, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x54, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x55, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x56, 0x1E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x57, 0x1F);

	/* GIP Timing */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x58, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x59, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5A, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5B, 0x30);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5C, 0x14);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5D, 0x30);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5E, 0x01);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x5F, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x60, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x61, 0x01);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x62, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x63, 0x03);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x64, 0x64);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x65, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x66, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x67, 0x73);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x68, 0x16);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x69, 0x1C);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6A, 0x68);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x6B, 0x08);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x77, 0xDD);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x79, 0x0E);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7A, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7D, 0x1F);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x7E, 0x37);

	/* Page4 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x04);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x04, 0x01);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x09, 0x10);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0E, 0x2A);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x0F, 0x87);

	/* Page5 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x05);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0x15, 0x1B);

	/* 切回Page0 + 解锁后寄存器 */
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE0, 0x00);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE6, 0x02);
	mipi_dsi_dcs_write_seq_multi(&ctx, 0xE7, 0x1F);

	/* 休眠退出 + 延时120ms */
	mipi_dsi_dcs_exit_sleep_mode_multi(&ctx);
	mipi_dsi_msleep(&ctx, 120);

	/* 开显示 + 延时5ms */
	mipi_dsi_dcs_set_display_on_multi(&ctx);
	mipi_dsi_msleep(&ctx, 5);

	return ctx.accum_err;
}

static int jadard_prepare(struct drm_panel *panel)
{
	struct jadard *jadard = panel_to_jadard(panel);
	int ret;

	if (jadard->reset) {
		gpiod_set_value_cansleep(jadard->reset, 0);
		msleep(10);
		gpiod_set_value_cansleep(jadard->reset, 1);
		msleep(20);
		gpiod_set_value_cansleep(jadard->reset, 0);
		msleep(120);
	}

	ret = jadard->desc->init(jadard);
	if (ret)
		return ret;

	return 0;
}

static int jadard_enable(struct drm_panel *panel)
{
	struct mipi_dsi_multi_context ctx = { .dsi = to_mipi_dsi_device(panel->dev) };

	mipi_dsi_dcs_set_display_on_multi(&ctx);
	return ctx.accum_err;
}

static int jadard_disable(struct drm_panel *panel)
{
	struct mipi_dsi_multi_context ctx = { .dsi = to_mipi_dsi_device(panel->dev) };

	mipi_dsi_dcs_set_display_off_multi(&ctx);
	return ctx.accum_err;
}

static int jadard_unprepare(struct drm_panel *panel)
{
	struct jadard *jadard = panel_to_jadard(panel);
	struct mipi_dsi_multi_context ctx = { .dsi = jadard->dsi };

	mipi_dsi_dcs_enter_sleep_mode_multi(&ctx);
	msleep(120);

	if (jadard->reset)
		gpiod_set_value_cansleep(jadard->reset, 1);

	return ctx.accum_err;
}

static int jadard_get_modes(struct drm_panel *panel,
			    struct drm_connector *connector)
{
	struct jadard *jadard = panel_to_jadard(panel);
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, &jadard->desc->mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	drm_mode_probed_add(connector, mode);

	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;

	return 1;
}

static enum drm_panel_orientation jadard_get_orientation(struct drm_panel *panel)
{
	return panel_to_jadard(panel)->orientation;
}

static const struct drm_panel_funcs jadard_funcs = {
	.prepare = jadard_prepare,
	.enable = jadard_enable,
	.disable = jadard_disable,
	.unprepare = jadard_unprepare,
	.get_modes = jadard_get_modes,
	.get_orientation = jadard_get_orientation,
};

static const struct jadard_panel_desc jd9365_720x1280_desc = {
	.mode = {
		.clock = 63400,                          /* 63.4 MHz */

		.hdisplay = 720,
		.hsync_start = 720 + 42,                 /* front porch */
		.hsync_end   = 720 + 42 + 8,             /* + pulse */
		.htotal      = 720 + 42 + 8 + 42,        /* + back porch = 812 */

		.vdisplay = 1280,
		.vsync_start = 1280 + 24,
		.vsync_end   = 1280 + 24 + 4,
		.vtotal      = 1280 + 24 + 4 + 12,       /* = 1320 */

		.width_mm  = 62,                         /* 5寸屏幕 */
		.height_mm = 110,
		.type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
	},
	.lanes = 2,
	.format = MIPI_DSI_FMT_RGB888,
	.flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST | MIPI_DSI_MODE_LPM,
	.init = jd9365_720x1280_init,
};

static int jadard_dsi_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct jadard_panel_desc *desc;
	struct jadard *jadard;
	int ret;

	jadard = devm_kzalloc(dev, sizeof(*jadard), GFP_KERNEL);
	if (!jadard)
		return -ENOMEM;

	desc = of_device_get_match_data(dev);
	dsi->mode_flags = desc->flags;
	dsi->format = desc->format;
	dsi->lanes = desc->lanes;

	jadard->reset = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(jadard->reset))
		return dev_err_probe(dev, PTR_ERR(jadard->reset), "Failed to get reset GPIO\n");

	ret = of_drm_get_panel_orientation(dev->of_node, &jadard->orientation);
	if (ret < 0)
		jadard->orientation = DRM_MODE_PANEL_ORIENTATION_NORMAL;

	drm_panel_init(&jadard->panel, dev, &jadard_funcs, DRM_MODE_CONNECTOR_DSI);
	jadard->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&jadard->panel);
	if (ret)
		return ret;

	drm_panel_add(&jadard->panel);

	mipi_dsi_set_drvdata(dsi, jadard);
	jadard->dsi = dsi;
	jadard->desc = desc;

	ret = mipi_dsi_attach(dsi);
	if (ret)
		drm_panel_remove(&jadard->panel);

	return ret;
}

static void jadard_dsi_remove(struct mipi_dsi_device *dsi)
{
	struct jadard *jadard = mipi_dsi_get_drvdata(dsi);

	mipi_dsi_detach(dsi);
	drm_panel_remove(&jadard->panel);
}

static const struct of_device_id jadard_of_match[] = {
	{ .compatible = "jadard,jd9365-720x1280", .data = &jd9365_720x1280_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, jadard_of_match);

static struct mipi_dsi_driver jadard_driver = {
	.probe = jadard_dsi_probe,
	.remove = jadard_dsi_remove,
	.driver = {
		.name = "panel-jadard-jd9365",
		.of_match_table = jadard_of_match,
	},
};
module_mipi_dsi_driver(jadard_driver);

MODULE_AUTHOR("Adapted for JD9365 720x1280");
MODULE_DESCRIPTION("Jadard JD9365 720x1280 2-Lane MIPI-DSI Panel Driver");
MODULE_LICENSE("GPL");
