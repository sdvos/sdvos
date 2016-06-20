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
 * @file   src/include/arch/avr5/task.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Task Management Utility
 */
#ifndef _AVR5_TASK_H_
#define _AVR5_TASK_H_

#include <arch/avr5/types.h>
#include <arch/avr5/utils.h>
#include <task.h>

/**
 * @def IRQSaveContext()
 * @brief Save processor context for Category 2 ISR.
 *
 * System services can be called in Category 2 ISR
 * and at the end of Category 2 ISR, rescheduling
 * might occur. We need to pessimistically save the
 * whole processor context. This macro will be placed
 * at the beginning of each category 2 ISR automatically.
 */
#define IRQSaveContext()                          \
  __asm__ volatile ("push r31\n\t"                \
                    "push r30\n\t");              \
  __asm__ volatile ("push r0\n\t"                 \
                    "in r0, 0x3F\n\t"             \
                    "sbiw r30, 0x0\n\t"           \
                    "brne 1f\n\t"                 \
                    "call panic\n\t"              \
                    "1:\n\t"                      \
                    "std z+%0, r0\n\t"            \
                    /* Set ctx_flag to 0 */       \
                    "clr r0\n\t"                  \
                    "std z+%4, r0\n\t"            \
                    "pop r0\n\t"                  \
                    "std z+%1, r0\n\t"            \
                    "std z+%1+1, r1\n\t"          \
                    "std z+%1+2, r2\n\t"          \
                    "std z+%1+3, r3\n\t"          \
                    "std z+%1+4, r4\n\t"          \
                    "std z+%1+5, r5\n\t"          \
                    "std z+%1+6, r6\n\t"          \
                    "std z+%1+7, r7\n\t"          \
                    "std z+%1+8, r8\n\t"          \
                    "std z+%1+9, r9\n\t"          \
                    "std z+%1+10, r10\n\t"        \
                    "std z+%1+11, r11\n\t"        \
                    "std z+%1+12, r12\n\t"        \
                    "std z+%1+13, r13\n\t"        \
                    "std z+%1+14, r14\n\t"        \
                    "std z+%1+15, r15\n\t"        \
                    "std z+%1+16, r16\n\t"        \
                    "std z+%1+17, r17\n\t"        \
                    "std z+%1+18, r18\n\t"        \
                    "std z+%1+19, r19\n\t"        \
                    "std z+%1+20, r20\n\t"        \
                    "std z+%1+21, r21\n\t"        \
                    "std z+%1+22, r22\n\t"        \
                    "std z+%1+23, r23\n\t"        \
                    "std z+%1+24, r24\n\t"        \
                    "std z+%1+25, r25\n\t"        \
                    "std z+%1+26, r26\n\t"        \
                    "std z+%1+27, r27\n\t"        \
                    "std z+%1+28, r28\n\t"        \
                    "std z+%1+29, r29\n\t"        \
                    "pop r0\n\t"                  \
                    "std z+%1+30, r0\n\t"         \
                    "pop r0\n\t"                  \
                    "std z+%1+31, r0\n\t"         \
                    "pop r0\n\t"                  \
                    "std z+%2, r0\n\t"            \
                    "pop r0\n\t"                  \
                    "std z+%3, r0\n\t"            \
                    /* Save SP in context */      \
                    "in r0, 0x3D\n\t"             \
                    "st z, r0\n\t"                \
                    "in r0, 0x3E\n\t"             \
                    "std z+1, r0\n\t"             \
                    : /* No ouput */              \
                    : "I" (CONTEXT_SREG_OFFSET),  \
                      "I" (CONTEXT_REGS_OFFSET),  \
                      "I" (CONTEXT_PCH_OFFSET),   \
                      "I" (CONTEXT_PCL_OFFSET),   \
                      "I" (CONTEXT_FLAG_OFFSET),  \
                      "z" (cur_task))
/**
 * @def IRQRestoreContext()
 * @brief Restore processor context for Category 2 ISR.
 *
 * Counterpart to IRQSaveContext macro. It will be placed
 * at the bottom of each category 2 ISR automatically. It
 * is only used if preemption did not occur.
 */
