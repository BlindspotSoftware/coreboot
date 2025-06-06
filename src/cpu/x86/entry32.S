/* SPDX-License-Identifier: GPL-2.0-or-later */

/* For starting coreboot in protected mode */

/*
 * This is the modern bootblock. It prepares the system for C environment runtime
 * setup. The actual setup is done by hardware-specific code.
 *
 * It provides a bootflow similar to other architectures, and thus is considered
 * to be the modern approach.
 *
 */

#include <cpu/x86/gdt.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>

.section .init, "ax", @progbits

	.code32
/*
 *	When we come here we are in protected mode.
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 */
	.align	4

.globl bootblock_protected_mode_entry
bootblock_protected_mode_entry:

	/* Save the BIST value */
	movl	%eax, %ebp

	post_code(POSTCODE_ENTER_PROTECTED_MODE)

	movw	$GDT_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	xor	%ax, %ax /* zero out the gs and fs segment index */
	movw	%ax, %fs
	movw	%ax, %gs /* Will be used for cpu_info */

	/* Restore the BIST value to %eax */
	movl	%ebp, %eax

#if CONFIG(BOOTBLOCK_DEBUG_SPINLOOP)

	/* Wait for a JTAG debugger to break in and set EBX non-zero */
	xor	%ebx, %ebx

debug_spinloop:
	cmp	$0, %ebx
	jz	debug_spinloop
#endif

	/* MMX registers required here */

	/* BIST result in eax */
	movd	%eax, %mm0

__timestamp:

	/* Get an early timestamp */
	rdtsc
	movd	%eax, %mm1
	movd	%edx, %mm2

#if CONFIG(SSE)
enable_sse:
	mov	%cr4, %eax
	or	$CR4_OSFXSR, %ax
	mov	%eax, %cr4
#endif /* CONFIG(SSE) */

	/* We're done. Now it's up to platform-specific code */
	jmp	bootblock_pre_c_entry
