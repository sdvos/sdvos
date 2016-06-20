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
 * @file   src/include/arch/avr/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR Type Definitions
 */
#ifndef _AVR_TYPES_H_
#define _AVR_TYPES_H_

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int int32_t;
typedef signed long long int64_t;
typedef uint8_t bool;

/* Integer with the same length as data pointer */
typedef uint16_t data_addr_t;

/* Platform default signed data type */
typedef int8_t data_word_t;
/* Platform default unsigned data type */
typedef uint8_t udata_word_t;
/* Type for flags (8-bit minimum) */
typedef uint8_t FlagType;
/* Type for OS service parameter (in debug structure) */
typedef data_addr_t ParamType;

#ifdef __ARCH_AVR5__
#include <arch/avr5/types.h>
#elif defined __ARCH_AVR6__
#include <arch/avr6/types.h>
#endif

#endif

/* vi: set et ai sw=2 sts=2: */
