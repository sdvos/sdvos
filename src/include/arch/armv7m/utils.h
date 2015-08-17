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
 * @file   src/include/arch/armv7m/utils.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Memory Utilities
 */
#ifndef _ARMV7M_UTILS_H_
#define _ARMV7M_UTILS_H_

#include <arch/armv7m/types.h>

/**
 * @brief Copy content in memory.
 *
 * Internal memory copy utility used by kernel. For
 * efficiency on multiple platforms, count is of unsigned
 * data word length.
 *
 * @param[in] dst
 *   Destination address in SRAM.
 * @param[in] src
 *   Source address in SRAM
 * @param[in] count
 *   Copy count in bytes.
 * @return
 *   None
 */
void sdvos_memcpy (void * dst, void * src, udata_word_t count);

/**
 * @brief Set memory to a specified value.
 *
 * Internal memory set utility used by kernel. For
 * efficiency on multiple platforms, count is of unsigned
 * data word length.
 *
 * @param[in] dst
 *   Destination address in SRAM.
 * @param[in] ch
 *   Value to be set.
 * @param[in] count
 *   Set count in bytes.
 * @return
 *   None
 */
void sdvos_memset (void * dst, udata_word_t ch,
                   udata_word_t count);

#endif

/* vi: set et ai sw=2 sts=2: */
