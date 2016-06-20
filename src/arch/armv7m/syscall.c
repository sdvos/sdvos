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
 * @file   src/arch/armv7m/syscall.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  System Services
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <autosar/autosar.h>
#include <config/config.h>
#include <arch/armv7m/syscall.h>

/**
 * @brief System Service Table
 *
 * System service table used by SVC handler.
 */
code_addr_t * syscall_vectors[NUM_SYSCALLS] ALIGNED(32) = {
  (code_addr_t *) NULL,
  (code_addr_t *) Sys_ActivateTask_Preempt,
  (code_addr_t *) Sys_Schedule,
  (code_addr_t *) Sys_ReleaseResource_Preempt,
  (code_addr_t *) Sys_SetEvent_Preempt,
  (code_addr_t *) Sys_WaitEvent,
  (code_addr_t *) Sys_IncrementCounter_Preempt,
  (code_addr_t *) Sys_TerminateTask,
  (code_addr_t *) Sys_ChainTask,
  (code_addr_t *) Sys_GetTaskID,
  (code_addr_t *) Sys_GetTaskState,
  (code_addr_t *) Sys_DisableAllInterrupts,
  (code_addr_t *) Sys_EnableAllInterrupts,
  (code_addr_t *) Sys_SuspendAllInterrupts,
  (code_addr_t *) Sys_ResumeAllInterrupts,
  (code_addr_t *) Sys_SuspendOSInterrupts,
  (code_addr_t *) Sys_ResumeOSInterrupts,
  (code_addr_t *) Sys_GetResource,
  (code_addr_t *) Sys_ClearEvent,
  (code_addr_t *) Sys_GetEvent,
  (code_addr_t *) Sys_GetAlarmBase,
  (code_addr_t *) Sys_GetAlarm,
  (code_addr_t *) Sys_SetRelAlarm,
  (code_addr_t *) Sys_SetAbsAlarm,
  (code_addr_t *) Sys_CancelAlarm,
  (code_addr_t *) Sys_GetActiveApplicationMode,
  (code_addr_t *) NULL,
  (code_addr_t *) Sys_ShutdownOS,
  (code_addr_t *) Sys_GetCounterValue,
  (code_addr_t *) Sys_GetElapsedValue,
#ifdef USE_SCHEDTBL
  (code_addr_t *) Sys_StartScheduleTableRel,
  (code_addr_t *) Sys_StartScheduleTableAbs,
  (code_addr_t *) Sys_StopScheduleTable,
  (code_addr_t *) Sys_NextScheduleTable,
  (code_addr_t *) Sys_GetScheduleTableStatus,
#ifdef SCHEDTBL_SYNC
  (code_addr_t *) Sys_StartScheduleTableSynchron,
  (code_addr_t *) Sys_SyncScheduleTable,
  (code_addr_t *) Sys_SetScheduleTableAsync,
#endif
#endif
};

/**
 * @brief Check whether we are in handler/kernel mode
 *
 * CONTROL has a minimum of 2 bits in Cortex M3.
 *
 * CONTROL[1] Stack status:
 *   1 = Alternate stack is used
 *   0 = Default stack (MSP) is used
 *
 *   If it is in the thread or base level, the alternate
 *   stack is the PSP. There is no alternate stack for
 *   handler mode, so this bit must be 0 when the processor
 *   is in handler mode.
 *
 * CONTROL[0]
 *   0 = Privileged in thread mode
 *   1 = User state in thread mode
 *
 *   If in handler mode (not thread mode), the processor
 *   operates in privileged mode.
 *
 * User task will always use PSP. As a result, we just need
 * to check CONTROL[1] to know whether we are in user or
 * kernel (handler) mode.
 *
 * @retval TRUE
 *   System is in kernel/handler mode
 * @retval FALSE
 *   System is in user mode
 */
static bool
InKernel ()
{
  uint32_t control = 0;
  __asm__ volatile ("mrs %0, control\n\t":"=l" (control)::);
  if (!(control & 0x2)) return TRUE;
  return FALSE;
}

StatusType
ActivateTask (TaskType tid)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_ActivateTask (tid);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid), "I" (SVC_NO_ACTIVATETASK)
                      :"r0");
  }
  return ret;
}

StatusType
TerminateTask (void)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_TerminateTask ();
  } else {
    __asm__ volatile ("svc %1\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"I" (SVC_NO_TERMINATETASK)
                      :"r0");
  }
  return ret;
}

