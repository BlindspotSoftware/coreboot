/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_CHIP_H_
#define _SOC_CHIP_H_

#include <device/pci_ids.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <gpio.h>
#include <drivers/intel/gma/gma.h>
#include <intelblocks/cfg.h>
#include <intelblocks/gspi.h>
#include <intelblocks/pcie_rp.h>
#include <intelblocks/power_limit.h>
#include <intelblocks/xhci.h>
#include <soc/gpe.h>
#include <soc/pch.h>
#include <soc/pci_devs.h>
#include <soc/pcie_modphy.h>
#include <soc/pmc.h>
#include <soc/serialio.h>
#include <soc/usb.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_HD_AUDIO_DMIC_LINKS 2
#define MAX_HD_AUDIO_SNDW_LINKS 4
#define MAX_HD_AUDIO_SSP_LINKS  6

/* Types of different SKUs */
enum soc_intel_jasperlake_power_limits {
	JSL_N4500_6W_CORE,
	JSL_N6000_6W_CORE,
	JSL_N5100_6W_CORE,
	JSL_N4505_10W_CORE,
	JSL_N5105_10W_CORE,
	JSL_N6005_10W_CORE,
	JSL_POWER_LIMITS_COUNT
};

/* TDP values for different SKUs */
enum soc_intel_jasperlake_cpu_tdps {
	TDP_6W  = 6,
	TDP_10W = 10
};

/* Mapping of different SKUs based on CPU ID and TDP values */
static const struct {
	unsigned int pci_did;
	enum soc_intel_jasperlake_power_limits limits;
	enum soc_intel_jasperlake_cpu_tdps cpu_tdp;
} cpuid_to_jsl[] = {
	{ PCI_DID_INTEL_JSL_ID_1, JSL_N4500_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_JSL_ID_2, JSL_N6000_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_JSL_ID_3, JSL_N5100_6W_CORE, TDP_6W },
	{ PCI_DID_INTEL_JSL_ID_4, JSL_N4505_10W_CORE, TDP_10W },
	{ PCI_DID_INTEL_JSL_ID_5, JSL_N5105_10W_CORE, TDP_10W },
	{ PCI_DID_INTEL_JSL_ID_6, JSL_N6005_10W_CORE, TDP_10W },
};

/* Gfx related */
enum igd_dvmt50_pre_alloc {
	IGD_SM_0MB = 0x00,
	IGD_SM_32MB = 0x01,
	IGD_SM_64MB = 0x02,
	IGD_SM_96MB = 0x03,
	IGD_SM_128MB = 0x04,
	IGD_SM_160MB = 0x05,
	IGD_SM_4MB = 0xF0,
	IGD_SM_8MB = 0xF1,
	IGD_SM_12MB = 0xF2,
	IGD_SM_16MB = 0xF3,
	IGD_SM_20MB = 0xF4,
	IGD_SM_24MB = 0xF5,
	IGD_SM_28MB = 0xF6,
	IGD_SM_36MB = 0xF8,
	IGD_SM_40MB = 0xF9,
	IGD_SM_44MB = 0xFA,
	IGD_SM_48MB = 0xFB,
	IGD_SM_52MB = 0xFC,
	IGD_SM_56MB = 0xFD,
	IGD_SM_60MB = 0xFE,
};

enum igd_aperture_size {
	IGD_AP_SZ_128MB = 0x00,
	IGD_AP_SZ_256MB = 0x01,
	IGD_AP_SZ_512MB = 0x02,
};

struct soc_intel_jasperlake_config {
	/* Common struct containing soc config data required by common code */
	struct soc_intel_common_config common_soc_config;

	/* Common struct containing power limits configuration information */
	struct soc_power_limits_config power_limits_config[JSL_POWER_LIMITS_COUNT];

