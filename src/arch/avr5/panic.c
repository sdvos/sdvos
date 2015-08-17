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
 * @file   src/arch/avr5/panic.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 panic
 */
#include <arch/avr5/utils.h>
#include <arch/avr5/types.h>
#include <debug.h>

/*
 * AVR5 panic will print the address on the stack. The
 * address is adjusted to point to the instruction right
 * before calling panic.
 *
 * If DEBUG_SDVOS is not enabled, it simply loops with
 * interrupt disabled.
 */
void
panic ()
{
#ifdef DEBUG_SDVOS
  uint16_t pc = 0;

  /*
   * For debug, we are trying to get the address of the
   * call to panic. This is done by stealing the return
   * address of panic, which points the the instruction
   * after panic call. For this inline assembly to work,
   * the compiler has to put it at the top of the function.
   * This might not be true for all the cases since gcc
   * will put the code pushing call preserved registers in
   * front of the inline assembly if necessary. However,
   * for the simple task performed by this function, this
   * is not really a big problem. Anyway, just remember
   * there is no guarantee the pc printed is correct.
   */
  __asm__ volatile ("pop r0\n\t"
                    "mov %B0, r0\n\t"
                    "pop r0\n\t"
                    "mov %A0, r0\n\t"
                    : "=z" (pc):: "r0");

  DEBUG_PRINTF ("SDVOS Panic: PC=0x%X\n", pc << 1);
#endif

  __asm__ volatile ("cli\n\t"
                    "1:\n\t"
                    "rjmp 1b\n\t"
                    ::);
}

/* vi: set et ai sw=2 sts=2: */