#define IRQRestoreContext()                       \
  __asm__ volatile ("sbiw r30, 0x0\n\t"           \
                    "brne 1f\n\t"                 \
                    "call panic\n\t"              \
                    "1:\n\t"                      \
                    "ldd r0, z+%0\n\t"            \
                    "out 0x3F, r0\n\t"            \
                    /* Restore SP */              \
                    "ld r0, z\n\t"                \
                    "out 0x3D, r0\n\t"            \
                    "ldd r0, z+1\n\t"             \
                    "out 0x3E, r0\n\t"            \
                    "ldd r0, z+%3\n\t"            \
                    "push r0\n\t"                 \
                    "ldd r0, z+%2\n\t"            \
                    "push r0\n\t"                 \
                    "ldd r0, z+%1\n\t"            \
                    "ldd r1, z+%1+1\n\t"          \
                    "ldd r2, z+%1+2\n\t"          \
                    "ldd r3, z+%1+3\n\t"          \
                    "ldd r4, z+%1+4\n\t"          \
                    "ldd r5, z+%1+5\n\t"          \
                    "ldd r6, z+%1+6\n\t"          \
                    "ldd r7, z+%1+7\n\t"          \
                    "ldd r8, z+%1+8\n\t"          \
                    "ldd r9, z+%1+9\n\t"          \
                    "ldd r10, z+%1+10\n\t"        \
                    "ldd r11, z+%1+11\n\t"        \
                    "ldd r12, z+%1+12\n\t"        \
                    "ldd r13, z+%1+13\n\t"        \
                    "ldd r14, z+%1+14\n\t"        \
                    "ldd r15, z+%1+15\n\t"        \
                    "ldd r16, z+%1+16\n\t"        \
                    "ldd r17, z+%1+17\n\t"        \
                    "ldd r18, z+%1+18\n\t"        \
                    "ldd r19, z+%1+19\n\t"        \
                    "ldd r20, z+%1+20\n\t"        \
                    "ldd r21, z+%1+21\n\t"        \
                    "ldd r22, z+%1+22\n\t"        \
                    "ldd r23, z+%1+23\n\t"        \
                    "ldd r24, z+%1+24\n\t"        \
                    "ldd r25, z+%1+25\n\t"        \
                    "ldd r26, z+%1+26\n\t"        \
                    "ldd r27, z+%1+27\n\t"        \
                    "ldd r28, z+%1+28\n\t"        \
                    "ldd r29, z+%1+29\n\t"        \
                    "push r26\n\t"                \
                    "push r27\n\t"                \
                    "ldd r26, z+%1+30\n\t"        \
                    "ldd r27, z+%1+31\n\t"        \
                    "movw r30, r26\n\t"           \
                    "pop r27\n\t"                 \
                    "pop r26\n\t"                 \
                    : /* No ouput */              \
                    : "I" (CONTEXT_SREG_OFFSET),  \
                      "I" (CONTEXT_REGS_OFFSET),  \
                      "I" (CONTEXT_PCH_OFFSET),   \
                      "I" (CONTEXT_PCL_OFFSET),   \
                      "z" (cur_task))

/**
 * @brief Initialize Processor Context
 *
 * Initialize AVR5 processor context for a given task.
 *
 * @param[in] task
 *   Reference to the TCB of the task whose context will
 *   be initialized.
 * @return
 *   None
 */
static inline void
InitContext (TCB * task)
{
  /* PC is pushed to stack by call instruction in BIG-ENDIAN */
  task->context.regs.pch = ((task->start) >> 8) & 0xFF;
  task->context.regs.pcl = (task->start) & 0xFF;
  sdvos_memset (&(task->context.raw[2]), 0, CONTEXT_SIZE - 2);
  /* Enable interrupt in task level */
  /* Global Interrupt Enable: Bit 7 (I) in SREG */
  task->context.regs.sreg |= (1 << 7);
}

#endif

/* vi: set et ai sw=2 sts=2: */