	/* Gpio group routed to each dword of the GPE0 block. Values are
	 * of the form PMC_GPP_[A:U] or GPD. */
	uint8_t pmc_gpe0_dw0; /* GPE0_31_0 STS/EN */
	uint8_t pmc_gpe0_dw1; /* GPE0_63_32 STS/EN */
	uint8_t pmc_gpe0_dw2; /* GPE0_95_64 STS/EN */

	/* Generic IO decode ranges */
	uint32_t gen1_dec;
	uint32_t gen2_dec;
	uint32_t gen3_dec;
	uint32_t gen4_dec;

	/* Enable S0iX support */
	bool s0ix_enable;
	/* Enable DPTF support */
	bool dptf_enable;

	/* Deep SX enable for both AC and DC */
	bool deep_s3_enable_ac;
	bool deep_s3_enable_dc;
	bool deep_s5_enable_ac;
	bool deep_s5_enable_dc;

	/* Deep Sx Configuration
	 *  DSX_EN_WAKE_PIN       - Enable WAKE# pin
	 *  DSX_EN_LAN_WAKE_PIN   - Enable LAN_WAKE# pin
	 *  DSX_DIS_AC_PRESENT_PD - Disable pull-down on AC_PRESENT pin */
	uint32_t deep_sx_config;

	/* TCC activation offset */
	uint32_t tcc_offset;

	/* System Agent dynamic frequency support.
	 * When enabled memory will be training at different frequencies.
	 * 0:Disabled, 1:FixedPoint0(low), 2:FixedPoint1(mid), 3:FixedPoint2
	 * (high), 4:Enabled */
	enum {
		SaGv_Disabled,
		SaGv_FixedPoint0,
		SaGv_FixedPoint1,
		SaGv_FixedPoint2,
		SaGv_Enabled,
	} SaGv;

	/* Rank Margin Tool */
	bool RMT;

	/* USB related */
	struct usb2_port_config usb2_ports[16];
	struct usb3_port_config usb3_ports[10];
	/* Wake Enable Bitmap for USB2 ports */
	uint16_t usb2_wake_enable_bitmap;
	/* Wake Enable Bitmap for USB3 ports */
	uint16_t usb3_wake_enable_bitmap;

	/* Set the LFPS periodic sampling off time for USB3 Ports.
	   Default value of PMCTRL_REG bits[7:4] is 9 which means periodic
	   sampling off interval is 9ms, the range is from 0 to 15. */
	uint8_t xhci_lfps_sampling_offtime_ms;

	/* SATA related */
	uint8_t SataMode;
	bool SataSalpSupport;
	bool SataPortsEnable[8];
	bool SataPortsDevSlp[8];

	/* Audio related */
	bool PchHdaDspEnable;
	bool PchHdaAudioLinkHdaEnable;
	bool PchHdaAudioLinkDmicEnable[MAX_HD_AUDIO_DMIC_LINKS];
	bool PchHdaAudioLinkSspEnable[MAX_HD_AUDIO_SSP_LINKS];
	bool PchHdaAudioLinkSndwEnable[MAX_HD_AUDIO_SNDW_LINKS];
	uint8_t PchHdaIDispLinkTmode;
	uint8_t PchHdaIDispLinkFrequency;
	bool PchHdaIDispCodecDisconnect;

	/* PCIe Root Ports */
	/* PCIe output clocks type to PCIe devices.
	 * 0-23: PCH rootport, 0x70: LAN, 0x80: unspecified but in use,
	 * 0xFF: not used */
	uint8_t PcieClkSrcUsage[CONFIG_MAX_PCIE_CLOCK_SRC];
	/* PCIe ClkReq-to-ClkSrc mapping, number of clkreq signal assigned to
	 * clksrc. */
	uint8_t PcieClkSrcClkReq[CONFIG_MAX_PCIE_CLOCK_SRC];

	/* Probe CLKREQ# signal before enabling CLKREQ# based power management.*/
	bool PcieRpClkReqDetect[CONFIG_MAX_ROOT_PORTS];

