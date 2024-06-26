/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_rom.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <stdint.h>

u32 map_oprom_vendev(u32 vendev)
{
	if (vendev == PICASSO_VBIOS_VID_DID) {
		/* Check if the RV2 video bios needs to be used instead of the RV1/PCO one */
		if (soc_is_raven2()) {
			printk(BIOS_NOTICE, "Using RV2 VBIOS.\n");
			return RAVEN2_VBIOS_VID_DID;
		} else {
			printk(BIOS_NOTICE, "Using RV1/PCO VBIOS.\n");
		}
	} else if (vendev == RAVEN2_VBIOS_VID_DID) {
		printk(BIOS_NOTICE, "Using RV2 VBIOS.\n");
	}

	return vendev;
}