StatusType
ChainTask (TaskType tid)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_ChainTask (tid);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid), "I" (SVC_NO_CHAINTASK)
                      :"r0");
  }
  return ret;
}

StatusType
Schedule (void)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_Schedule ();
  } else {
    __asm__ volatile ("svc %1\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"I" (SVC_NO_SCHEDULE)
                      :"r0");
  }
  return ret;
}

StatusType
GetTaskID (TaskRefType tid_ref)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetTaskID (tid_ref);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid_ref), "I" (SVC_NO_GETTASKID)
                      :"r0");
  }
  return ret;
}

StatusType
GetTaskState (TaskType tid, TaskStateRefType state_ref)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetTaskState (tid, state_ref);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid), "l" (state_ref),
                       "I" (SVC_NO_GETTASKSTATE)
                      :"r0", "r1");
  }
  return ret;
}

void
EnableAllInterrupts ()
{
  if (InKernel ()) {
    Sys_EnableAllInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_ENABLEALLINTERRUPTS)
                      :);
  }
}

void
DisableAllInterrupts ()
{
  if (InKernel ()) {
    Sys_DisableAllInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_DISABLEALLINTERRUPTS)
                      :);
  }
}

void
ResumeAllInterrupts ()
{
  if (InKernel ()) {
    Sys_ResumeAllInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_RESUMEALLINTERRUPTS)
                      :);
  }
}

void
SuspendAllInterrupts ()
{
  if (InKernel ()) {
    Sys_SuspendAllInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_SUSPENDALLINTERRUPTS)
                      :);
  }
}

void
ResumeOSInterrupts ()
{
  if (InKernel ()) {
    Sys_ResumeOSInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_RESUMEOSINTERRUPT)
                      :);
  }
}

void
SuspendOSInterrupts ()
{
  if (InKernel ()) {
    Sys_SuspendOSInterrupts ();
  } else {
    __asm__ volatile ("svc %0\n\t"
                      :
                      : "I" (SVC_NO_SUSPENDOSINTERRUPTS)
                      :);
  }
}

StatusType
GetResource (ResourceType rid)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetResource (rid);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (rid), "I" (SVC_NO_GETRESOURCE)
                      :"r0");
  }
  return ret;
}

StatusType
ReleaseResource (ResourceType rid)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_ReleaseResource (rid);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (rid), "I" (SVC_NO_RELEASERESOURCE)
                      :"r0");
  }
  return ret;
}

StatusType
SetEvent (TaskType tid, EventMaskType mask)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_SetEvent (tid, mask);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid), "l" (mask),
                       "I" (SVC_NO_SETEVENT)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
ClearEvent (EventMaskType mask)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_ClearEvent (mask);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (mask), "I" (SVC_NO_CLEAREVENT)
                      :"r0");
  }
  return ret;
}

StatusType
GetEvent (TaskType tid, EventMaskRefType event)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetEvent (tid, event);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (tid), "l" (event),
                       "I" (SVC_NO_GETEVENT)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
WaitEvent (EventMaskType mask)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_WaitEvent (mask);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (mask), "I" (SVC_NO_WAITEVENT)
                      :"r0");
  }
  return ret;
}

StatusType
GetAlarmBase (AlarmType alarm, AlarmBaseRefType info)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetAlarmBase (alarm, info);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (alarm), "l" (info),
                       "I" (SVC_NO_GETALARMBASE)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
GetAlarm (AlarmType alarm, TickRefType tick)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetAlarm (alarm, tick);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (alarm), "l" (tick),
                       "I" (SVC_NO_GETALARM)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
SetRelAlarm (AlarmType alarm, TickType inc, TickType cycle)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_SetRelAlarm (alarm, inc, cycle);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "mov r2, %3\n\t"
                      "svc %4\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (alarm), "l" (inc), "l" (cycle),
                       "I" (SVC_NO_SETRELALARM)
                      :"r0", "r1", "r2");
  }
  return ret;
}

StatusType
SetAbsAlarm (AlarmType alarm, TickType start, TickType cycle)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_SetAbsAlarm (alarm, start, cycle);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "mov r2, %3\n\t"
                      "svc %4\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (alarm), "l" (start), "l" (cycle),
                       "I" (SVC_NO_SETABSALARM)
                      :"r0", "r1", "r2");
  }
  return ret;
}

