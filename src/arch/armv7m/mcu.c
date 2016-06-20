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
 * @file   src/arch/armv7m/mcu.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M MCU Initialization
 */
#include <arch/armv7m/scs.h>
#include <arch/armv7m/cache.h>

#ifdef __USE_FPU__
void
FpuInit ()
{
  /* Give CP10 and CP11 full access */
  SCB->CPACR |= ((3UL << (10 * 2)) | (3UL << (11 * 2)));
  /* Enable automatic and lazy context save */
  FPU->FPCCR |= (FPU_FPCCR_ASPEN | FPU_FPCCR_LSPEN);
  /* Clear lazy active bit */
  FPU->FPCCR &= (~FPU_FPCCR_LSPACT);
  /* Set FPDSCR (Default, AHP, DN, FZ, RMode all cleared) */
  FPU->FPDSCR = 0x0UL;

  /* Clear FPSCR (Not necessary, just to be clear) */
  __asm__ volatile ("movs r0, #0\n\t"
                    "vmsr fpscr, r0\n\t":::"r0");
}
#endif

void
SystemInit ()
{
#ifdef __USE_FPU__
  /* Initialize FPU */
  FpuInit ();
#endif
#ifdef __USE_CACHE__
  /* Enable Instruction Cache */
  SCB_EnableICache ();
  /* Enable Data Cache */
  SCB_EnableDCache ();
#endif
}

/* vi: set et ai sw=2 sts=2: */
