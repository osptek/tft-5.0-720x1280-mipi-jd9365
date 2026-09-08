// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for Chipsemi CHSC5432 capacitive touchscreen controller
 *
 * Supports multi-touch (up to 5 points), optional reset GPIO and IRQ GPIO.
 * If no IRQ is configured, falls back to polling mode.
 * Default resolution: 720 x 1280
 *
 * I2C address: 0x2E
 * Register access uses 4-byte command prefix: 0x20 0x00 0x00 <reg>
 */

#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/interrupt.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/workqueue.h>

#define CHSC5432_NAME			"chsc5432"
#define CHSC5432_MAX_POINTS		5
#define CHSC5432_TOUCH_DATA_LEN		28
#define CHSC5432_POLL_INTERVAL_MS	20	/* 50Hz polling */

/* Registers */
#define CHSC5432_REG_TOUCH_DATA		0x2C
#define CHSC5432_REG_IC_TYPE		0x81
#define CHSC5432_IC_TYPE_VALUE		0x05

struct chsc5432_data {
	struct i2c_client		*client;
	struct input_dev		*input;
	struct gpio_desc		*reset_gpio;
	struct touchscreen_properties	prop;
	struct mutex			mutex;
	bool				suspended;

	/* Polling support */
	struct delayed_work		poll_work;
	bool				polling;
};

/*
 * Read register(s) from CHSC5432.
 * Command format: {0x20, 0x00, 0x00, reg} + repeated start read
 */
static int chsc5432_i2c_read(struct i2c_client *client, u8 reg,
			     u8 *buf, size_t len)
{
	u8 cmd[4] = { 0x20, 0x00, 0x00, reg };
	struct i2c_msg msgs[2] = {
		{
			.addr	= client->addr,
			.flags	= 0,
			.len	= sizeof(cmd),
			.buf	= cmd,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= len,
			.buf	= buf,
		},
	};
	int ret;

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret != 2) {
		dev_err_ratelimited(&client->dev, "i2c transfer failed: %d\n", ret);
		return ret < 0 ? ret : -EIO;
	}

	return 0;
}

static int chsc5432_reset(struct chsc5432_data *ts)
{
	if (!ts->reset_gpio)
		return 0;

	gpiod_set_value_cansleep(ts->reset_gpio, 1);
	msleep(20);
	gpiod_set_value_cansleep(ts->reset_gpio, 0);
	msleep(200);	/* give chip enough time to boot */

	return 0;
}

static int chsc5432_read_touch_data(struct chsc5432_data *ts)
{
	struct input_dev *input = ts->input;
	u8 data[CHSC5432_TOUCH_DATA_LEN];
	u8 fingers;
	int i, ret;

	ret = chsc5432_i2c_read(ts->client, CHSC5432_REG_TOUCH_DATA,
				data, sizeof(data));
	if (ret)
		return ret;

	fingers = data[1] & 0x0F;
	if (fingers > CHSC5432_MAX_POINTS)
		fingers = CHSC5432_MAX_POINTS;

	for (i = 0; i < fingers; i++) {
		u16 x, y, strength;

		x = ((data[5 + i * 5] & 0x0F) << 8) | data[2 + i * 5];
		y = (((data[5 + i * 5] & 0xF0) >> 4) << 8) | data[3 + i * 5];
		strength = data[4 + i * 5];

		input_mt_slot(input, i);
		input_mt_report_slot_state(input, MT_TOOL_FINGER, true);
		touchscreen_report_pos(input, &ts->prop, x, y, true);
		input_report_abs(input, ABS_MT_PRESSURE, strength);
	}

	for (; i < CHSC5432_MAX_POINTS; i++) {
		input_mt_slot(input, i);
		input_mt_report_slot_state(input, MT_TOOL_FINGER, false);
	}

	input_mt_sync_frame(input);
	input_sync(input);

	return 0;
}

static void chsc5432_poll_work(struct work_struct *work)
{
	struct chsc5432_data *ts = container_of(to_delayed_work(work),
						struct chsc5432_data, poll_work);

	mutex_lock(&ts->mutex);
	if (!ts->suspended)
		chsc5432_read_touch_data(ts);
	mutex_unlock(&ts->mutex);

	/* Reschedule next poll */
	if (ts->polling && !ts->suspended)
		schedule_delayed_work(&ts->poll_work,
				      msecs_to_jiffies(CHSC5432_POLL_INTERVAL_MS));
}

static irqreturn_t chsc5432_irq_handler(int irq, void *dev_id)
{
	struct chsc5432_data *ts = dev_id;

	mutex_lock(&ts->mutex);
	if (!ts->suspended)
		chsc5432_read_touch_data(ts);
	mutex_unlock(&ts->mutex);

	return IRQ_HANDLED;
}

static int chsc5432_check_id(struct chsc5432_data *ts)
{
	u8 id = 0;
	int ret;

	ret = chsc5432_i2c_read(ts->client, CHSC5432_REG_IC_TYPE, &id, 1);
	if (ret) {
		dev_warn(&ts->client->dev,
			 "IC Type read failed (non-fatal): %d\n", ret);
		return 0;
	}

	dev_info(&ts->client->dev, "IC Type = 0x%02X %s\n",
		 id, (id == CHSC5432_IC_TYPE_VALUE) ? "(CHSC5432)" : "");

	return 0;
}