StatusType
CancelAlarm (AlarmType alarm)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_CancelAlarm (alarm);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (alarm), "I" (SVC_NO_CANCELALARM)
                      :"r0");
  }
  return ret;
}

AppModeType
GetActiveApplicationMode ()
{
  AppModeType mode = 0;
  if (InKernel ()) {
    mode = Sys_GetActiveApplicationMode ();
  } else {
    __asm__ volatile ("svc %1\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (mode)
                      :"I" (SVC_NO_GETACTIVEAPPLICATIONMODE)
                      :"r0");
  }
  return mode;
}

void
ShutdownOS (StatusType error)
{
  if (InKernel ()) {
    Sys_ShutdownOS (error);
  } else {
    __asm__ volatile ("mov r0, %0\n\t"
                      "svc %1\n\t"
                      :
                      :"l" (error), "I" (SVC_NO_SHUTDOWNOS)
                      :"r0");
  }
  return;
}

StatusType
IncrementCounter (CounterType CounterID)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_IncrementCounter (CounterID);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (CounterID), "I" (SVC_NO_INCREMENTCOUNTER)
                      :"r0");
  }
  return ret;
}

StatusType
GetCounterValue (CounterType CounterID, TickRefType Value)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetCounterValue (CounterID, Value);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (CounterID), "l" (Value),
                       "I" (SVC_NO_GETCOUNTERVALUE)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
GetElapsedValue (CounterType CounterID, TickRefType Value,
                 TickRefType ElapsedValue)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetElapsedValue (CounterID, Value, ElapsedValue);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "mov r2, %3\n\t"
                      "svc %4\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (CounterID), "l" (Value),
                       "l" (ElapsedValue),
                       "I" (SVC_NO_GETELAPSEDVALUE)
                      :"r0", "r1", "r2");
  }
  return ret;
}

#ifdef USE_SCHEDTBL
StatusType
StartScheduleTableRel (ScheduleTableType ScheduleTableID,
                       TickType Offset)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_StartScheduleTableRel (ScheduleTableID, Offset);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID), "l" (Offset),
                       "I" (SVC_NO_STARTSCHEDULETABLEREL)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
StartScheduleTableAbs (ScheduleTableType ScheduleTableID,
                       TickType Start)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_StartScheduleTableAbs (ScheduleTableID, Start);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID), "l" (Start),
                       "I" (SVC_NO_STARTSCHEDULETABLEABS)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
StopScheduleTable (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_StopScheduleTable (ScheduleTableID);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID),
                       "I" (SVC_NO_STOPSCHEDULETABLE)
                      :"r0");
  }
  return ret;
}

StatusType
NextScheduleTable (ScheduleTableType ScheduleTableID_From,
                   ScheduleTableType ScheduleTableID_To)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_NextScheduleTable (ScheduleTableID_From,
                                 ScheduleTableID_To);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID_From),
                       "l" (ScheduleTableID_To),
                       "I" (SVC_NO_NEXTSCHEDULETABLE)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
GetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_GetScheduleTableStatus (ScheduleTableID,
                                      ScheduleStatus);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID),
                       "l" (ScheduleStatus),
                       "I" (SVC_NO_GETSCHEDULETABLESTATUS)
                      :"r0", "r1");
  }
  return ret;
}

#ifdef SCHEDTBL_SYNC
StatusType
StartScheduleTableSynchron (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_StartScheduleTableSynchron (ScheduleTableID);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID),
                       "I" (SVC_NO_STARTSCHEDULETABLESYNCHRON)
                      :"r0");
  }
  return ret;
}

StatusType
SyncScheduleTable (ScheduleTableType ScheduleTableID,
                   TickType Value)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_SyncScheduleTable (ScheduleTableID, Value);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "mov r1, %2\n\t"
                      "svc %3\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID), "l" (Value),
                       "I" (SVC_NO_SYNCSCHEDULETABLE)
                      :"r0", "r1");
  }
  return ret;
}

StatusType
SetScheduleTableAsync (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  if (InKernel ()) {
    ret = Sys_SetScheduleTableAsync (ScheduleTableID);
  } else {
    __asm__ volatile ("mov r0, %1\n\t"
                      "svc %2\n\t"
                      "mov %0, r0\n\t"
                      :"=l" (ret)
                      :"l" (ScheduleTableID),
                       "I" (SVC_NO_SETSCHEDULETABLEASYNC)
                      :"r0");
  }
  return ret;
}
#endif
#endif

/* vi: set et ai sw=2 sts=2: */
