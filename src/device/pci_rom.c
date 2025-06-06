/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <bootmode.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/endian.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <stdio.h>
#include <string.h>
#include <timestamp.h>

/* Rmodules don't like weak symbols. */
u32 __weak map_oprom_vendev(u32 vendev) { return vendev; }

void vga_oprom_preload(void)
{
/* The CONFIG_VGA_BIOS_ID symbol is only defined when VGA_BIOS is selected */
#if CONFIG(VGA_BIOS)
	const char name[] = "pci" CONFIG_VGA_BIOS_ID ".rom";

	if (!CONFIG(CBFS_PRELOAD))
		return;

	printk(BIOS_DEBUG, "Preloading VGA ROM %s\n", name);

	cbfs_preload(name);
#endif
}

static void *cbfs_boot_map_optionrom(uint16_t vendor, uint16_t device)
{
	char name[17] = "pciXXXX,XXXX.rom";

	snprintf(name, sizeof(name), "pci%04hx,%04hx.rom", vendor, device);

	return cbfs_map(name, NULL);
}

struct rom_header *pci_rom_probe(const struct device *dev)
{
	struct rom_header *rom_header = NULL;
	struct pci_data *rom_data;
	u32 vendev = (dev->vendor << 16) | dev->device;
	u32 mapped_vendev = vendev;

	/* If the ROM is in flash, then don't check the PCI device for it. */
	mapped_vendev = map_oprom_vendev(vendev);
	rom_header = cbfs_boot_map_optionrom(mapped_vendev >> 16, mapped_vendev & 0xffff);

	/* Handle the case of VGA_BIOS_ID not being set to the remapped PCI ID. This is a
	   workaround that should be removed once the underlying issue is fixed. */
	if (!rom_header && vendev != mapped_vendev) {
		rom_header = cbfs_boot_map_optionrom(vendev >> 16, vendev & 0xffff);
		if (rom_header) {
			printk(BIOS_NOTICE, "VGA_BIOS_ID should be the remapped PCI ID "
					    "%04hx,%04hx in the VBIOS file\n",
			       mapped_vendev >> 16, mapped_vendev & 0xffff);
		}
	}

	if (rom_header) {
		printk(BIOS_DEBUG, "In CBFS, ROM address for %s = %p\n",
		       dev_path(dev), rom_header);
	} else if (CONFIG(ON_DEVICE_ROM_LOAD)) {
		uintptr_t rom_address;

		rom_address = pci_read_config32(dev, PCI_ROM_ADDRESS);

		if (rom_address == 0x00000000 || rom_address == 0xffffffff) {
			if (CONFIG(CPU_QEMU_X86) && (dev->class >> 8) == PCI_CLASS_DISPLAY_VGA)
				rom_address = 0xc0000;
			else
				return NULL;
		} else {
			/* Enable expansion ROM address decoding. */
			pci_write_config32(dev, PCI_ROM_ADDRESS,
					   rom_address|PCI_ROM_ADDRESS_ENABLE);
		}

		rom_address &= PCI_ROM_ADDRESS_MASK;

		printk(BIOS_DEBUG, "Option ROM address for %s = %lx\n",
		       dev_path(dev), (unsigned long)rom_address);
		rom_header = (struct rom_header *)rom_address;
	} else {
		printk(BIOS_DEBUG, "PCI Option ROM loading disabled for %s\n",
		       dev_path(dev));
		return NULL;
	}

	printk(BIOS_SPEW,
	       "PCI expansion ROM, signature 0x%04x, INIT size 0x%04x, data ptr 0x%04x\n",
	       le16_to_cpu(rom_header->signature),
	       rom_header->size * 512, le16_to_cpu(rom_header->data));

	if (le16_to_cpu(rom_header->signature) != PCI_ROM_HDR) {
		printk(BIOS_ERR, "Incorrect expansion ROM header signature %04x\n",
		       le16_to_cpu(rom_header->signature));
		return NULL;
	}

	rom_data = (((void *)rom_header) + le16_to_cpu(rom_header->data));

	printk(BIOS_SPEW, "PCI ROM image, vendor ID %04x, device ID %04x,\n",
	       rom_data->vendor, rom_data->device);
	/* If the device id is mapped, a mismatch is expected */
	if ((dev->vendor != rom_data->vendor
	    || dev->device != rom_data->device)
	    && (vendev == mapped_vendev)) {
		printk(BIOS_ERR, "ID mismatch: vendor ID %04x, device ID %04x\n",
		       dev->vendor, dev->device);
		return NULL;
	}

	printk(BIOS_SPEW, "PCI ROM image, Class Code %04x%02x, Code Type %02x\n",
	       rom_data->class_hi, rom_data->class_lo,
	       rom_data->type);