static int chsc5432_probe(struct i2c_client *client)
{
	struct chsc5432_data *ts;
	struct input_dev *input;
	struct device *dev = &client->dev;
	int error;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(dev, "I2C functionality not supported\n");
		return -ENODEV;
	}

	ts = devm_kzalloc(dev, sizeof(*ts), GFP_KERNEL);
	if (!ts)
		return -ENOMEM;

	ts->client = client;
	mutex_init(&ts->mutex);
	i2c_set_clientdata(client, ts);

	ts->reset_gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ts->reset_gpio)) {
		error = PTR_ERR(ts->reset_gpio);
		dev_err(dev, "Failed to get reset GPIO: %d\n", error);
		return error;
	}

	error = chsc5432_reset(ts);
	if (error)
		return error;

	chsc5432_check_id(ts);

	input = devm_input_allocate_device(dev);
	if (!input)
		return -ENOMEM;

	ts->input = input;
	input->name = "CHSC5432 Touchscreen";
	input->id.bustype = BUS_I2C;
	input->dev.parent = dev;

	input_set_abs_params(input, ABS_MT_POSITION_X, 0, 719, 0, 0);
	input_set_abs_params(input, ABS_MT_POSITION_Y, 0, 1279, 0, 0);
	input_set_abs_params(input, ABS_MT_PRESSURE, 0, 255, 0, 0);

	touchscreen_parse_properties(input, true, &ts->prop);

	if (!input_abs_get_max(input, ABS_MT_POSITION_X))
		input_set_abs_params(input, ABS_MT_POSITION_X, 0, 719, 0, 0);
	if (!input_abs_get_max(input, ABS_MT_POSITION_Y))
		input_set_abs_params(input, ABS_MT_POSITION_Y, 0, 1279, 0, 0);

	error = input_mt_init_slots(input, CHSC5432_MAX_POINTS,
				    INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	if (error) {
		dev_err(dev, "Failed to init MT slots: %d\n", error);
		return error;
	}

	error = input_register_device(input);
	if (error) {
		dev_err(dev, "Failed to register input device: %d\n", error);
		return error;
	}

	/* IRQ mode preferred */
	if (client->irq > 0) {
		error = devm_request_threaded_irq(dev, client->irq,
						  NULL, chsc5432_irq_handler,
						  IRQF_ONESHOT | IRQF_TRIGGER_FALLING,
						  client->name, ts);
		if (error) {
			dev_err(dev, "Failed to request IRQ %d: %d\n",
				client->irq, error);
			return error;
		}
		dev_info(dev, "Using IRQ %d\n", client->irq);
		ts->polling = false;
	} else {
		/* Fallback to polling */
		INIT_DELAYED_WORK(&ts->poll_work, chsc5432_poll_work);
		ts->polling = true;
		schedule_delayed_work(&ts->poll_work,
				      msecs_to_jiffies(CHSC5432_POLL_INTERVAL_MS));
		dev_info(dev, "No IRQ configured, using polling mode (%dms)\n",
			 CHSC5432_POLL_INTERVAL_MS);
	}

	dev_info(dev, "CHSC5432 touchscreen probed (max %d points, %dx%d)\n",
		 CHSC5432_MAX_POINTS,
		 input_abs_get_max(input, ABS_MT_POSITION_X) + 1,
		 input_abs_get_max(input, ABS_MT_POSITION_Y) + 1);

	return 0;
}

static void chsc5432_remove(struct i2c_client *client)
{
	struct chsc5432_data *ts = i2c_get_clientdata(client);

	if (ts->polling) {
		ts->polling = false;
		cancel_delayed_work_sync(&ts->poll_work);
	}
}

static int chsc5432_suspend(struct device *dev)
{
	struct chsc5432_data *ts = i2c_get_clientdata(to_i2c_client(dev));

	mutex_lock(&ts->mutex);
	ts->suspended = true;
	mutex_unlock(&ts->mutex);

	if (ts->polling)
		cancel_delayed_work_sync(&ts->poll_work);

	return 0;
}

static int chsc5432_resume(struct device *dev)
{
	struct chsc5432_data *ts = i2c_get_clientdata(to_i2c_client(dev));

	chsc5432_reset(ts);

	mutex_lock(&ts->mutex);
	ts->suspended = false;
	mutex_unlock(&ts->mutex);

	if (ts->polling)
		schedule_delayed_work(&ts->poll_work,
				      msecs_to_jiffies(CHSC5432_POLL_INTERVAL_MS));

	return 0;
}

static DEFINE_SIMPLE_DEV_PM_OPS(chsc5432_pm_ops,
				chsc5432_suspend, chsc5432_resume);

static const struct of_device_id chsc5432_of_match[] = {
	{ .compatible = "chipsemi,chsc5432" },
	{ .compatible = "chsc,chsc5432" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, chsc5432_of_match);

static const struct i2c_device_id chsc5432_id[] = {
	{ "chsc5432", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, chsc5432_id);

static struct i2c_driver chsc5432_driver = {
	.driver = {
		.name		= CHSC5432_NAME,
		.of_match_table	= chsc5432_of_match,
		.pm		= pm_sleep_ptr(&chsc5432_pm_ops),
	},
	.probe		= chsc5432_probe,
	.remove		= chsc5432_remove,
	.id_table	= chsc5432_id,
};

module_i2c_driver(chsc5432_driver);

MODULE_AUTHOR("CHSC5432 Driver");
MODULE_DESCRIPTION("Chipsemi CHSC5432 Touchscreen Driver");
MODULE_LICENSE("GPL");
