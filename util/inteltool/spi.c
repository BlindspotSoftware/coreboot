
#include <stdio.h>
#include <stdlib.h>
#include "inteltool.h"

static const io_register_t ich6_bios_cntl_registers[] = {
	{ 0x0, 1, "BIOSWE - write enable" },
	{ 0x1, 1, "BLE - lock enable" },
	{ 0x2, 6, "reserved" },
};

static const io_register_t ich7_bios_cntl_registers[] = {
	{ 0x0, 1, "BIOSWE - write enable" },
	{ 0x1, 1, "BLE - lock enable" },
	{ 0x2, 2, "SPI Read configuration" },
	{ 0x4, 1, "TopSwapStatus" },
	{ 0x5, 3, "reserved" },
};

static const io_register_t pch_bios_cntl_registers[] = {
	{ 0x0, 1, "BIOSWE - write enable" },
	{ 0x1, 1, "BLE - lock enable" },
	{ 0x2, 2, "SPI Read configuration" },
	{ 0x4, 1, "TopSwapStatus" },
	{ 0x5, 1, "SMM BIOS Write Protect Disable" },
	{ 0x6, 2, "reserved" },
};

static const io_register_t adl_pch_bios_cntl_registers[] = {
	{ 0x0, 1, "WPD - Write Protect Disable" },
	{ 0x1, 1, "LE - Lock Enable" },
	{ 0x2, 1, "ESPI - eSPI Enable Pin Strap" },
	{ 0x3, 1, "Reserved" },
	{ 0x4, 1, "TS - Top Swap" },
	{ 0x5, 1, "EISS - Enable InSMM.STS" },
	{ 0x6, 1, "BBS - Boot BIOS Strap" },
	{ 0x7, 1, "BILD - BIOS Interface Lock-Down" },
	{ 0x8, 1, "BWPDS - BIOS Write Protect Disable Status" },
	{ 0x9, 1, "Reserved" },
	{ 0x10, 1, "BWRS - BIOS Write Status" },
	{ 0x11, 1, "BWRE - BIOS Write Reporting (Async-SMI)" },
	{ 0x12, 19, "Reserved" },
};

static const io_register_t elkhart_bios_cntl_registers[] = {
	{ 0x0, 1, "BIOSWE - write enable" },
	{ 0x1, 1, "BLE - lock enable" },
	{ 0x2, 1, "ESPI - eSPI Enable Pin Strap" },
	{ 0x3, 1, "Reserved" },
	{ 0x4, 1, "TS - TopSwapStatus" },
	{ 0x5, 1, "EISS - Enable InSMM.STS" },
	{ 0x6, 1, "BBS - Boot BIOS Strap" },
	{ 0x7, 1, "BILD - BIOS Interface Lock-Down" },
	{ 0x8, 1, "BWPDS - BIOS Write Protect Disable Status" },
	{ 0x9, 1, "Reserved" },
	{ 0xa, 1, "BWRS - BIOS Write Status" },
	{ 0xb, 1, "BWRE - BIOS Write Reporting (Async-SMI) Enable" },
};

#define ICH9_SPIBAR 0x3800
#define ICH78_SPIBAR 0x3020

