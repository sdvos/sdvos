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
 * @file   src/include/arch/armv7m/task.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Task Management Utility
 */
#ifndef _ARMV7M_TASK_H_
#define _ARMV7M_TASK_H_

#include <arch/armv7m/types.h>
#include <arch/armv7m/utils.h>
#include <arch/armv7m/interrupt.h>
#include <task.h>

/**
 * @brief Initialize Processor Context
 */
static inline void
InitContext (TCB * task)
{
  data_addr_t * stk = (data_addr_t *) ((task->bp) - 32);

  /* Clear the task context */
  sdvos_memset (&(task->context.raw[0]), 0, CONTEXT_SIZE);
  /* Set initial EXC_RETURN to thread with PSP */
  task->context.regs.exc = EXC_RETURN_PSP;
  /* Clear initial stack frame */
  sdvos_memset ((void *) stk, 0, 32);

  /*
   * Task Initial Stack
   *
   * +------+
   * | xPSR |
   * |------|
   * |   PC |
   * |------|
   * |   LR |
   * |------|
   * |  R12 |
   * |------|
   * |   R3 |
   * |------|
   * |   R2 |
   * |------|
   * |   R1 |
   * |------|
   * |   R0 | <--- PSP
   * +------+
   */
  /* Set PC to task entry address */
  stk[6] = task->start;
  /* Set T bit in EPSR */
  stk[7] = (0x1U << 24);
  /* All the rest should be 0 */
  /* Bit 9 of xPSR is 0. Stack need to be dword aligned */

  /* Initialize sp. This will be PSP. */
  task->sp = (data_addr_t) stk;
}

#endif

/* vi: set et ai sw=2 sts=2: */