	/* PCIe LTR */
	bool PcieRpLtrEnable[CONFIG_MAX_ROOT_PORTS];

	/* PCIe RP L1 substate */
	enum L1_substates_control PcieRpL1Substates[CONFIG_MAX_ROOT_PORTS];

	/* PCIe ModPhy related */
	struct pcie_modphy_config pcie_mp_cfg[CONFIG_MAX_ROOT_PORTS];

	/* SMBus */
	bool SmbusEnable;

	/* eMMC and SD */
	bool ScsEmmcHs400Enabled;

	/* Enable if SD Card Power Enable Signal is Active High */
	bool SdCardPowerEnableActiveHigh;

	/* VR Config Settings for IA Core */
	uint16_t ImonSlope;
	uint16_t ImonOffset;

	/* Gfx related */
	bool SkipExtGfxScan;

	bool eist_enable;

	/* Enable C6 DRAM */
	bool enable_c6dram;

	/*
	 * SerialIO device mode selection:
	 * PchSerialIoDisabled,
	 * PchSerialIoPci,
	 * PchSerialIoHidden,
	 * PchSerialIoLegacyUart,
	 * PchSerialIoSkipInit
	 */
	uint8_t SerialIoI2cMode[CONFIG_SOC_INTEL_I2C_DEV_MAX];
	uint8_t SerialIoGSpiMode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	uint8_t SerialIoUartMode[CONFIG_SOC_INTEL_UART_DEV_MAX];
	/*
	 * GSPIn Default Chip Select Mode:
	 * 0:Hardware Mode,
	 * 1:Software Mode
	 */
	uint8_t SerialIoGSpiCsMode[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];
	/*
	 * GSPIn Default Chip Select State:
	 * 0: Low,
	 * 1: High
	 */
	uint8_t SerialIoGSpiCsState[CONFIG_SOC_INTEL_COMMON_BLOCK_GSPI_MAX];

	/*
	 * TraceHubMode config
	 * 0: Disable, 1: Target Debugger Mode, 2: Host Debugger Mode
	 */
	uint8_t TraceHubMode;

	/* Debug interface selection */
	enum {
		DEBUG_INTERFACE_RAM = (1 << 0),
		DEBUG_INTERFACE_UART_8250IO = (1 << 1),
		DEBUG_INTERFACE_USB3 = (1 << 3),
		DEBUG_INTERFACE_LPSS_SERIAL_IO = (1 << 4),
		DEBUG_INTERFACE_TRACEHUB = (1 << 5),
	} debug_interface_flag;

	/* GPIO SD card detect pin */
	unsigned int sdcard_cd_gpio;

	/* Enable Pch iSCLK */
	bool pch_isclk;

	/* Tcss */
	bool TcssXhciEn;
	bool TcssXdciEn;

	/*
	 * Override GPIO PM configuration:
	 * false: Use FSP default GPIO PM program,
	 * true: coreboot to override GPIO PM program
	 */
	bool gpio_override_pm;

	/*
	 * GPIO PM configuration: 0 to disable, 1 to enable power gating
	 * Bit 6-7: Reserved
	 * Bit 5: MISCCFG_GPSIDEDPCGEN
	 * Bit 4: MISCCFG_GPRCOMPCDLCGEN
	 * Bit 3: MISCCFG_GPRTCDLCGEN
	 * Bit 2: MISCCFG_GSXLCGEN
	 * Bit 1: MISCCFG_GPDPCGEN
	 * Bit 0: MISCCFG_GPDLCGEN
	 */
	uint8_t gpio_pm[TOTAL_GPIO_COMM];

	/* DP config */
	/*
	 * Port config
	 * 0:Disabled, 1:eDP, 2:MIPI DSI
	 */
	uint8_t DdiPortAConfig;
	uint8_t DdiPortBConfig;