static const io_register_t spi_bar_registers[] = {
	{ 0x00, 4, "BFPR - BIOS Flash primary region" },
	{ 0x04, 2, "HSFSTS - Hardware Sequencing Flash Status" },
	{ 0x06, 2, "HSFCTL - Hardware Sequencing Flash Control" },
	{ 0x08, 4, "FADDR - Flash Address" },
	{ 0x0c, 4, "Reserved" },
	{ 0x10, 4, "FDATA0" },
	/* 0x10 .. 0x4f are filled with data */
	{ 0x50, 4, "FRACC - Flash Region Access Permissions" },
	{ 0x54, 4, "Flash Region 0" },
	{ 0x58, 4, "Flash Region 1" },
	{ 0x5c, 4, "Flash Region 2" },
	{ 0x60, 4, "Flash Region 3" },
	{ 0x64, 4, "Flash Region 4" },
	{ 0x74, 4, "FPR0 Flash Protected Range 0" },
	{ 0x78, 4, "FPR0 Flash Protected Range 1" },
	{ 0x7c, 4, "FPR0 Flash Protected Range 2" },
	{ 0x80, 4, "FPR0 Flash Protected Range 3" },
	{ 0x84, 4, "FPR0 Flash Protected Range 4" },
	{ 0x90, 1, "SSFSTS - Software Sequencing Flash Status" },
	{ 0x91, 3, "SSFSTS - Software Sequencing Flash Status" },
	{ 0x94, 2, "PREOP - Prefix opcode Configuration" },
	{ 0x96, 2, "OPTYPE - Opcode Type Configuration" },
	{ 0x98, 8, "OPMENU - Opcode Menu Configuration" },
	{ 0xa0, 1, "BBAR - BIOS Base Address Configuration" },
	{ 0xb0, 4, "FDOC - Flash Descriptor Observability Control" },
	{ 0xb8, 4, "Reserved" },
	{ 0xc0, 4, "AFC - Additional Flash Control" },
	{ 0xc4, 4, "LVSCC - Host Lower Vendor Specific Component Capabilities" },
	{ 0xc8, 4, "UVSCC - Host Upper Vendor Specific Component Capabilities" },
	{ 0xd0, 4, "FPB - Flash Partition Boundary" },
};

static const io_register_t ich7_spi_bar_registers[] = {
	{ 0x00, 2, "SPIS - SPI Status" },
	{ 0x02, 2, "SPIC - SPI Control" },
	{ 0x04, 4, "SPIA - SPI Address" },
	/*
	 *0x08 .. 0x47 are filled with data
	 *0x48 .. 0x4f is not mentioned by datasheet
	 */
	{ 0x50, 4, "BBAR - BIOS Base Address Configuration" },
	{ 0x54, 2, "PREOP Prefix Opcode Configuration" },
	{ 0x56, 2, "OPTYPE Opcode Type Configuration" },
	{ 0x58, 8, "OPMENU Opcode Menu Configuration" },
	{ 0x60, 4, "PBR0 Protected BIOS Range 0" },
	{ 0x64, 4, "PBR1 Protected BIOS Range 1" },
	{ 0x68, 4, "PBR2 Protected BIOS Range 2" },
};

/*
 * Intel Atom x6000E Series, and Intel Pentium and Celeron N and J Series Processors for IoT Applications
 * February 2023,
 * Document number 636722
 */
