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
 * @file   src/include/arch/avr5/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Type Definitions
 */
#ifndef _AVR5_TYPES_H_
#define _AVR5_TYPES_H_

/* Size of task context on stack excluding PC (2 bytes) */
/* A context flag is also included */
#define CONTEXT_SIZE    0x24

/* Field offsets in processor context structure */
#define CONTEXT_OFFSET                         0x4
#define CONTEXT_PCH_OFFSET          CONTEXT_OFFSET
#define CONTEXT_PCL_OFFSET    (CONTEXT_OFFSET + 1)
#define CONTEXT_SREG_OFFSET   (CONTEXT_OFFSET + 2)
#define CONTEXT_REGS_OFFSET   (CONTEXT_OFFSET + 3)
#define CONTEXT_FLAG_OFFSET   (CONTEXT_OFFSET + 35)
#define PREEMPT_FLAG_OFFSET   (CONTEXT_OFFSET + CONTEXT_SIZE)

#ifndef __ASSEMBLER__

#include <assert.h>

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;
typedef unsigned long int uint32_t;
typedef unsigned long long uint64_t;

typedef signed char int8_t;
typedef signed int int16_t;
typedef signed long int int32_t;
typedef signed long long int64_t;

/* Integer with the same length as code pointer */
typedef uint16_t code_addr_t;
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

typedef uint8_t bool;

/**
 * @union task_context
 * @brief AVR5 processor context
 *
 * AVR5 general registers, Program Counter, Status Register
 * and context flag.
 */
typedef union {
  /**
   * @struct regs_struct
   * @brief AVR5 registers and flag
   */
  struct regs_struct {
    uint8_t pch;    /**< High byte of PC */
    uint8_t pcl;    /**< Low byte of PC */
    uint8_t sreg;   /**< Status Register */
    uint8_t r0;     /**< General Register r0 */
    uint8_t r1;     /**< General Register r1 */
    uint8_t r2;     /**< General Register r2 */
    uint8_t r3;     /**< General Register r3 */
    uint8_t r4;     /**< General Register r4 */
    uint8_t r5;     /**< General Register r5 */
    uint8_t r6;     /**< General Register r6 */
    uint8_t r7;     /**< General Register r7 */
    uint8_t r8;     /**< General Register r8 */
    uint8_t r9;     /**< General Register r9 */
    uint8_t r10;    /**< General Register r10 */
    uint8_t r11;    /**< General Register r11 */
    uint8_t r12;    /**< General Register r12 */
    uint8_t r13;    /**< General Register r13 */
    uint8_t r14;    /**< General Register r14 */
    uint8_t r15;    /**< General Register r15 */
    uint8_t r16;    /**< General Register r16 */
    uint8_t r17;    /**< General Register r17 */
    uint8_t r18;    /**< General Register r18 */
    uint8_t r19;    /**< General Register r19 */
    uint8_t r20;    /**< General Register r20 */
    uint8_t r21;    /**< General Register r21 */
    uint8_t r22;    /**< General Register r22 */
    uint8_t r23;    /**< General Register r23 */
    uint8_t r24;    /**< General Register r24 */
    uint8_t r25;    /**< General Register r25 */
    uint8_t r26;    /**< General Register r26 */
    uint8_t r27;    /**< General Register r27 */
    uint8_t r28;    /**< General Register r28 */
    uint8_t r29;    /**< General Register r29 */
    uint8_t r30;    /**< General Register r30 */
    uint8_t r31;    /**< General Register r31 */
    /**
     * @brief Flag indicating context type.
     *
     * 0 means Full Context, 1 means Reduced Context.
     * Full context is only used in category 2 ISR.
     */
    uint8_t ctx_flag;
  } regs;  /**< AVR5 registers and flag */
  uint8_t raw[CONTEXT_SIZE];  /**< Raw byte array */
} task_context;

CASSERT (sizeof (task_context) == CONTEXT_SIZE, task_context);

#endif  /* __ASSEMBLER__ */

#endif

/* vi: set et ai sw=2 sts=2: */
