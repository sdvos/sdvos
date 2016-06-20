/*
 *         Standard Dependable Vehicle Operating System
 *
 * Copyright (C) 2015 Ye Li (liye@sdvos.org)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file   src/arch/armv7m/panic.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7m panic
 */
#include <arch/armv7m/utils.h>
#include <arch/armv7m/types.h>
#include <arch/armv7m/interrupt.h>
#include <cc.h>
#include <debug.h>

void NAKED
panic ()
{
  __disable_irq ();

#ifdef DEBUG_SDVOS
  uint32_t lr = 0, pc = 0;


  __asm__ volatile ("mov %0, lr\n\t"
                    :"=r" (lr)
                    ::);
  __asm__ volatile ("mrs r0, psp\n\t"
                    "ldr r0, [r0, #24]\n\t"
                    "mov %0, r0\n\t"
                    :"=r" (pc)
                    :
                    :"r0");

  DEBUG_PRINTF ("Kernel Panic! LR=0x%X, PSP_PC=0x%X\n",
                lr, pc & (~0x1U));
#endif

  while (1);
}

/* vi: set et ai sw=2 sts=2: */
