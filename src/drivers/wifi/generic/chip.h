/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _WIFI_GENERIC_H_
#define _WIFI_GENERIC_H_

#include <acpi/acpi_device.h>

/**
 * struct drivers_wifi_generic_config - Data structure to contain generic wifi config
 * @wake: Wake pin for ACPI _PRW
 */
struct drivers_wifi_generic_config {
	unsigned int wake;
	/* When set to true, this will add a _DSD which contains a single
	   property, `DmaProperty`, set to 1, under the ACPI Device. */
	bool add_acpi_dma_property;

	/*
	 * Applicable for Intel chipsets that use CNVi WiFi only. Set this to 1
	 * to enable CNVi DDR RFIM (radio frequency interference mitigation);
	 * SoC code propagates this value the applicable FSP UPD.
	 */
	bool enable_cnvi_ddr_rfim;

	/*
	 * Enable GPIO for CNVi that will be used for WiFi SW RF Kill (Low
	 * Power Mode).
	 */
	struct acpi_gpio cnvi_enable_gpio;

	/* Pointer to the Bluetooth companion device */
	DEVTREE_CONST struct device *bluetooth_companion;
};

#endif /* _GENERIC_WIFI_H_ */
