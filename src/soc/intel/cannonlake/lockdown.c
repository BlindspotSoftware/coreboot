/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/msr.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <intelblocks/cfg.h>
#include <intelblocks/pmclib.h>
#include <intelpch/lockdown.h>
#include <soc/pm.h>

static void lock_debug_interface(void)
{
	msr_t msr = rdmsr(MSR_IA32_DEBUG_INTERFACE);

	if (msr.lo & MSR_IA32_DEBUG_INTERFACE_LOCK)
		return;

	if (CONFIG(INTEL_TXT))
		msr.lo &= ~MSR_IA32_DEBUG_INTERFACE_EN;

	msr.lo |= MSR_IA32_DEBUG_INTERFACE_LOCK;
	wrmsr(MSR_IA32_DEBUG_INTERFACE, msr);
}

static void pmc_lock_pmsync(void)
{
	uint8_t *pmcbase;
	uint32_t pmsyncreg;

	pmcbase = pmc_mmio_regs();

	pmsyncreg = read32(pmcbase + PMSYNC_TPR_CFG);
	pmsyncreg |= PCH2CPU_TPR_CFG_LOCK;
	write32(pmcbase + PMSYNC_TPR_CFG, pmsyncreg);
}

static void pmc_lock_abase(void)
{
	uint8_t *pmcbase;
	uint32_t reg32;

	pmcbase = pmc_mmio_regs();

	reg32 = read32(pmcbase + GEN_PMCON_B);
	reg32 |= (SLP_STR_POL_LOCK | ACPI_BASE_LOCK);
	write32(pmcbase + GEN_PMCON_B, reg32);
}

static void pmc_lock_smi(void)
{
	uint8_t *pmcbase;
	uint8_t reg8;

	pmcbase = pmc_mmio_regs();

	reg8 = read8(pmcbase + GEN_PMCON_B);
	reg8 |= SMI_LOCK;
	write8(pmcbase + GEN_PMCON_B, reg8);
}

static void pmc_lockdown_cfg(int chipset_lockdown)
{
	/* PMSYNC */
	pmc_lock_pmsync();
	/* Lock down ABASE and sleep stretching policy */
	pmc_lock_abase();
	/* Make sure payload/OS can't trigger global reset */
	pmc_global_reset_disable_and_lock();

	if (chipset_lockdown == CHIPSET_LOCKDOWN_COREBOOT)
		pmc_lock_smi();
}

void soc_lockdown_config(int chipset_lockdown)
{
	/* PMC lock down configuration */
	pmc_lockdown_cfg(chipset_lockdown);
	lock_debug_interface();
}
