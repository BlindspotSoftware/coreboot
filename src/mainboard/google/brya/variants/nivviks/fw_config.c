/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config lte_disable_pads_nivviks[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* D6  : WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config lte_disable_pads_nirwen[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* E13  : WWAN_EN */
	PAD_NC_LOCK(GPP_E13, NONE, LOCK_CONFIG),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config sd_disable_pads[] = {
	/* D8  : SD_CLKREQ_ODL */
	PAD_NC(GPP_D8, NONE),
	/* H12 : SD_PERST_L */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : EN_PP3300_SD_X */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
};

static const struct pad_config wfc_disable_pads[] = {
	/* D3  : WCAM_RST_L */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D15 : EN_PP2800_WCAM_X */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : EN_PP1800_PP1200_WCAM_X */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* H22 : WCAM_MCLK_R */
	PAD_NC(GPP_H22, NONE),
	/* R6 : DMIC_WCAM_CLK_R */
	PAD_NC(GPP_R6, NONE),
	/* R7 : DMIC_WCAM_DATA */
	PAD_NC(GPP_R7, NONE),
};

static const struct pad_config emmc_disable_pads[] = {
	/* I7  : EMMC_CMD */
	PAD_NC(GPP_I7, NONE),
	/* I8  : EMMC_D0 */
	PAD_NC(GPP_I8, NONE),
	/* I9  : EMMC_D1 */
	PAD_NC(GPP_I9, NONE),
	/* I10 : EMMC_D2 */
	PAD_NC(GPP_I10, NONE),
	/* I11 : EMMC_D3 */
	PAD_NC(GPP_I11, NONE),
	/* I12 : EMMC_D4 */
	PAD_NC(GPP_I12, NONE),
	/* I13 : EMMC_D5 */
	PAD_NC(GPP_I13, NONE),
	/* I14 : EMMC_D6 */
	PAD_NC(GPP_I14, NONE),
	/* I15 : EMMC_D7 */
	PAD_NC(GPP_I15, NONE),
	/* I16 : EMMC_RCLK */
	PAD_NC(GPP_I16, NONE),
	/* I17 : EMMC_CLK */
	PAD_NC(GPP_I17, NONE),
	/* I18 : EMMC_RST_L */
	PAD_NC(GPP_I18, NONE),
};

static const struct pad_config nvme_disable_pads[] = {
	/* B4  : SSD_PERST_L */
	PAD_NC_LOCK(GPP_B4, NONE, LOCK_CONFIG),
	/* D6  : SSD_CLKREQ_ODL */
	PAD_NC(GPP_D6, NONE),
	/* D11 : EN_PP3300_SSD */
	PAD_NC_LOCK(GPP_D11, NONE, LOCK_CONFIG),
	/* E17 : SSD_PLN_L */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
};

static const struct pad_config wifi_pcie_enable_pad[] = {
	/* H3 : SX_EXIT_HOLDOFF => WLAN_PCIE_WAKE_ODL */
	PAD_CFG_GPI_IRQ_WAKE(GPP_H3, NONE, PLTRST, LEVEL, INVERT),
	/* D7  : SRCCLKREQ2# ==> WLAN_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D7, NONE, DEEP, NF1),
	/* H20 : IMGCLKOUT1 ==> WLAN_PERST_L */
	PAD_CFG_GPO_LOCK(GPP_H20, 1, LOCK_CONFIG),
};

static const struct pad_config stylus_disable_pads[] = {
	/* F13 : SOC_PEN_DETECT_R_ODL */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : SOC_PEN_DETECT_ODL */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
};

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (!fw_config_probe(FW_CONFIG(WIFI_CATEGORY, WIFI_6))) {
		printk(BIOS_INFO, "CNVi bluetooth disabled by fw_config\n");
		config->cnvi_bt_core = false;
		printk(BIOS_INFO, "CNVi bluetooth audio offload disabled by fw_config\n");
		config->cnvi_bt_audio_offload = false;
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_probe(FW_CONFIG(DB_USB, DB_1C_LTE))) {
		if (board_id() >= 2) {
			printk(BIOS_INFO, "Disable LTE-related GPIO pins on nirwen.\n");
			gpio_padbased_override(padbased_table, lte_disable_pads_nirwen,
							ARRAY_SIZE(lte_disable_pads_nirwen)
			);
		} else {
			printk(BIOS_INFO, "Disable LTE-related GPIO pins on nivviks.\n");
			gpio_padbased_override(padbased_table, lte_disable_pads_nivviks,
							ARRAY_SIZE(lte_disable_pads_nivviks)
			);
		}
	}

	if (fw_config_probe(FW_CONFIG(WIFI_CATEGORY, WIFI_7))) {
		printk(BIOS_INFO, "Enable PCie based Wifi GPIO pins.\n");
		gpio_padbased_override(padbased_table, wifi_pcie_enable_pad,
						ARRAY_SIZE(wifi_pcie_enable_pad));
	}

	if (fw_config_probe(FW_CONFIG(SD_CARD, SD_ABSENT))) {
		printk(BIOS_INFO, "Disable SD card GPIO pins.\n");
		gpio_padbased_override(padbased_table, sd_disable_pads,
						ARRAY_SIZE(sd_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(WFC, WFC_ABSENT))) {
		printk(BIOS_INFO, "Disable MIPI WFC GPIO pins.\n");
		gpio_padbased_override(padbased_table, wfc_disable_pads,
						ARRAY_SIZE(wfc_disable_pads));
	}

	if (!fw_config_probe(FW_CONFIG(STORAGE, STORAGE_EMMC))) {
		printk(BIOS_INFO, "Disable eMMC SSD GPIO pins.\n");
		gpio_padbased_override(padbased_table, emmc_disable_pads,
						ARRAY_SIZE(emmc_disable_pads));
	}

	if (board_id() >= 2 && !fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME))) {
		printk(BIOS_INFO, "Disable NVMe SSD GPIO pins.\n");
		gpio_padbased_override(padbased_table, nvme_disable_pads,
						ARRAY_SIZE(nvme_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Disable Stylus GPIO pins.\n");
		gpio_padbased_override(padbased_table, stylus_disable_pads,
						ARRAY_SIZE(stylus_disable_pads));
	}
}