	if (dev->class != ((rom_data->class_hi << 8) | rom_data->class_lo)) {
		printk(BIOS_DEBUG, "Class Code mismatch ROM %08x, dev %08x\n",
		       (rom_data->class_hi << 8) | rom_data->class_lo,
		       dev->class);
		// return NULL;
	}

	return rom_header;
}

static void *pci_ram_image_start = (void *)PCI_RAM_IMAGE_START;

struct rom_header *pci_rom_load(struct device *dev,
				struct rom_header *rom_header)
{
	struct pci_data * rom_data;
	unsigned int rom_size;
	unsigned int image_size=0;

	do {
		/* Get next image. */
		rom_header = (struct rom_header *)((void *)rom_header
							    + image_size);

		rom_data = (struct pci_data *)((void *)rom_header
				+ le16_to_cpu(rom_header->data));

		image_size = le16_to_cpu(rom_data->ilen) * 512;
	} while ((rom_data->type != 0) && (rom_data->indicator != 0)); // make sure we got x86 version

	if (rom_data->type != 0)
		return NULL;

	rom_size = rom_header->size * 512;

	/*
	 * We check to see if the device thinks it is a VGA device not
	 * whether the ROM image is for a VGA device because some
	 * devices have a mismatch between the hardware and the ROM.
	 */
	if ((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA) {
		extern struct device *vga_pri; /* Primary VGA device (device.c). */
		if (dev != vga_pri) return NULL; /* Only one VGA supported. */
		if ((void *)PCI_VGA_RAM_IMAGE_START != rom_header) {
			printk(BIOS_DEBUG,
			       "Copying VGA ROM Image from %p to 0x%x, 0x%x bytes\n",
			       rom_header, PCI_VGA_RAM_IMAGE_START, rom_size);
			memcpy((void *)PCI_VGA_RAM_IMAGE_START, rom_header,
			       rom_size);
		}

		dev->pci_vga_option_rom = (struct rom_header *)(PCI_VGA_RAM_IMAGE_START);
		return (struct rom_header *)(PCI_VGA_RAM_IMAGE_START);
	}

	printk(BIOS_DEBUG, "Copying non-VGA ROM image from %p to %p, 0x%x bytes\n",
	       rom_header, pci_ram_image_start, rom_size);

	dev->pci_vga_option_rom = (struct rom_header *)pci_ram_image_start;
	memcpy(pci_ram_image_start, rom_header, rom_size);
	pci_ram_image_start += rom_size;
	return (struct rom_header *)(pci_ram_image_start-rom_size);
}

static int should_run_oprom(struct device *dev, struct rom_header *rom)
{
	static int should_run = -1;

	if (dev->upstream->segment_group) {
		printk(BIOS_ERR, "Only option ROMs of devices in first PCI segment group can "
				 "be run.\n");
		return 0;
	}

	if (CONFIG(VENDORCODE_ELTAN_VBOOT))
		if (rom != NULL)
			if (!verified_boot_should_run_oprom(rom))
				return 0;

	if (should_run >= 0)
		return should_run;

	if (CONFIG(ALWAYS_RUN_OPROM)) {
		should_run = 1;
		return should_run;
	}

	/* Don't run VGA option ROMs, unless we have to print
	 * something on the screen before the kernel is loaded.
	 */
	should_run = display_init_required();

	if (!should_run)
		printk(BIOS_DEBUG, "Not running VGA Option ROM\n");
	return should_run;
}

static int should_load_oprom(struct device *dev)
{
	/* If S3_VGA_ROM_RUN is disabled, skip running VGA option
	 * ROMs when coming out of an S3 resume.
	 */
	if (!CONFIG(S3_VGA_ROM_RUN) && acpi_is_wakeup_s3() &&
		((dev->class >> 8) == PCI_CLASS_DISPLAY_VGA))
		return 0;
	if (CONFIG(ALWAYS_LOAD_OPROM))
		return 1;
	if (should_run_oprom(dev, NULL))
		return 1;

	return 0;
}

static void oprom_pre_graphics_stall(void)
{
	if (CONFIG_PRE_GRAPHICS_DELAY_MS)
		mdelay(CONFIG_PRE_GRAPHICS_DELAY_MS);
}

/** Load an Option ROM into the C-segment and run it if necessary */
void pci_rom_run(struct device *dev)
{
	struct rom_header *rom, *ram;

	/* Only execute VGA ROMs. */
	if (((dev->class >> 8) != PCI_CLASS_DISPLAY_VGA))
		return;

	if (!should_load_oprom(dev))
		return;
	timestamp_add_now(TS_OPROM_INITIALIZE);

	rom = pci_rom_probe(dev);
	if (rom == NULL)
		return;

	ram = pci_rom_load(dev, rom);
	if (ram == NULL)
		return;
	timestamp_add_now(TS_OPROM_COPY_END);

	if (!should_run_oprom(dev, rom))
		return;

	/* Wait for any configured pre-graphics delay */
	oprom_pre_graphics_stall();

	run_bios(dev, (unsigned long)ram);

	gfx_set_init_done(1);
	printk(BIOS_DEBUG, "VGA Option ROM was run\n");
	timestamp_add_now(TS_OPROM_END);
}

/* ACPI */
#if CONFIG(HAVE_ACPI_TABLES)

static unsigned long
ati_rom_acpi_fill_vfct(const struct device *device, acpi_vfct_t *vfct_struct,
		       unsigned long current)
{
	acpi_vfct_image_hdr_t *header = &vfct_struct->image_hdr;
	struct rom_header *rom;

	/* Already loaded into DRAM? */
	rom = device->pci_vga_option_rom;
	if (!rom)
		rom = pci_rom_probe(device);
	if (!rom) {
		printk(BIOS_ERR, "%s failed\n", __func__);
		return current;
	}
	if (device->upstream->segment_group) {
		printk(BIOS_ERR, "VFCT only supports GPU in first PCI segment group.\n");
		return current;
	}

	printk(BIOS_DEBUG, "           Copying VBIOS image from %p\n", rom);

	header->DeviceID = device->device;
	header->VendorID = device->vendor;
	header->PCIBus = device->upstream->secondary;
	header->PCIFunction = PCI_FUNC(device->path.pci.devfn);
	header->PCIDevice = PCI_SLOT(device->path.pci.devfn);
	header->ImageLength = rom->size * 512;
	memcpy((void *)header->VbiosContent, rom, header->ImageLength);

	vfct_struct->VBIOSImageOffset = (size_t)header - (size_t)vfct_struct;

	current += header->ImageLength;
	return current;
}

unsigned long
pci_rom_write_acpi_tables(const struct device *device, unsigned long current,
			  struct acpi_rsdp *rsdp)
{
	/* Only handle display devices */
	if ((device->class >> 16) != PCI_BASE_CLASS_DISPLAY)
		return current;

