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

#include <arch/armv7m/types.h>
#include <config/config.h>
#include <task.h>

/* Using unified format */
.syntax unified

.globl SwitchTask

.section .text

/* This makes the next symbol a thumb encoded function. */
.thumb_func
/*
 * R0 - Source TCB
 * R1 - Destination TCB
 */
SwitchTask:
  /* Is source TCB NULL? */
  cmp r0, #0
  beq 1f

  /* Source is not NULL */
#ifdef __USE_FPU__
  /* Check EXC_RETURN to see whether FPU states should be saved */
  /* Load source EXC_RETURN to r2 */
  ldr r2, [r0, #EXC_OFFSET]
  tst r2, #0x10
  /* FPU not used by source task, skip context save */
  itt eq
  /* Save FPU context for source task */
  addeq r2, r0, #FPU_CONTEXT_OFFSET
  vstmiaeq.32 r2, {s16-s31}
#endif

  /* Registers should already have been saved */
  /* Save PSP in TCB */
  mrs r2, psp
  str r2, [r0, #0]
1:
  /* Restore destination context */
  /* Restore PSP */
  ldr r2, [r1]
  msr psp, r2
  /* Restore r4 - r11 and lr (EXC_RETURN) */
  adds r1, r1, #CONTEXT_OFFSET
  ldmia r1!, {r4-r11, lr}

#ifdef __USE_FPU__
  /* Restore FPU states if necessary */
  tst lr, #0x10
  ite eq
  vldmiaeq.32 r1!, {s16-s31}
  addne r1, r1, #0x40
#endif

  /* Check privilege flag and set CONTROL */
  mrs r3, control
  ldrb r2, [r1]
  tst r2, #TASK_PRIVILEGED
  ite eq
  /* Destination task is not privileged */
  /* CONTROL[1] should be 0 */
  /* Set CONTROL[0] to 1, CONTROL[1] to 0 */
  orreq r3, r3, #1
  /* Destination task is privileged */
  /* Set CONTROL[0] to 0, CONTROL[1] to 0 */
  bicne r3, r3, #1
  /* Update CONTROL register */
  msr control, r3
  isb
  /* Reset MSP */
  ldr r2, =KERN_STACK
  msr msp, r2
  bx lr

/* vi: set et ai sw=2 sts=2: */
