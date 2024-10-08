/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>

void mainboard_config_rcba(void)
{
	RCBA16(D31IR) = DIR_ROUTE(PIRQA, PIRQD, PIRQC, PIRQA);
	RCBA16(D29IR) = DIR_ROUTE(PIRQF, PIRQD, PIRQA, PIRQC);
	RCBA16(D28IR) = DIR_ROUTE(PIRQA, PIRQA, PIRQA, PIRQA);
	RCBA16(D27IR) = DIR_ROUTE(PIRQG, PIRQB, PIRQC, PIRQD);
	RCBA16(D26IR) = DIR_ROUTE(PIRQA, PIRQF, PIRQC, PIRQD);
	RCBA16(D25IR) = DIR_ROUTE(PIRQE, PIRQF, PIRQG, PIRQH);
	RCBA16(D22IR) = DIR_ROUTE(PIRQA, PIRQD, PIRQC, PIRQB);
	RCBA16(D20IR) = DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD);
}

const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS] = {
	/* Length, Enable, OCn#, Location */
	{ 0x0040, 1, 0, USB_PORT_BACK_PANEL }, /* dock left */
	{ 0x0040, 1, 1, USB_PORT_BACK_PANEL }, /* right, EHCI debug */
	{ 0x0080, 1, USB_OC_PIN_SKIP, USB_PORT_DOCK }, /* WLAN + BT */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* webcam */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* USH / Smart Card */
	{ 0x0110, 1, 2, USB_PORT_BACK_PANEL }, /* back right, dock back */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_DOCK }, /* WWAN */
	{ 0x0040, 1, USB_OC_PIN_SKIP, USB_PORT_BACK_PANEL }, /* touchscreen */
};

const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS] = {
	{ 1, 1 }, /* right */
	{ 1, 0 }, /* back left */
	{ 1, 2 }, /* back right, dock back */
	{ 1, USB_OC_PIN_SKIP }
};
