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
 * @file   src/include/arch/linux/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Type Definitions
 */
#ifndef _LINUX_TYPES_H_
#define _LINUX_TYPES_H_

#ifndef __ASSEMBLER__

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <ucontext.h>

/* Integer with the same length as code pointer */
typedef uint32_t code_addr_t;
/* Integer with the same length as data pointer */
typedef uint32_t data_addr_t;

/* Platform default signed data type */
typedef int32_t data_word_t;
/* Platform default unsigned data type */
typedef uint32_t udata_word_t;
/* Type for flags (8-bit minimum) */
typedef uint8_t FlagType;
/* Type for OS service parameter (in debug structure) */
typedef data_addr_t ParamType;

/**
 * @union task_context
 * @brief Linux process context
 */
typedef union {
  ucontext_t context;                /**< Linux ucontext context */
  uint8_t raw[sizeof (ucontext_t)];  /**< Raw byte array */
} task_context;

#endif  /* __ASSEMBLER__ */

#endif

/* vi: set et ai sw=2 sts=2: */
