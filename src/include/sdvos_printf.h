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
 * @file   src/include/sdvos_printf.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Simplified version of printf and sprintf.
 */
#ifndef __SDVOS_PRINTF__
#define __SDVOS_PRINTF__

#include <stdarg.h>

/**
 * @brief Initialize printf
 *
 * Initialize printf and sprintf by providing a user
 * putchar function.
 *
 * @param[in] putc
 *   Function pointer to user putchar
 */
void sdvos_init_printf (void (*putc) (char));

/**
 * @brief Simple printf
 *
 * Simplified version of printf. Supported content types
 * are %u, %d, %x, %X, %c and %s. Only int is supported.
 * No support for long and long long yet. Field width is
 * supported but extra space will be padded with ' ' only.
 *
 * @param[in] fmt
 *   Printf format string
 */
void sdvos_printf (const char * fmt, ...);

/**
 * @brief Simple sprintf
 *
 * Simplified version of sprintf. Same as printf otherwise.
 *
 * @param[out] s
 *   Output buffer
 * @param[in] fmt
 *   Printf format string
 */
void sdvos_sprintf (char * s, const char * fmt, ...);

#endif
/* vi: set et ai sw=2 sts=2: */