	/* Only handle enabled devices */
	if (!device->enabled)
		return current;

	/* AMD/ATI uses VFCT */
	if (device->vendor == PCI_VID_ATI) {
		acpi_vfct_t *vfct;

		current = ALIGN_UP(current, 8);
		vfct = (acpi_vfct_t *)current;
		acpi_create_vfct(device, vfct, ati_rom_acpi_fill_vfct);
		if (vfct->header.length) {
			printk(BIOS_DEBUG, "ACPI:    * VFCT at %lx\n", current);
			current += vfct->header.length;
			acpi_add_table(rsdp, vfct);
		}
	}

	return current;
}

void pci_rom_ssdt(const struct device *device)
{
	const struct rom_header *rom;
	static size_t ngfx;

	/* Only handle display devices */
	if ((device->class >> 16) != PCI_BASE_CLASS_DISPLAY)
		return;

	/* Only handle enabled devices */
	if (!device->enabled)
		return;

	/* Already loaded into DRAM? */
	rom = device->pci_vga_option_rom;
	if (!rom) {
		/* Probe for option rom */
		rom = pci_rom_probe(device);
		if (!rom || !rom->size) {
			printk(BIOS_WARNING, "%s: Missing PCI Option ROM\n",
			dev_path(device));
			return;
		}

		/* Supports up to four devices. */
		if ((CBMEM_ID_ROM0 + ngfx) > CBMEM_ID_ROM3) {
			printk(BIOS_ERR, "%s: Out of CBMEM IDs.\n", dev_path(device));
			return;
		}

		/* Prepare memory */
		const size_t cbrom_length = rom->size * 512;
		if (!cbrom_length) {
			printk(BIOS_ERR, "%s: ROM has zero length!\n",
			dev_path(device));
			return;
		}

		void *cbrom = cbmem_add(CBMEM_ID_ROM0 + ngfx, cbrom_length);
		if (!cbrom) {
			printk(BIOS_ERR, "%s: Failed to allocate CBMEM.\n",
			dev_path(device));
			return;
		}
		/* Increment CBMEM id for next device */
		ngfx++;

		memcpy(cbrom, rom, cbrom_length);

		rom = cbrom;
	}

	const char *scope = acpi_device_path(device);
	if (!scope) {
		printk(BIOS_ERR, "%s: Missing ACPI scope\n", dev_path(device));
		return;
	}

	/* write _ROM method */
	acpigen_write_scope(scope);
	acpigen_write_rom((void *)rom, rom->size * 512);
	acpigen_pop_len(); /* pop scope */
}
#endif
