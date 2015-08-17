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
 * @file   src/include/arch/armv7m/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Type Definitions
 */
#ifndef _ARMV7M_TYPES_H_
#define _ARMV7M_TYPES_H_

#ifdef __USE_FPU__
#define CONTEXT_SIZE                               0x64
#else
#define CONTEXT_SIZE                               0x24
#endif

/* Field offsets in processor context structure */
#define CONTEXT_OFFSET                             0x08
#define EXC_OFFSET               (CONTEXT_OFFSET + 0x20)
#define FLAG_OFFSET      (CONTEXT_OFFSET + CONTEXT_SIZE)

#ifdef __USE_FPU__
#define FPU_CONTEXT_OFFSET       (CONTEXT_OFFSET + 0x24)
#endif

#ifndef __ASSEMBLER__

#include <assert.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int int32_t;
typedef signed long long int64_t;

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

typedef uint8_t bool;

/**
 * @union task_context
 * @brief ARMv7-M processor context
 */
typedef union {
  /**
   * @struct regs_struct
   * @brief ARMv7-M registers and flag
   */
  struct regs_struct {
    uint32_t r4;     /**< Low Register R4 */
    uint32_t r5;     /**< Low Register R5 */
    uint32_t r6;     /**< Low Register R6 */
    uint32_t r7;     /**< Low Register R7 */
    uint32_t r8;     /**< High Register R8 */
    uint32_t r9;     /**< High Register R9 */
    uint32_t r10;    /**< High Register R10 */
    uint32_t r11;    /**< High Register R11 */
    uint32_t exc;    /**< EXC_RETURN */
#ifdef __USE_FPU__
    uint32_t s16;    /**< FPU single precision register 16 */
    uint32_t s17;    /**< FPU single precision register 17 */
    uint32_t s18;    /**< FPU single precision register 18 */
    uint32_t s19;    /**< FPU single precision register 19 */
    uint32_t s20;    /**< FPU single precision register 20 */
    uint32_t s21;    /**< FPU single precision register 21 */
    uint32_t s22;    /**< FPU single precision register 22 */
    uint32_t s23;    /**< FPU single precision register 23 */
    uint32_t s24;    /**< FPU single precision register 24 */
    uint32_t s25;    /**< FPU single precision register 25 */
    uint32_t s26;    /**< FPU single precision register 26 */
    uint32_t s27;    /**< FPU single precision register 27 */
    uint32_t s28;    /**< FPU single precision register 28 */
    uint32_t s29;    /**< FPU single precision register 29 */
    uint32_t s30;    /**< FPU single precision register 30 */
    uint32_t s31;    /**< FPU single precision register 31 */
#endif
  } regs;  /**< ARMv7-M callee saved registers */
  uint8_t raw[CONTEXT_SIZE];  /**< Raw byte array */
} task_context;

CASSERT (sizeof (task_context) == CONTEXT_SIZE, task_context);

#endif

#endif

/* vi: set et ai sw=2 sts=2: */