static const io_register_t elkhart_spi_bar_registers[] = {
	{ 0x00, 4, "BFPR - BIOS Flash primary region" },
	{ 0x04, 2, "HSFSTS - Hardware Sequencing Flash Status" },
	{ 0x06, 2, "HSFCTL - Hardware Sequencing Flash Control" },
	{ 0x08, 4, "FADDR - Flash Address" },
	{ 0x0c, 4, "BIOS_DLOCK - Discrete Lock Bits" },
	{ 0x10, 4, "FDATA0" },
	/* 0x10 .. 0x4f are filled with data */
	{ 0x50, 4, "FRACC - Flash Region Access Permissions" },
	{ 0x54, 4, "Flash Region 0" },
	{ 0x58, 4, "Flash Region 1" },
	{ 0x5c, 4, "Flash Region 2" },
	{ 0x60, 4, "Flash Region 3" },
	{ 0x64, 4, "Flash Region 4" },
	{ 0x68, 4, "Flash Region 5" },
	{ 0x6c, 4, "Flash Region 6" },
	{ 0x70, 4, "Flash Region 7" },
	{ 0x74, 4, "Flash Region 8" },
	{ 0x78, 4, "Flash Region 9" },
	{ 0x7c, 4, "Flash Region 10" },
	{ 0x80, 4, "Flash Region 11" },
	{ 0x84, 4, "FPR0 - Flash Protected Range 0" },
	{ 0x88, 4, "FPR0 - Flash Protected Range 1" },
	{ 0x8c, 4, "FPR0 - Flash Protected Range 2" },
	{ 0x90, 4, "FPR0 - Flash Protected Range 3" },
	{ 0x94, 4, "FPR0 - Flash Protected Range 4" },
	{ 0x98, 4, "GPR0 - Global Protected Range 0" },
	{ 0xa0, 4, "SSFSTS - Software Sequencing Flash Status" },
	{ 0xa4, 2, "PREOP - Prefix opcode Configuration" },
	{ 0xa6, 2, "OPTYPE - Opcode Type Configuration" },
	{ 0xa8, 4, "OPMENU0 - Opcode Menu Configuration" },
	{ 0xac, 4, "OPMENU1 - Opcode Menu Configuration" },
	{ 0xb0, 4, "SFRACC - Secondary Flash Region Access Permissions" },
	{ 0xb4, 4, "FDOC - Flash Descriptor Observability Control" },
	{ 0xb8, 4, "FDOD - Flash Descriptor Observability Data" },
	{ 0xc0, 4, "AFC - Additional Flash Control" },
	{ 0xc4, 4, "SFDP0_VSCC0 - Vendor Specific Component Capabilities" },
	{ 0xc8, 4, "SFDP0_VSCC1 - Vendor Specific Component Capabilities" },
	{ 0xcc, 4, "PTINX - Parameter Table Index" },
	{ 0xd0, 4, "PTDATA - Parameter Table Data" },
	{ 0xd4, 4, "SBRS - SPI Bus Requester Status" },
	{ 0xe0, 4, "FREG12 - Flash Region" },
	{ 0xe4, 4, "FREG13 - Flash Region" },
	{ 0xe8, 4, "FREG14 - Flash Region" },
	{ 0xec, 4, "FREG15 - Flash Region" },
	{ 0x118, 4, "BM_WAP - BIOS Master Read Access Permissions (BIOS_BM_RAP)" },
	{ 0x11c, 4, "BM_WAP - BIOS Master Write Access Permissions (BIOS_BM_WAP)" },
	{ 0x184, 4, "CSXE_PR0 - CSXE Flash Protected Range" },
	{ 0x188, 4, "CSXE_PR1 - CSXE Flash Protected Range" },
	{ 0x18c, 4, "CSXE_PR2 - CSXE Flash Protected Range" },
	{ 0x190, 4, "CSXE_PR3 - CSXE Flash Protected Range" },
	{ 0x194, 4, "CSXE_PR4 - CSXE Flash Protected Range" },
	{ 0x198, 4, "CSXE_PR0 - CSXE Flash Protected Range" },
	{ 0x198, 4, "CSXE_WPR0 - Write Protected Range 0" },
};

