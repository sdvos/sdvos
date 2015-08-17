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
 * @file   src/include/arch/avr5/utils.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Memory Utilities
 */
#ifndef _AVR5_UTILS_H_
#define _AVR5_UTILS_H_

#include <arch/avr5/types.h>

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

#ifdef WITH_FLASH_UTILITY
/**
 * @brief Load one byte from flash.
 *
 * Load one byte from flash at specified address.
 *
 * @param[in] addr
 *   Flash address.
 * @return
 *   Byte read.
 */
uint8_t sdvos_pgm_read (code_addr_t addr);
/**
 * @brief Write one page (PGSIZE) to flash.
 *
 * Write one page (PGSIZE)from SRAM to Flash. Page size
 * is architectural dependent. This function cannot be
 * called in application section in AVR5.
 *
 * @param[in] dst
 *   Destination address in Flash.
 * @param[in] src
 *   Source address in SRAM.
 * @return
 *   None
 */
void sdvos_pgm_write (code_addr_t dst, data_addr_t src);
#endif

#endif

/* vi: set et ai sw=2 sts=2: */
