/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0892,	/* Codec Vendor / Device ID: Realtek */
	0x1849c892,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x1849c892),
	AZALIA_PIN_CFG(0, 0x11, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x12, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, 0x01011012),
	AZALIA_PIN_CFG(0, 0x16, 0x01016011),
	AZALIA_PIN_CFG(0, 0x17, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19840),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19850),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181304f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1c, 0x411111f0),
	AZALIA_PIN_CFG(0, 0x1d, 0x4005e601),
	AZALIA_PIN_CFG(0, 0x1e, 0x01452130),
	AZALIA_PIN_CFG(0, 0x1f, 0x411111f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