static int print_bioscntl(struct pci_dev *sb)
{
	int i, size = 0;
	unsigned char bios_cntl = 0xff;
	const io_register_t *bios_cntl_register = NULL;

	printf("\n============= SPI / BIOS CNTL =============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH6:
		bios_cntl = pci_read_byte(sb, 0xdc);
		bios_cntl_register = ich6_bios_cntl_registers;
		size = ARRAY_SIZE(ich6_bios_cntl_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
	case PCI_DEVICE_ID_INTEL_ICH8:
	case PCI_DEVICE_ID_INTEL_ICH8M:
	case PCI_DEVICE_ID_INTEL_ICH8ME:
	case PCI_DEVICE_ID_INTEL_ICH9DH:
	case PCI_DEVICE_ID_INTEL_ICH9DO:
	case PCI_DEVICE_ID_INTEL_ICH9R:
	case PCI_DEVICE_ID_INTEL_ICH9:
	case PCI_DEVICE_ID_INTEL_ICH9M:
	case PCI_DEVICE_ID_INTEL_ICH9ME:
	case PCI_DEVICE_ID_INTEL_ICH10:
	case PCI_DEVICE_ID_INTEL_ICH10D:
	case PCI_DEVICE_ID_INTEL_ICH10DO:
	case PCI_DEVICE_ID_INTEL_ICH10R:
	case PCI_DEVICE_ID_INTEL_NM10:
		bios_cntl = pci_read_byte(sb, 0xdc);
		bios_cntl_register = ich7_bios_cntl_registers;
		size = ARRAY_SIZE(ich7_bios_cntl_registers);
		break;
	case PCI_DEVICE_ID_INTEL_3400:
	case PCI_DEVICE_ID_INTEL_3420:
	case PCI_DEVICE_ID_INTEL_3450:
	case PCI_DEVICE_ID_INTEL_3400_DESKTOP:
	case PCI_DEVICE_ID_INTEL_B55_A:
	case PCI_DEVICE_ID_INTEL_B55_B:
	case PCI_DEVICE_ID_INTEL_H55:
	case PCI_DEVICE_ID_INTEL_H57:
	case PCI_DEVICE_ID_INTEL_P55:
	case PCI_DEVICE_ID_INTEL_Q57:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF:
	case PCI_DEVICE_ID_INTEL_HM55:
	case PCI_DEVICE_ID_INTEL_HM57:
	case PCI_DEVICE_ID_INTEL_PM55:
	case PCI_DEVICE_ID_INTEL_QM57:
	case PCI_DEVICE_ID_INTEL_QS57:
	case PCI_DEVICE_ID_INTEL_Z68:
	case PCI_DEVICE_ID_INTEL_P67:
	case PCI_DEVICE_ID_INTEL_UM67:
	case PCI_DEVICE_ID_INTEL_HM65:
	case PCI_DEVICE_ID_INTEL_H67:
	case PCI_DEVICE_ID_INTEL_HM67:
	case PCI_DEVICE_ID_INTEL_Q65:
	case PCI_DEVICE_ID_INTEL_QS67:
	case PCI_DEVICE_ID_INTEL_Q67:
	case PCI_DEVICE_ID_INTEL_QM67:
	case PCI_DEVICE_ID_INTEL_B65:
	case PCI_DEVICE_ID_INTEL_C202:
	case PCI_DEVICE_ID_INTEL_C204:
	case PCI_DEVICE_ID_INTEL_C206:
	case PCI_DEVICE_ID_INTEL_H61:
	case PCI_DEVICE_ID_INTEL_Z77:
	case PCI_DEVICE_ID_INTEL_Z75:
	case PCI_DEVICE_ID_INTEL_Q77:
	case PCI_DEVICE_ID_INTEL_Q75:
	case PCI_DEVICE_ID_INTEL_B75:
	case PCI_DEVICE_ID_INTEL_H77:
	case PCI_DEVICE_ID_INTEL_C216:
	case PCI_DEVICE_ID_INTEL_QM77:
	case PCI_DEVICE_ID_INTEL_QS77:
	case PCI_DEVICE_ID_INTEL_HM77:
	case PCI_DEVICE_ID_INTEL_UM77:
	case PCI_DEVICE_ID_INTEL_HM76:
	case PCI_DEVICE_ID_INTEL_HM75:
	case PCI_DEVICE_ID_INTEL_HM70:
	case PCI_DEVICE_ID_INTEL_NM70:
	case PCI_DEVICE_ID_INTEL_C8_MOBILE:
	case PCI_DEVICE_ID_INTEL_C8_DESKTOP:
	case PCI_DEVICE_ID_INTEL_Z87:
	case PCI_DEVICE_ID_INTEL_Z85:
	case PCI_DEVICE_ID_INTEL_HM86:
	case PCI_DEVICE_ID_INTEL_H87:
	case PCI_DEVICE_ID_INTEL_HM87:
	case PCI_DEVICE_ID_INTEL_Q85:
	case PCI_DEVICE_ID_INTEL_Q87:
	case PCI_DEVICE_ID_INTEL_QM87:
	case PCI_DEVICE_ID_INTEL_B85:
	case PCI_DEVICE_ID_INTEL_C222:
	case PCI_DEVICE_ID_INTEL_C224:
	case PCI_DEVICE_ID_INTEL_C226:
	case PCI_DEVICE_ID_INTEL_H81:
	case PCI_DEVICE_ID_INTEL_C9_MOBILE:
	case PCI_DEVICE_ID_INTEL_C9_DESKTOP:
	case PCI_DEVICE_ID_INTEL_HM97:
	case PCI_DEVICE_ID_INTEL_Z97:
	case PCI_DEVICE_ID_INTEL_H97:
	case PCI_DEVICE_ID_INTEL_H110:
	case PCI_DEVICE_ID_INTEL_H170:
	case PCI_DEVICE_ID_INTEL_Z170:
	case PCI_DEVICE_ID_INTEL_Q170:
	case PCI_DEVICE_ID_INTEL_Q150:
	case PCI_DEVICE_ID_INTEL_B150:
	case PCI_DEVICE_ID_INTEL_C236:
	case PCI_DEVICE_ID_INTEL_C232:
	case PCI_DEVICE_ID_INTEL_QM170:
	case PCI_DEVICE_ID_INTEL_HM170:
	case PCI_DEVICE_ID_INTEL_CM236:
	case PCI_DEVICE_ID_INTEL_HM175:
	case PCI_DEVICE_ID_INTEL_QM175:
	case PCI_DEVICE_ID_INTEL_CM238:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
		bios_cntl = pci_read_byte(sb, 0xdc);
		bios_cntl_register = pch_bios_cntl_registers;
		size = ARRAY_SIZE(pch_bios_cntl_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ADL_N:
		bios_cntl = pci_read_byte(sb, 0xdc);
		bios_cntl_register = adl_pch_bios_cntl_registers;
		size = ARRAY_SIZE(adl_pch_bios_cntl_registers);
		break;
	case PCI_DEVICE_ID_INTEL_EHL:
		bios_cntl = pci_read_byte(sb, 0xdc);
		bios_cntl_register = elkhart_bios_cntl_registers;
		size = ARRAY_SIZE(elkhart_bios_cntl_registers);
		break;
	default:
		printf("Error: Dumping SPI on this southbridge is not (yet) supported.\n");
		return 1;
	}

	printf("BIOS_CNTL = 0x%04x (IO)\n\n", bios_cntl);

	if (bios_cntl_register) {
		for (i = 0; i < size; i++) {
			unsigned int val = bios_cntl >> bios_cntl_register[i].addr;
			val &= ((1 << bios_cntl_register[i].size) -1);
			printf("0x%04x = %s\n", val, bios_cntl_register[i].name);
		}
	}

	return 0;
}

static int print_spibar(struct pci_dev *sb, struct pci_access *pacc) {
	int i, size = 0, rcba_size = 0x4000;
	volatile uint8_t *rcba;
	uint32_t rcba_phys;
	const io_register_t *spi_register = NULL;
	uint32_t spibaroffset;
	struct pci_dev *spidev;

	printf("\n============= SPI Bar ==============\n\n");

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_ICH6:
		printf("This southbridge does not have a SPI controller.\n");
		return 1;
	case PCI_DEVICE_ID_INTEL_ICH7:
	case PCI_DEVICE_ID_INTEL_ICH7M:
	case PCI_DEVICE_ID_INTEL_ICH7DH:
	case PCI_DEVICE_ID_INTEL_ICH7MDH:
		spibaroffset = ICH78_SPIBAR;
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
		size = ARRAY_SIZE(ich7_spi_bar_registers);
		spi_register = ich7_spi_bar_registers;
		break;
	case PCI_DEVICE_ID_INTEL_ICH8:
		spibaroffset = ICH78_SPIBAR;
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
		size = ARRAY_SIZE(spi_bar_registers);
		spi_register = spi_bar_registers;
		break;
	case PCI_DEVICE_ID_INTEL_ICH8M:
	case PCI_DEVICE_ID_INTEL_ICH8ME:
	case PCI_DEVICE_ID_INTEL_ICH9DH:
	case PCI_DEVICE_ID_INTEL_ICH9DO:
	case PCI_DEVICE_ID_INTEL_ICH9R:
	case PCI_DEVICE_ID_INTEL_ICH9:
	case PCI_DEVICE_ID_INTEL_ICH9M:
	case PCI_DEVICE_ID_INTEL_ICH9ME:
	case PCI_DEVICE_ID_INTEL_ICH10:
	case PCI_DEVICE_ID_INTEL_ICH10D:
	case PCI_DEVICE_ID_INTEL_ICH10DO:
	case PCI_DEVICE_ID_INTEL_ICH10R:
	case PCI_DEVICE_ID_INTEL_NM10:
	case PCI_DEVICE_ID_INTEL_I63XX:
	case PCI_DEVICE_ID_INTEL_3400:
	case PCI_DEVICE_ID_INTEL_3420:
	case PCI_DEVICE_ID_INTEL_3450:
	case PCI_DEVICE_ID_INTEL_3400_DESKTOP:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE:
	case PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF:
	case PCI_DEVICE_ID_INTEL_B55_A:
	case PCI_DEVICE_ID_INTEL_B55_B:
	case PCI_DEVICE_ID_INTEL_H55:
	case PCI_DEVICE_ID_INTEL_H57:
	case PCI_DEVICE_ID_INTEL_HM55:
	case PCI_DEVICE_ID_INTEL_HM57:
	case PCI_DEVICE_ID_INTEL_P55:
	case PCI_DEVICE_ID_INTEL_PM55:
	case PCI_DEVICE_ID_INTEL_Q57:
	case PCI_DEVICE_ID_INTEL_QM57:
	case PCI_DEVICE_ID_INTEL_QS57:
	case PCI_DEVICE_ID_INTEL_Z68:
	case PCI_DEVICE_ID_INTEL_P67:
	case PCI_DEVICE_ID_INTEL_UM67:
	case PCI_DEVICE_ID_INTEL_HM65:
	case PCI_DEVICE_ID_INTEL_H67:
	case PCI_DEVICE_ID_INTEL_HM67:
	case PCI_DEVICE_ID_INTEL_Q65:
	case PCI_DEVICE_ID_INTEL_QS67:
	case PCI_DEVICE_ID_INTEL_Q67:
	case PCI_DEVICE_ID_INTEL_QM67:
	case PCI_DEVICE_ID_INTEL_B65:
	case PCI_DEVICE_ID_INTEL_C202:
	case PCI_DEVICE_ID_INTEL_C204:
	case PCI_DEVICE_ID_INTEL_C206:
	case PCI_DEVICE_ID_INTEL_H61:
	case PCI_DEVICE_ID_INTEL_Z77:
	case PCI_DEVICE_ID_INTEL_Z75:
	case PCI_DEVICE_ID_INTEL_Q77:
	case PCI_DEVICE_ID_INTEL_Q75:
	case PCI_DEVICE_ID_INTEL_B75:
	case PCI_DEVICE_ID_INTEL_H77:
	case PCI_DEVICE_ID_INTEL_C216:
	case PCI_DEVICE_ID_INTEL_QM77:
	case PCI_DEVICE_ID_INTEL_QS77:
	case PCI_DEVICE_ID_INTEL_HM77:
	case PCI_DEVICE_ID_INTEL_UM77:
	case PCI_DEVICE_ID_INTEL_HM76:
	case PCI_DEVICE_ID_INTEL_HM75:
	case PCI_DEVICE_ID_INTEL_HM70:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP:
	case PCI_DEVICE_ID_INTEL_C8_MOBILE:
	case PCI_DEVICE_ID_INTEL_C8_DESKTOP:
	case PCI_DEVICE_ID_INTEL_Z87:
	case PCI_DEVICE_ID_INTEL_Z85:
	case PCI_DEVICE_ID_INTEL_HM86:
	case PCI_DEVICE_ID_INTEL_H87:
	case PCI_DEVICE_ID_INTEL_HM87:
	case PCI_DEVICE_ID_INTEL_Q85:
	case PCI_DEVICE_ID_INTEL_Q87:
	case PCI_DEVICE_ID_INTEL_QM87:
	case PCI_DEVICE_ID_INTEL_B85:
	case PCI_DEVICE_ID_INTEL_C222:
	case PCI_DEVICE_ID_INTEL_C224:
	case PCI_DEVICE_ID_INTEL_C226:
	case PCI_DEVICE_ID_INTEL_H81:
	case PCI_DEVICE_ID_INTEL_C9_MOBILE:
	case PCI_DEVICE_ID_INTEL_C9_DESKTOP:
	case PCI_DEVICE_ID_INTEL_HM97:
	case PCI_DEVICE_ID_INTEL_Z97:
	case PCI_DEVICE_ID_INTEL_H97:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_PRE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_SKL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_BASE_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_PREM_KBL:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_BASE:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_U_IHDCP_PREM:
	case PCI_DEVICE_ID_INTEL_SUNRISEPOINT_LP_Y_IHDCP_PREM:
		spibaroffset = ICH9_SPIBAR;
		rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
		size = ARRAY_SIZE(spi_bar_registers);
		spi_register = spi_bar_registers;
		break;
	case PCI_DEVICE_ID_INTEL_EHL:
		/* the southbridge is the eSPI controller, we need to get the SPI flash controller */
		if (!(spidev = pci_get_dev(pacc, sb->domain, sb->bus, sb->dev, 5))) {
			perror("Error: no spi device 0:31.5\n");
			return 1;
		}

		rcba_phys = ((uint64_t)pci_read_long(spidev, 0x10) & 0xfffff000);
		rcba_size = 4096;
		if (!rcba_phys) {
			fprintf(stderr, "Error: no valid bar 0 of device 0:31.5 found %x %x\n", rcba_phys, rcba_size);
			return 1;
		}

		/* this is not rcba, but we keep it to use common code */
		spibaroffset = 0;
		spi_register = elkhart_spi_bar_registers;
		size = ARRAY_SIZE(elkhart_spi_bar_registers);
		break;
	case PCI_DEVICE_ID_INTEL_ICH:
	case PCI_DEVICE_ID_INTEL_ICH0:
	case PCI_DEVICE_ID_INTEL_ICH2:
	case PCI_DEVICE_ID_INTEL_ICH4:
	case PCI_DEVICE_ID_INTEL_ICH4M:
	case PCI_DEVICE_ID_INTEL_ICH5:
	case PCI_DEVICE_ID_INTEL_ADL_N:
		printf("This southbridge does not have RCBA.\n");
		return 1;
	default:
		printf("Error: Dumping RCBA on this southbridge is not (yet) supported.\n");
		return 1;
	}

	rcba = map_physical(rcba_phys, rcba_size);
	if (rcba == NULL) {
		perror("Error mapping RCBA");
		exit(1);
	}

	for (i = 0; i < size; i++) {
		switch(spi_register[i].size) {
			case 1:
				printf("0x%08x = %s\n", read8(rcba + spibaroffset + spi_register[i].addr), spi_register[i].name);
				break;
			case 2:
				printf("0x%08x = %s\n", read16(rcba + spibaroffset + spi_register[i].addr), spi_register[i].name);
				break;
			case 4:
				printf("0x%08x = %s\n", read32(rcba + spibaroffset + spi_register[i].addr), spi_register[i].name);
				break;
			case 8:
				printf("0x%08x%08x = %s\n",  read32(rcba + spibaroffset + spi_register[i].addr + 4),
					read32(rcba + spibaroffset + spi_register[i].addr), spi_register[i].name);
				break;
		}
	}

	unmap_physical((void *)rcba, rcba_size);
	return 0;
}

int print_spi(struct pci_dev *sb, struct pci_access *pacc) {
	return (print_bioscntl(sb) || print_spibar(sb, pacc));
}
