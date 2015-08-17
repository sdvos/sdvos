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

#include <arch/armv7m/syscall.h>
#include <arch/armv7m/types.h>
#include <osek/error.h>

/* Using unified format */
.syntax unified

.globl SvcHandler
.globl PendSVHandler

.section .text

/* This makes the next symbol a thumb encoded function. */
.thumb_func
SvcHandler:
  /* Load caller stack to r0 */
  tst lr, #4
  ite eq
  mrseq r0, msp
  mrsne r0, psp
  /* Load caller PC into r2 */
  ldr r2, [r0, #24]
  /* Get SVC instruction argument */
  ldrb r1, [r2, #-2]
  /* System call number is now in r1 */
  cmp r1, #0
  bne 1f

  /* Syscall 0 is a special service reserved for init */
  /* If called from user (check exception return), ignore. */
  tst lr, #4
  it ne
  bxne lr
  /* Save caller PC on MSP */
  /* Flip thumb bit */
  orrs r2, r2, #1
  push {r2}
  /* Restore r0 - r3, r12, lr, APSR */
  /* Restoration of stack frame will be done by caller. */
  ldr r3, [r0, #28]
  msr apsr_nzcvq, r3
  ldr lr, [r0, #20]
  ldr r12, [r0, #16]
  ldr r3, [r0, #12]
  ldr r2, [r0, #8]
  ldr r1, [r0, #4]
  ldr r0, [r0, #0]
  /* Directly jump to the instruction after svc #0 */
  pop {pc}

1:
  /* Common system call */
  /* r0 = Caller stack  */
  /* r1 = System call number */
  /* r2 = Caller PC */

  /* If called from kernel/handler, ignore. */
  tst lr, #4
  it eq
  bxeq lr

  /*
   * Skip saving context if no preemption will happen.
   * All the system services with an SVC number greater
   * than SVC_MAX_NO_PREEMPT will never incur preemption
   * in the kernel. These do not exclude system services
   * that involve terminating the calling task. These
   * services do not need to save extra context either.
   */
  /* Save lr (EXC_RETURN) on stack */
  /* We will use it to return when no preemption happens */
  push {lr}
  cmp r1, #SVC_MAX_NO_PREEMPT
  bgt 1f

  /* Save context */
  ldr r3, =cur_task
  ldr r3, [r3]
  cmp r3, #0
  beq 1f
  /* Save r4 to r11 and lr to context */
  adds r3, r3, #CONTEXT_OFFSET
  stmia r3, {r4-r11, lr}

1:
  ldr r3, =syscall_vectors
  /* System service function address in r12 */
  ldr r12, [r3, r1, lsl #2]

  /*
   * Before calling actual system services, we put E_OK
   * into r0 in PSP as the system service return value.
   * The actual return value of the system call will be
   * updated later after the call. This is only for system
   * services that could incur kernel preemption. All such
   * system services in SDVOS will only trigger context
   * switch at the end if everything goes well and E_OK
   * should be returned. This is how we can discard the whole
   * kernel stack every time we exit kernel (single kernel
   * stack is used). If preemption did not happen, the real
   * return value will be updated accordingly.
   */
  /* Save first parameter in r0. */
  /* LR can be trashed because it has been saved (stack/TCB). */
  ldr lr, [r0, #0]
  ldr r3, =E_OK
  /* Store E_OK to user r0 */
  str r3, [r0, #0]

  /* Restore parameters */
  ldr r3, [r0, #12]
  ldr r2, [r0, #8]
  ldr r1, [r0, #4]
  movs r0, lr

  /* Call service function */
  blx r12

  /* If we reach this point, context switch did not happen. */
  /* Actual return value in r0. Store in PSP. */
  mrs r1, psp
  str r0, [r1, #0]

  /* No need to restore r4-r11 since they are callee saved */

  /* Return to task directly */
  /* lr (EXC_RETURN) should be top of the stack */
  pop {pc}

/* vi: set et ai sw=2 sts=2: */
