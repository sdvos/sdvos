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
 * @file   src/event.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Event Control
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <config/config.h>

StatusType
Sys_SetEvent (TaskType tid, EventMaskType mask)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid valid? */
  ValidateTaskID (tid);
  /* Is tid extended task? */
  if (!(tasks[tid].flag & TASK_EXTENDED)) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Is tid in suspended state? */
  if (tasks[tid].state == SUSPENDED) {
    ret = E_OS_STATE;
    goto std_ret;
  }
#endif

  /* Set events specified by mask */
  tasks[tid].cevent |= mask;
  if (tasks[tid].wevent & mask) {
    /*
     * Task tid is waiting for at least one of the events
     * specified in mask. Task tid should be transferred to
     * the ready state.
     */
    if (tasks[tid].state == WAITING) {
      tasks[tid].state = READY;
      EnqueueTaskTail (tid);
    }
  }

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_SetEvent, tid, mask, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_SetEvent_Preempt (TaskType tid, EventMaskType mask)
{
  StatusType ret = E_OK;
  ret = Sys_SetEvent (tid, mask);
  if (ret == E_OK) CheckPreemption (PREEMPT_SCHED);
  return ret;
}

StatusType
Sys_ClearEvent (EventMaskType mask)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid extended task? */
  if (!(cur_task->flag & TASK_EXTENDED)) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Check whether OS is at interrupt level */
  if (NestedISRs) {
    ret = E_OS_CALLEVEL;
    goto std_ret;
  }
#endif

  /* Clear current event status */
  cur_task->cevent &= ((EventMaskType) (~mask));

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_ClearEvent, mask, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_GetEvent (TaskType tid, EventMaskRefType event)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid valid? */
  ValidateTaskID (tid);
  /* Is tid extended task? */
  if (!(tasks[tid].flag & TASK_EXTENDED)) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Is tid in suspended state? */
  if (tasks[tid].state == SUSPENDED) {
    ret = E_OS_STATE;
    goto std_ret;
  }
#endif

  *event = tasks[tid].cevent;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetEvent, tid, event, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_WaitEvent (EventMaskType mask)
{
  TaskType tid = 0;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid extended task? */
  if (!(cur_task->flag & TASK_EXTENDED)) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Check whether current task still occupies resource */
  if (cur_task->res) {
    ret = E_OS_RESOURCE;
    goto std_ret;
  }
  /* Check whether OS is at interrupt level */
  if (NestedISRs) {
    ret = E_OS_CALLEVEL;
    goto std_ret;
  }
#endif

  /* At least one of the events in mask is set. Do nothing. */
  if (cur_task->cevent & mask) {
    ret = E_OK;
    goto std_ret;
  }

  /* None of the events in mask is set. Need to block. */
  cur_task->wevent = mask;
  /* Block and re-schedule */
  NextTask (&tid, MAX_PRIO, 0);

  /*
   * PostTaskHook is called after executing the current
   * task, but before leaving the task's running state.
   */
  POSTTASKHOOK ();
  cur_task->state = WAITING;
  /* Release internal resource */
  cur_task->priority = cur_task->orig_prio;
  Dispatch (tid, DISPATCH_BLOCK);

std_ret:
  SaveOSErrorService (OSServiceId_WaitEvent, mask, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

/* vi: set et ai sw=2 sts=2: */
