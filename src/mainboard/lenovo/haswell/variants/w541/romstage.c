/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <device/pci_ops.h>

const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* Length, Enable, OCn#, Location */
	{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 0, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 1, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 2, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 3, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 3, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 4, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 4, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 5, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 6, USB_PORT_BACK_PANEL },
	{ 0x0040, 1, 6, USB_PORT_BACK_PANEL },
};

const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, 0 },
	{ 1, 0 },
	{ 1, USB_OC_PIN_SKIP },
	{ 1, USB_OC_PIN_SKIP },
	{ 1, 1 },
	{ 1, 1 },
};
