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
 * @file   src/arch/avr5/utils.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Memory Utilities
 */

#include <arch/avr5/utils.h>

void
sdvos_memcpy (void * dst, void * src, udata_word_t count)
{
  __asm__ volatile ("tst %2\n\t"
                    "breq 2f\n\t"
                    "1:\n\t"
                    "ld __tmp_reg__, %a1+\n\t"
                    "st %a0+, __tmp_reg__\n\t"
                    "dec %2\n\t"
                    "brne 1b\n\t"
                    "2:\n\t"
                    ::"x" (dst), "y" (src), "a" (count));
}

void
sdvos_memset (void * dst, udata_word_t ch, udata_word_t count)
{
  __asm__ volatile ("tst %2\n\t"
                    "breq 2f\n\t"
                    "1:\n\t"
                    "st %a0+, %1\n\t"
                    "dec %2\n\t"
                    "brne 1b\n\t"
                    "2:\n\t"
                    ::"x" (dst), "a" (ch), "a" (count));
}

/* vi: set et ai sw=2 sts=2: */
