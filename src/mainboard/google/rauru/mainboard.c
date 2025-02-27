/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <gpio.h>
#include <soc/bl31.h>
#include <soc/pcie.h>
#include <soc/usb.h>

#include "gpio.h"

static void power_on_fpmcu(void)
{
	/* Power on the fingerprint MCU */
	gpio_output(GPIO_EN_PWR_FP, 1);
	gpio_output(GPIO_FP_RST_1V8_S3_L, 1);
}

bool mainboard_needs_pcie_init(void)
{
	return true;
}

static void mainboard_init(struct device *dev)
{
	setup_usb_host();
	power_on_fpmcu();

	if (CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE))
		register_reset_to_bl31(GPIO_AP_EC_WARM_RST_REQ.id, true);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