	/*
	 * HDP config
	 */
	bool DdiPortAHpd;
	bool DdiPortBHpd;
	bool DdiPortCHpd;
	bool DdiPort1Hpd;
	bool DdiPort2Hpd;
	bool DdiPort3Hpd;
	bool DdiPort4Hpd;

	/*
	 * DDC config
	 */
	bool DdiPortADdc;
	bool DdiPortBDdc;
	bool DdiPortCDdc;
	bool DdiPort1Ddc;
	bool DdiPort2Ddc;
	bool DdiPort3Ddc;
	bool DdiPort4Ddc;

	/* Hybrid storage mode
	 * This mode makes FSP detect Optane and NVME and set PCIe lane mode
	 * accordingly
	 *
	 * true: Enable Hybrid storage mode
	 * false Dsiable Hybrid storage mode
	 */
	bool HybridStorageMode;

	/*
	 * Override CPU flex ratio value:
	 * CPU ratio value controls the maximum processor non-turbo ratio.
	 * Valid Range 0 to 63.
	 * In general descriptor provides option to set default cpu flex ratio.
	 * Default cpu flex ratio 0 ensures booting with non-turbo max frequency.
	 * That's the reason FSP skips cpu_ratio override if cpu_ratio is 0.
	 * Only override CPU flex ratio to not boot with non-turbo max.
	 */
	uint8_t cpu_ratio_override;

	/* Skip CPU replacement check
	 *
	 * Setting this option to skip CPU replacement check to avoid the forced MRC training
	 * for the platforms with soldered down SOC.
	 *
	 * false: disable
	 * true: enable
	 */
	bool SkipCpuReplacementCheck;

	/*
	 * SLP_S3 Minimum Assertion Width Policy
	 *  1 = 60us
	 *  2 = 1ms
	 *  3 = 50ms (default)
	 *  4 = 2s
	 */
	uint8_t PchPmSlpS3MinAssert;

	/*
	 * SLP_S4 Minimum Assertion Width Policy
	 *  1 = 1s (default)
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s
	 */
	uint8_t PchPmSlpS4MinAssert;

	/*
	 * SLP_SUS Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 500ms
	 *  3 = 1s
	 *  4 = 4s (default)
	 */
	uint8_t PchPmSlpSusMinAssert;

	/*
	 * SLP_A Minimum Assertion Width Policy
	 *  1 = 0ms
	 *  2 = 4s
	 *  3 = 98ms
	 *  4 = 2s (default)
	 */
	uint8_t PchPmSlpAMinAssert;

	/*
	 * PCH PM Reset Power Cycle Duration
	 *  0 = 4s (default)
	 *  1 = 1s
	 *  2 = 2s
	 *  3 = 3s
	 *  4 = 4s
	 *
	 * NOTE: Duration programmed in the PchPmPwrCycDur should never be smaller than the
	 * stretch duration programmed in the following registers:
	 *  - GEN_PMCON_A.SLP_S3_MIN_ASST_WDTH (PchPmSlpS3MinAssert)
	 *  - GEN_PMCON_A.S4MAW (PchPmSlpS4MinAssert)
	 *  - PM_CFG.SLP_A_MIN_ASST_WDTH (PchPmSlpAMinAssert)
	 *  - PM_CFG.SLP_LAN_MIN_ASST_WDTH
	 */
	uint8_t PchPmPwrCycDur;

	/*
	 * FIVR RFI Frequency
	 * PCODE MMIO Mailbox: Set the desired RFI frequency, in increments of 100KHz.
	 * 0: Auto.
	 * Range varies based on XTAL clock:
	 *    0-1918 (Up to 191.8HMz) for 24MHz clock;
	 *    0-1535 (Up to 153.5MHz) for 19MHz clock.
	 */
	uint16_t FivrRfiFrequency;

