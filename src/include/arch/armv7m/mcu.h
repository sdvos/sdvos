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
 * @file   src/include/arch/armv7m/mcu.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M MCU Initialization
 */
#ifndef _ARMV7M_MCU_H_
#define _ARMV7M_MCU_H_

#include <config/config.h>

/**
 * @brief MCU specific initialization
 *
 * StartOS should only be called in privileged mode. We
 * just need to switch MSP back to the top of kernel
 * stack. This saves stack space because StartOS will
 * never return. In addition, we also set thread mode to
 * be unprivileged and switch to MSP if we are not using
 * it now.
 */
#define McuInit()  do {                      \
  /* Disable interrupts with PRIMASK */      \
  __asm__ volatile ("cpsid i\n\t"            \
                    /* Set up CONTROL */     \
                    /* Need to use MSP */    \
                    "mrs r0, control\n\t"    \
                    "ands r0, #1\n\t"        \
                    "msr control, r0\n\t"    \
                    "isb\n\t"                \
                    :                        \
                    :                        \
                    :"r0");                  \
  /* Early MCU initialization */             \
  SystemInit ();                             \
} while (0)

/**
 * @brief Startup function
 *
 * MCU specific startup function that initializes data and
 * bss sections before calling main function.
 */
void _start (void);

#ifdef __USE_FPU__
/**
 * @brief Initialize FPU
 *
 * MCU specific FPU initialization routine
 */
void FpuInit (void);
#endif

/**
 * @brief Early MCU initialization
 *
 * MCU specific early initialization routine. FPU, clock and
 * other board specific setting could be performed here.
 */
void SystemInit (void);

#endif

/* vi: set et ai sw=2 sts=2: */
