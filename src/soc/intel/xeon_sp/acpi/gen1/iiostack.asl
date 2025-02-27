/* SPDX-License-Identifier: GPL-2.0-only */

#define MAKE_IIO_DEV(id,rt,pxm)						\
	Device (PC##id)							\
	{								\
		Name (_HID, EisaId ("PNP0A08") /* PCI Express Bus */)	\
		Name (_CID, EisaId ("PNP0A03") /* PCI Bus */)		\
		Name (_UID, 0x##id)					\
		Method (_STA, 0, NotSerialized)				\
		{							\
			If (CondRefOf (_CRS))				\
			{						\
				Return (0xf)				\
			}						\
			Else						\
			{						\
				Return (0)				\
			}						\
		}							\
		Method (_PRT, 0, NotSerialized)				\
		{							\
			If (PICM)					\
			{						\
				Return (\_SB_.AR##rt)			\
			}						\
			Return (\_SB_.PR##rt)				\
		}							\
		Name (SUPP, 0x00)					\
		Name (CTRL, 0x00)					\
		Name (_PXM, pxm)  /* _PXM: Device Proximity */		\
		Method (_OSC, 4, NotSerialized)				\
		{							\
			CreateDWordField (Arg3, 0x00, CDW1)		\
			If ((Arg0 == ToUUID ("33db4d5b-1ff7-401c-9657-7441c03dd766") /* PCI Host Bridge Device */))  \
			{							\
				If (Arg2 < 0x03)				\
				{						\
					CDW1 |= 0x02 /* Unknown failure */	\
					Return (Arg3)				\
				}						\
				CreateDWordField (Arg3, 0x04, CDW2)		\
				CreateDWordField (Arg3, 0x08, CDW3)		\
				SUPP = CDW2					\
				CTRL = CDW3					\
				If ((SUPP & 0x16) != 0x16)		\
				{						\
					CTRL &= 0x1E				\
				}						\
				/* Never allow SHPC (no SHPC controller in system) */ \
				CTRL &= 0x1D					\
				/* Disable Native PCIe AER handling from OS */	\
				CTRL &= 0x17					\
				If ((Arg1 != 1)) /* unknown revision */	\
				{						\
					CDW1 |= 0x08				\
				}						\
				If ((CDW3 != CTRL)) /* capabilities bits were masked */ \
				{						\
					CDW1 |= 0x10				\
				}						\
				CDW3 = CTRL					\
				Return (Arg3)					\
			}							\
			Else							\
			{							\
				/* indicate unrecognized UUID */		\
				CDW1 |= 0x04					\
				DBG0 = 0xEE					\
				Return (Arg3)					\
			}							\
		}								\
	}

// Keep in sync with iio_domain_set_acpi_name()!
MAKE_IIO_DEV(00, 00, 0)
MAKE_IIO_DEV(01, 10, 0)
MAKE_IIO_DEV(02, 20, 0)
MAKE_IIO_DEV(03, 28, 0)

#if (CONFIG_MAX_SOCKET > 1)
MAKE_IIO_DEV(20, 40, 1)
MAKE_IIO_DEV(21, 50, 1)
MAKE_IIO_DEV(22, 60, 1)
MAKE_IIO_DEV(23, 68, 1)
#endif
