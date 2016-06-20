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
 * @file   src/include/arch/linux/task.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Task Management Utility
 */
#ifndef _LINUX_TASK_H_
#define _LINUX_TASK_H_

#include <arch/linux/types.h>
#include <arch/linux/utils.h>
#include <task.h>
#include <ucontext.h>
#include <signal.h>
#include <stdlib.h>

/**
 * @brief Initialize Processor Context
 *
 * Initialize Linux process context for a given task.
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
  extern void panic (void);
  if (getcontext (&(task->context.context)) == -1) {
    panic ();
  }
  task->context.context.uc_stack.ss_sp = (void *) task->sp_end;
  task->context.context.uc_stack.ss_size = (task->bp - task->sp_end);
  task->context.context.uc_stack.ss_flags = 0;
  task->context.context.uc_link = (void *) 0;
  /* Clear the signal mask */
  sigemptyset (&(task->context.context.uc_sigmask));
  makecontext (&(task->context.context),
               (void (*) (void)) (task->start), 0);
}

#endif

/* vi: set et ai sw=2 sts=2: */
