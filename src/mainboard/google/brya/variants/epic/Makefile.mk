# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += gpio.c

romstage-y += gpio.c

ramstage-$(CONFIG_FW_CONFIG) += fw_config.c

ramstage-y += variant.c

ramstage-y += gpio.c
