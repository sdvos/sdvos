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
 * @file   board/ARDUINO_UNO/board.c
 * @author Ye Li (liye@sdvos.org)
 * @brief Arduino UNO board specific configuration
 */
#include <arch/avr5/types.h>

/** Constant that makes delay_loop delay 1 ms */
#define CONST_LOOPS 1000

/**
 * @def delay_loop
 * @brief Basic delay loop used in LoopDelay
 *
 * This loop should take 16 cycles in the body. Based on
 * which we can calculate the CONST_LOOPS macro and
 * provide a benchmark 1 ms loop.
 *
 * @param[in] n
 *   Loop count
 */
#define delay_loop(n)                               \
  __asm__ volatile ("1:\n\t"                        \
                    "nop\n\t"                       \
                    "nop\n\t"                       \
                    "nop\n\t"                       \
                    "nop\n\t"                       \
                    "nop\n\t"                       \
                    "nop\n\t"                       \
                    "subi %0, 0x1\n\t"              \
                    "sbc %1, r1\n\t"                \
                    "sbc %2, r1\n\t"                \
                    "sbc %3, r1\n\t"                \
                    "cp %0, r1\n\t"                 \
                    "cpc %1, r1\n\t"                \
                    "cpc %2, r1\n\t"                \
                    "cpc %3, r1\n\t"                \
                    "brne 1b\n\t"                   \
                    :                               \
                    : "r" ((n) & 0xFF),             \
                      "r" (((n) >> 8) & 0xFF),      \
                      "r" (((n) >> 16) & 0xFF),     \
                      "r" (((n) >> 24) & 0xFF))

void
LoopDelay (uint16_t ms)
{
  if (!ms) return;

  delay_loop ((uint32_t) ms * CONST_LOOPS);
}

void
BoardInit (void)
{
  /* Nothing to do for Arduino UNO */
}

/* vi: set et ai sw=2 sts=2: */
