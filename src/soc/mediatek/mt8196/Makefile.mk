## SPDX-License-Identifier: GPL-2.0-only

ifeq ($(CONFIG_SOC_MEDIATEK_MT8196),y)

all-y += ../common/flash_controller.c
all-y += ../common/gpio.c ../common/gpio_op.c gpio.c gpio_eint.c
all-y += ../common/i2c.c i2c.c
all-y += ../common/pll.c pll.c
all-$(CONFIG_SPI_FLASH) += ../common/spi.c spi.c
all-y += timer.c timer_prepare.c
all-y += ../common/uart.c

bootblock-y += bootblock.c
bootblock-y += ../common/bootblock.c bootblock.c
bootblock-y += ../common/early_init.c
bootblock-y += ../common/lastbus_v2.c lastbus.c
bootblock-y += ../common/mmu_operations.c
bootblock-$(CONFIG_PCI) += ../common/pcie.c pcie.c
bootblock-y += ../common/wdt.c ../common/wdt_req.c wdt.c

romstage-y += ../common/cbmem.c
romstage-y += ../common/dram_init.c
romstage-y += ../common/dramc_param.c
romstage-$(CONFIG_PCI) += ../common/early_init.c ../common/pcie.c
romstage-y += ../common/emi.c
romstage-y += irq2axi.c
romstage-y += l2c_ops.c
romstage-y += ../common/memory.c memory.c
romstage-y += ../common/memory_test.c
romstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c

ramstage-y += ../common/early_init.c
ramstage-y += ../common/emi.c
ramstage-y += l2c_ops.c
ramstage-y += ../common/mmu_operations.c ../common/mmu_cmops.c
ramstage-$(CONFIG_PCI) += ../common/pcie.c pcie.c
ramstage-y += soc.c
ramstage-y += ../common/usb.c usb.c

CPPFLAGS_common += -Isrc/soc/mediatek/mt8196/include
CPPFLAGS_common += -Isrc/soc/mediatek/common/include

MT8196_BLOB_DIR := 3rdparty/blobs/soc/mediatek/mt8196

DRAM_CBFS := $(CONFIG_CBFS_PREFIX)/dram
$(DRAM_CBFS)-file := $(MT8196_BLOB_DIR)/dram.elf
$(DRAM_CBFS)-type := stage
$(DRAM_CBFS)-compression := $(CBFS_PRERAM_COMPRESS_FLAG)
ifneq ($(wildcard $($(DRAM_CBFS)-file)),)
	cbfs-files-y += $(DRAM_CBFS)
endif
$(objcbfs)/bootblock.bin: $(objcbfs)/bootblock.raw.bin
	./util/mtkheader/gen-bl-img.py mt8196 sf $< $@

endif
