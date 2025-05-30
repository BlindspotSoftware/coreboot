/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	0x10ec0887,	/* Codec Vendor / Device ID: Realtek */
	0x10438445,	/* Subsystem ID */
	15,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(0, 0x10438445),
	AZALIA_PIN_CFG(0, 0x11, 0x40330000),
	AZALIA_PIN_CFG(0, 0x12, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x14, 0x01014010),
	AZALIA_PIN_CFG(0, 0x15, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x16, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x17, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x18, 0x01a19030),
	AZALIA_PIN_CFG(0, 0x19, 0x02a19040),
	AZALIA_PIN_CFG(0, 0x1a, 0x0181303f),
	AZALIA_PIN_CFG(0, 0x1b, 0x02214020),
	AZALIA_PIN_CFG(0, 0x1c, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1d, 0x4024c601),
	AZALIA_PIN_CFG(0, 0x1e, AZALIA_PIN_CFG_NC(0)),
	AZALIA_PIN_CFG(0, 0x1f, AZALIA_PIN_CFG_NC(0)),

	0x8086281c,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	10,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(2, 0x80860101),
	AZALIA_PIN_CFG(2, 0x04, 0x18560010),
	AZALIA_PIN_CFG(2, 0x06, 0x18560010),
	AZALIA_PIN_CFG(2, 0x08, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0a, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0b, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0c, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0d, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0e, 0x18560010),
	AZALIA_PIN_CFG(2, 0x0f, 0x18560010),

	0x80862805,	/* Codec Vendor / Device ID: Intel */
	0x80860101,	/* Subsystem ID */
	4,		/* Number of 4 dword sets */
	AZALIA_SUBVENDOR(3, 0x80860101),
	AZALIA_PIN_CFG(3, 0x05, 0x58560010),
	AZALIA_PIN_CFG(3, 0x06, 0x58560020),
	AZALIA_PIN_CFG(3, 0x07, 0x18560030),

};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
