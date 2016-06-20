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
 * @file   src/arch/armv7m/utils.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Memory Utilities
 */
#include <arch/armv7m/types.h>

void
sdvos_memcpy (void * dst, void * src, udata_word_t count)
{
  __asm__ volatile ("cmp %2, #0\n\t"
                    "beq 2f\n\t"
                    "movs r3, #0\n\t"
                    "1:\n\t"
                    "ldrb r4, [%1, r3]\n\t"
                    "strb r4, [%0, r3]\n\t"
                    "adds r3, #1\n\t"
                    "subs %2, #1\n\t"
                    "bne 1b\n\t"
                    "2:\n\t"
                    :
                    :"l" (dst), "l" (src), "l" (count)
                    :"r4", "r3");
}

void
sdvos_memset (void * dst, udata_word_t ch, udata_word_t count)
{
  __asm__ volatile ("cmp %2, #0\n\t"
                    "beq 2f\n\t"
                    "movs r3, #0\n\t"
                    "1:\n\t"
                    "strb %1, [%0, r3]\n\t"
                    "adds r3, #1\n\t"
                    "subs %2, #1\n\t"
                    "bne 1b\n\t"
                    "2:\n\t"
                    :
                    :"l" (dst), "l" (ch), "l" (count)
                    :"r3");
}

/* vi: set et ai sw=2 sts=2: */
