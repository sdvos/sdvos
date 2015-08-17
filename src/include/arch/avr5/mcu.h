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
 * @file   src/include/arch/avr5/mcu.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 MCU Initialization
 */
#include <config/config.h>

/**
 * @brief Test whether I bit is set in SREG
 *
 * @return
 *   The value of I bit in SREG. 1 means interrupt enabled.
 *   0 means interrupt disabled.
 */
static inline uint8_t
TstI ()
{
  uint8_t sreg = 0;
  __asm__ volatile ("in %0, 0x3F\n\t"
                    : "=r" (sreg):);
  return ((sreg >> 7) & 0x1);
}

/**
 * @def SwitchStack(stk)
 * @brief Switch current stack
 *
 * Macro to swith current stack.
 *
 * @param[in] stk
 *   Destination stack to switch to.
 */
#define SwitchStack(stk)                        \
  __asm__ volatile ("ldi r18, %0\n\t"           \
                    "out 0x3D, r18\n\t"         \
                    "ldi r18, %1\n\t"           \
                    "out 0x3E, r18\n\t"         \
                    :                           \
                    :"M" ((stk) & 0xFF),        \
                     "M" (((stk) >> 8) & 0xFF)  \
                    :"r18")
/**
 * @def McuInit()
 * @brief MCU specific initialization
 *
 * Initialization code for AVR5. In AVR5, we do not need
 * to do much initialization since the chip does not have
 * privilege level or memory protection aside from
 * bootloader area. Here, we simply switch the current
 * stack to KERN_STACK. KERN_STACK is not necessarily the
 * kernel stack tasks use. It might just be used during
 * StartOS or by idle task.
 */
#define McuInit()  do {                          \
  /* Disable all interrupts */                   \
  __asm__ volatile ("cli");                      \
  /* Switch to the kernel stack (temporary) */   \
  SwitchStack (KERN_STACK);                      \
} while (0)

/* vi: set et ai sw=2 sts=2: */
