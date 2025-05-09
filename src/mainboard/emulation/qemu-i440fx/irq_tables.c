/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/pirq_routing.h>
#include <device/pci_def.h>

static const struct irq_routing_table intel_irq_routing_table = {
	PIRQ_SIGNATURE,  /* u32 signature */
	PIRQ_VERSION,    /* u16 version   */
	32+16*CONFIG_IRQ_SLOT_COUNT,	 /* there can be total CONFIG_IRQ_SLOT_COUNT devices on the bus */
	0x00,		 /* Where the interrupt router lies (bus) */
	PCI_DEVFN(0x01, 0),   /* Where the interrupt router lies (dev) */
	0,		 /* IRQs devoted exclusively to PCI usage */
	0x8086,		 /* Vendor */
	0x7000,		 /* Device */
	0,		 /* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x7,         /*  u8 checksum , this hase to set to some value that would give 0 after the sum of all bytes for this structure (including checksum) */
	/* clang-format off */
	{
	       /* bus,PCI_DEVFN(dev, fn),  {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,PCI_DEVFN(0x01, 0), {{0x60, 0xdef8}, {0x61, 0xdef8}, {0x62, 0xdef8}, {0x63, 0x0def8}}, 0x0, 0x0},
		{0x00,PCI_DEVFN(0x02, 0), {{0x61, 0xdef8}, {0x62, 0xdef8}, {0x63, 0xdef8}, {0x60, 0x0def8}}, 0x1, 0x0},
		{0x00,PCI_DEVFN(0x03, 0), {{0x62, 0xdef8}, {0x63, 0xdef8}, {0x60, 0xdef8}, {0x61, 0x0def8}}, 0x2, 0x0},
		{0x00,PCI_DEVFN(0x04, 0), {{0x63, 0xdef8}, {0x60, 0xdef8}, {0x61, 0xdef8}, {0x62, 0x0def8}}, 0x3, 0x0},
		{0x00,PCI_DEVFN(0x05, 0), {{0x60, 0xdef8}, {0x61, 0xdef8}, {0x62, 0xdef8}, {0x63, 0x0def8}}, 0x4, 0x0},
		{0x00,PCI_DEVFN(0x06, 0), {{0x61, 0xdef8}, {0x62, 0xdef8}, {0x63, 0xdef8}, {0x60, 0x0def8}}, 0x5, 0x0},
	}
	/* clang-format on */
};
unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &intel_irq_routing_table);
}