	/*
	 * FIVR RFI Spread Spectrum
	 * Set the Spread Spectrum Range. <b>0: 0%</b>;
	 * FIVR RFI Spread Spectrum, in 0.1% increments.
	 * Range: 0.0% to 10.0% (0-100)
	 */
	uint8_t FivrSpreadSpectrum;

	/*
	 * Disable Fast Slew Rate for Deep Package C States for VCCIN VR domain
	 * Disable Fast Slew Rate for Deep Package C States based on
	 * Acoustic Noise Mitigation feature enabled.
	 */
	bool FastPkgCRampDisable;

	/*
	 * Slew Rate configuration for Deep Package C States for VCCIN VR domain
	 * based on Acoustic Noise Mitigation feature enabled.
	 * 0: Fast/2 ; 1: Fast/4; 2: Fast/8; 3: Fast/16
	 */
	enum {
		SlewRateFastBy2 = 0,
		SlewRateFastBy4,
		SlewRateFastBy8,
		SlewRateFastBy16
	} SlowSlewRate;

	/*
	 * Enable or Disable Acoustic Noise Mitigation feature.
	 */
	bool AcousticNoiseMitigation;

	/*
	 * Acoustic Noise Mitigation Range.Defines the maximum Pre-Wake
	 * randomization time in micro ticks.This can be programmed only
	 * if AcousticNoiseMitigation is enabled.
	 * Range 0-255
	 */
	uint8_t PreWake;

	/*
	 * Acoustic Noise Mitigation Range.Defines the maximum Ramp Up
	 * randomization time in micro ticks.This can be programmed only
	 * if AcousticNoiseMitigation is enabled.
	 * Range 0-255
	 */
	uint8_t RampUp;

	/*
	 * Acoustic Noise Mitigation Range.Defines the maximum Ramp Down
	 * randomization time in micro ticks.This can be programmed only
	 * if AcousticNoiseMitigation is enabled.
	 * Range 0-255
	 */
	uint8_t RampDown;

	/*
	 * It controls below soc variables
	 *
	 *   PchFivrExtV1p05RailEnabledStates
	 *   PchFivrExtVnnRailSxEnabledStates
	 *   PchFivrExtVnnRailEnabledStates
	 *
	 * If your platform does not support external vnn power rail please set to 1
	 * 1: Disabled ; 0: Enabled
	 */
	bool disable_external_bypass_vr;

	/*
	 * Core Display Clock Frequency selection, FSP UPD CdClock values + 1
	 *
	 * FSP will use the value to program clock frequency for core display if GOP
	 * is not run. Ex: the Chromebook normal mode.
	 * For the cases GOP is run, GOP will be in charge of the related register
	 * settings.
	 */
	enum {
		CD_CLOCK_172_8_MHZ = 1,
		CD_CLOCK_180_MHZ   = 2,
		CD_CLOCK_192_MHZ   = 3,
		CD_CLOCK_307_MHZ   = 4,
		CD_CLOCK_312_MHZ   = 5,
		CD_CLOCK_552_MHZ   = 6,
		CD_CLOCK_556_8_MHZ = 7,
	} cd_clock;

	/* Platform Power Pmax */
	uint16_t PsysPmax;

	/*
	 * This is a workaround to mitigate higher SoC power consumption in S0ix
	 * when the CNVI has background activity.
	 *
	 * Setting this on a system that supports S0i3 (set xtalsdqdis [Bit 22] in
	 * cppmvric1 register to 0) will break CNVI timing.
	 * Affected Intel wireless chipsets: AC9560 (JfP2), AC9461/AC9462 (JfP1) and
	 * AX201 (HrP2)
	 *
	 * true: Enabled (fewer wakes, lower power)
	 * false: Disabled (more wakes, higher power)
	 */
	bool cnvi_reduce_s0ix_pwr_usage;

	/* i915 struct for GMA backlight control */
	struct i915_gpu_controller_info gfx;
};

typedef struct soc_intel_jasperlake_config config_t;

#endif
