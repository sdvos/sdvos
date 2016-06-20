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
 * @file   src/arch/avr5/syscall.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  System Services
 *
 * In AVR5 system services are simply wrappers around
 * internal service implementations which disables
 * interrupt before the service and enables it afterwards.
 * Extra care is taken to determine whether interrupt is
 * already disabled by user task before the service call.
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <config/config.h>

/**
 * @def SysEnter
 * @brief System service prologue
 *
 * This prologue tests current task level interrupt status,
 * disables interrupt and set system processing level to
 * SYS_LV_SCHED.
 */
#define SysEnter()            \
  uint8_t IBit = TstI ();     \
  __asm__ volatile ("cli")

/**
 * @def SysExit
 * @brief System service epilogue
 *
 * This epilogue restores system processing level to task
 * level and restores the original task level interrupt
 * status.
 */
#define SysExit()             \
  if (IBit)  __asm__ volatile ("sei")

StatusType
ActivateTask (TaskType tid)
{
  StatusType ret = E_OK;
  SysEnter ();
  if (NestedISRs) {
    ret = Sys_ActivateTask (tid);
  } else {
    ret = Sys_ActivateTask_Preempt (tid);
  }
  SysExit ();
  return ret;
}

StatusType
TerminateTask (void)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_TerminateTask ();
  SysExit ();
  return ret;
}

StatusType
ChainTask (TaskType tid)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_ChainTask (tid);
  SysExit ();
  return ret;
}

StatusType
Schedule (void)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_Schedule ();
  SysExit ();
  return ret;
}

StatusType
GetTaskID (TaskRefType tid_ref)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetTaskID (tid_ref);
  SysExit ();
  return ret;
}

StatusType
GetTaskState (TaskType tid, TaskStateRefType state_ref)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetTaskState (tid, state_ref);
  SysExit ();
  return ret;
}

AppModeType
GetActiveApplicationMode ()
{
  /* Disabling interrupt is not necessary for this service */
  /* Sys_GetActiveApplicationMode will not return error */
  return Sys_GetActiveApplicationMode ();
}

void
EnableAllInterrupts ()
{
  return Sys_EnableAllInterrupts ();
}

void
DisableAllInterrupts ()
{
  return Sys_DisableAllInterrupts ();
}

void
ResumeAllInterrupts ()
{
  return Sys_ResumeAllInterrupts ();
}

void
SuspendAllInterrupts ()
{
  return Sys_SuspendAllInterrupts ();
}

void
ResumeOSInterrupts ()
{
  return Sys_ResumeOSInterrupts ();
}

void
SuspendOSInterrupts ()
{
  return Sys_SuspendOSInterrupts ();
}

StatusType
GetResource (ResourceType rid)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetResource (rid);
  SysExit ();
  return ret;
}

StatusType
ReleaseResource (ResourceType rid)
{
  StatusType ret = E_OK;
  SysEnter ();
  if (NestedISRs) {
    ret = Sys_ReleaseResource (rid);
  } else {
    ret = Sys_ReleaseResource_Preempt (rid);
  }
  SysExit ();
  return ret;
}

StatusType
SetEvent (TaskType tid, EventMaskType mask)
{
  StatusType ret = E_OK;
  SysEnter ();
  if (NestedISRs) {
    ret = Sys_SetEvent (tid, mask);
  } else {
    ret = Sys_SetEvent_Preempt (tid, mask);
  }
  SysExit ();
  return ret;
}

StatusType
ClearEvent (EventMaskType mask)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_ClearEvent (mask);
  SysExit ();
  return ret;
}

StatusType
GetEvent (TaskType tid, EventMaskRefType event)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetEvent (tid, event);
  SysExit ();
  return ret;
}

StatusType
WaitEvent (EventMaskType mask)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_WaitEvent (mask);
  SysExit ();
  return ret;
}

StatusType
GetAlarmBase (AlarmType alarm, AlarmBaseRefType info)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetAlarmBase (alarm, info);
  SysExit ();
  return ret;
}

StatusType
GetAlarm (AlarmType alarm, TickRefType tick)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetAlarm (alarm, tick);
  SysExit ();
  return ret;
}

StatusType
SetRelAlarm (AlarmType alarm, TickType inc, TickType cycle)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_SetRelAlarm (alarm, inc, cycle);
  SysExit ();
  return ret;
}

StatusType
SetAbsAlarm (AlarmType alarm, TickType start, TickType cycle)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_SetAbsAlarm (alarm, start, cycle);
  SysExit ();
  return ret;
}

StatusType
CancelAlarm (AlarmType alarm)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_CancelAlarm (alarm);
  SysExit ();
  return ret;
}

void
ShutdownOS (StatusType error)
{
  return Sys_ShutdownOS (error);
}

StatusType
IncrementCounter (CounterType CounterID)
{
  StatusType ret = E_OK;
  SysEnter ();
  if (NestedISRs) {
    ret = Sys_IncrementCounter (CounterID);
  } else {
    ret = Sys_IncrementCounter_Preempt (CounterID);
  }
  SysExit ();
  return ret;
}

StatusType
GetCounterValue (CounterType CounterID, TickRefType Value)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetCounterValue (CounterID, Value);
  SysExit ();
  return ret;
}

StatusType
GetElapsedValue (CounterType CounterID, TickRefType Value,
                 TickRefType ElapsedValue)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetElapsedValue (CounterID, Value, ElapsedValue);
  SysExit ();
  return ret;
}

#ifdef USE_SCHEDTBL
StatusType
StartScheduleTableRel (ScheduleTableType ScheduleTableID,
                       TickType Offset)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_StartScheduleTableRel (ScheduleTableID, Offset);
  SysExit ();
  return ret;
}

StatusType
StartScheduleTableAbs (ScheduleTableType ScheduleTableID,
                       TickType Start)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_StartScheduleTableAbs (ScheduleTableID, Start);
  SysExit ();
  return ret;
}

StatusType
StopScheduleTable (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_StopScheduleTable (ScheduleTableID);
  SysExit ();
  return ret;
}

StatusType
NextScheduleTable (ScheduleTableType ScheduleTableID_From,
                   ScheduleTableType ScheduleTableID_To)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_NextScheduleTable (ScheduleTableID_From,
                               ScheduleTableID_To);
  SysExit ();
  return ret;
}

#ifdef SCHEDTBL_SYNC
StatusType
StartScheduleTableSynchron (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_StartScheduleTableSynchron (ScheduleTableID);
  SysExit ();
  return ret;
}

StatusType
SyncScheduleTable (ScheduleTableType ScheduleTableID,
                   TickType Value)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_SyncScheduleTable (ScheduleTableID, Value);
  SysExit ();
  return ret;
}

StatusType
SetScheduleTableAsync (ScheduleTableType ScheduleTableID)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_SetScheduleTableAsync (ScheduleTableID);
  SysExit ();
  return ret;
}
#endif

StatusType
GetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus)
{
  StatusType ret = E_OK;
  SysEnter ();
  ret = Sys_GetScheduleTableStatus (ScheduleTableID, ScheduleStatus);
  SysExit ();
  return ret;
}
#endif

/* vi: set et ai sw=2 sts=2: */
