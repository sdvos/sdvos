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
 * @file   src/include/sdvos.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Internal System Services
 */
#ifndef _SDVOS_H_
#define _SDVOS_H_

#include <osek/types.h>
#include <counter.h>
#include <task.h>
#include <debug.h>

#ifdef __ARCH_I386__
#include <arch/i386/mcu.h>
#include <arch/i386/task.h>
#include <arch/i386/atomic.h>
#elif defined __ARCH_AVR5__
#include <arch/avr5/mcu.h>
#include <arch/avr5/task.h>
#include <arch/avr5/atomic.h>
#elif defined __ARCH_ARMV7M__
#include <arch/armv7m/mcu.h>
#include <arch/armv7m/task.h>
#include <arch/armv7m/atomic.h>
#elif defined __ARCH_LINUX__
#include <arch/linux/mcu.h>
#include <arch/linux/task.h>
#include <arch/linux/atomic.h>
#else
/* Use i386 simulation as default */
#include <arch/i386/mcu.h>
#include <arch/i386/task.h>
#include <arch/i386/atomic.h>
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

/**
 * @def KERNEL_VERSION
 * @brief Convert kernel version to an integer
 *
 * Convert a.b.c version to integer aabbcc. The bigger the
 * number, the newer the version.
 *
 * @param[in] a
 *   Kernel version number (2 digits max)
 * @param[in] b
 *   Major revision number (2 digits max)
 * @param[in] c
 *   Minor revision number (2 digits max)
 */
#define KERNEL_VERSION(a, b, c)    (a * 10000 + b * 100 + c)

/** Current kernel version integer */
#define CURRENT_KERNEL_VERSION     KERNEL_VERSION(0, 1, 0)

/**
 * @def GET_KERNEL_VERSION
 * @brief Return kernel version from full version integer
 *
 * @param[in]
 *   Full kernel version integer
 */
#define GET_KERNEL_VERSION(v)                 \
  (CURRENT_KERNEL_VERSION / 10000)

/**
 * @def GET_MAJOR_REVISION
 * @brief
 * Return kernel major revision number from full version
 * integer
 *
 * @param[in]
 *   Full kernel version integer
 */
#define GET_MAJOR_REVISION(v)                 \
  ((CURRENT_KERNEL_VERSION % 10000) / 100)

/**
 * @def GET_MINOR_REVISION
 * @brief
 * Return kernel minor revision number from full version
 * integer
 *
 * @param[in]
 *   Full kernel version integer
 */
#define GET_MINOR_REVISION(v)                  \
  (CURRENT_KERNEL_VERSION % 100)

/** Counter used for nested ISR level count */
extern uatomic_t NestedISRs;

/** List of all Category 1 interrupt numbers */
extern IRQType isr1_list[NUM_ISR1];
/** List of all Category 2 interrupt numbers */
extern IRQType isr2_list[NUM_ISR2];

/**
 * @def MAX
 * @brief Return the bigger of value a and b
 *
 * @param[in] a
 *   Value a
 * @param[in] b
 *   Value b
 */
#define MAX(a, b)    ((a) > (b)) ? (a) : (b)

/**
 * @def MIN
 * @brief Return the smaller of value a and b
 *
 * @param[in] a
 *   Value a
 * @param[in] b
 *   Value b
 */
#define MIN(a, b)    ((a) > (b)) ? (b) : (a)

/**
 * @def ValidateTaskID
 * @brief Check whether tid is invalid
 *
 * ValidateTaskID is used internally by system services
 * to check whether a given task ID is valid. It sets
 * return value to E_OS_ID and goes to std_ret label for the
 * CALLING function when tid is invalid. 'ret' and label
 * 'std_ret' must be defined in the calling function locally
 * before using this macro.
 *
 * @param[in] tid
 *   Task ID
 */
#define ValidateTaskID(tid)  do {         \
  if (tid >= NUM_TASKS) {                 \
    ret = E_OS_ID;                        \
    goto std_ret;                         \
  }                                       \
} while (0)

/**
 * @def ValidateAlarm
 * @brief Check whether an alarm is invalid
 *
 * ValidateAlarm is used internally by system services
 * to check whether a given alarm is valid. It sets return
 * value to E_OS_ID and goes to label 'std_ret' for the
 * CALLING function when alarm is invalid. 'ret' and label
 * 'std_ret' must be defined in the calling function locally
 * before using this macro.
 *
 * @param[in] alarm
 *   Alarm ID of type AlarmType
 */
#define ValidateAlarm(alarm)  do {        \
  if (alarm >= NUM_ALARMS) {              \
    ret =  E_OS_ID;                       \
    goto std_ret;                         \
  }                                       \
} while (0)

/**
 * @def ValidateCounter
 * @brief Check whether a counter is invalid
 *
 * ValidateCounter is used internally by system services
 * to check whether a given counter is valid. It sets return
 * value to E_OS_ID and goes to label 'std_ret' for the
 * CALLING function when counter is invalid. 'ret' and label
 * 'std_ret' must be defined in the calling function locally
 * before using this macro.
 *
 * @param[in] counter
 *   Counter ID of type CounterType
 */
#define ValidateCounter(counter)  do {    \
  if (counter >= NUM_COUNTERS) {          \
    ret =  E_OS_ID;                       \
    goto std_ret;                         \
  }                                       \
} while (0)

/**
 * @brief Add a task to the tail of its priority queue.
 *
 * This will make the specified task the "newest" task
 * in its priority queue and get it scheduled last.
 *
 * @param[in] tid
 *   Task to be enqueued.
 * @return
 *   None
 */
void EnqueueTaskTail (TaskType tid);

/**
 * @brief Add a task to the head of its priority queue.
 *
 * This will make the specified task the "oldest" task
 * in its priority queue and get it scheduled first.
 *
 * @param[in] tid
 *   Task to be enqueued.
 * @return
 *   None
 */
void EnqueueTaskHead (TaskType tid);

/**
 * @brief Change a task into running state
 *
 * This function changes task state to RUNNING and handles
 * internal resource if it exists for the specified task.
 *
 * Internal resource should not be released when task is
 * preempted. However, it is save to set the priority of a
 * preempted task to its internal resource's ceiling
 * priority when it gets scheduled again if there is no
 * standard resource acquired at the time. And when a task
 * releases all its standard resources, its priority will
 * be restored to its internal resource's ceiling priority.
 *
 * @param[in] task
 *   Reference to a TCB
 */
void MakeRunning (TCB * task);

/**
 * @brief Architectural dependent panic function.
 *
 * The implementation may choose to simply halt, print a
 * message or print debug information such as stack trace
 * if possible.
 */
void panic (void);

/**
 * @brief Architectural dependent function of task switch.
 * 
 * Execution context of source task will be preserved
 * in its TCB. This function returns when the calling
 * task gets re-scheduled.
 *
 * SwitchTask will fail (silently/panic) if destination task
 * is NULL or src == dst. Caller has to make sure these will
 * not happen. If src is NULL, context restore will be
 * skipped and control flow will be directly switched to
 * destination task. SwitchTask is only used internally.
 * Extra task switch procedures such as changing task states
 * and switch current task are not included in this service.
 *
 * @param[in] src
 *   Reference to source TCB
 * @param[in] dst
 *   Reference to destination TCB
 * @return
 *   None
 */
void SwitchTask (TCB * src, TCB * dst);

/**
 * @brief Find the next highest priority ready task.
 *
 * NextTask returns INVALID_TASK when there is no ready
 * task in the system. Otherwise, it returns the next
 * highest priority available task in segment [min, max]
 * (inclusive) and remove it from its priority ready
 * queue. Use [0, MAX_PRIO] as the segment if next highest
 * priority ready task is needed.
 *
 * @param[out] tid_ref
 *   Reference to the current highest priority ready task.
 *   INVALID_TASK if there is no ready task.
 * @param[in] max
 *   Ceiling of the priority segment
 * @param[in] min
 *   Floor of the priority segment
 * @return
 *   None
 */ 
void NextTask (TaskRefType tid_ref, PrioType max,
               PrioType min);

/**
 * @brief Initializes task stack and processor context.
 *
 * InitTask initializes stack and processor context for
 * specified task. This function is architectural dependent.
 * The result of SwitchTask with a destination task not
 * properly initialized is undefined.
 *
 * @param[in] task
 *   Reference to TCB
 * @return
 *   None
 */
void InitTask (TCB * task);

/** 
 * @brief Schedule the next highest priority ready task.
 *
 * JumpNext finds the next available highest priority
 * application task and jumps to it directly without saving
 * the context of current running task.
 */
void JumpNext (void);

#define DISPATCH_DISCARD     0x0
#define DISPATCH_BLOCK       0x1

/**
 * @brief Task dispatcher
 *
 * Dispatch handles the context switch from current task
 * to a given destination task. Besides calling SwitchTask,
 * it also takes care of setting task states (for target
 * task) and system level. The exact behavior of the
 * dispather is based on the flag set. If DISPATCH_BLOCK is
 * used, the context of the calling task will be preserved.
 * If DISPATCH_DISCARD is used, the context of the calling
 * task will be discarded.
 *
 * @param[in] tid
 *   Task to be dispatched
 * @param[in] flag
 *   Dispatcher flag: DISPATCH_DISCARD or DISPATCH_BLOCK
 */
void Dispatch (TaskType tid, FlagType flag);

#define PREEMPT_ISR      0x0
#define PREEMPT_SCHED    0x1

/**
 * @brief Check for preemption.
 *
 * CheckPreemption can only be called from ISR category 2 or
 * logical level for scheduling (normal kernel level) with an
 * application task context (has cur_task set, not in hooks
 * and call-backs). If current task is preemptable and there
 * is higher priority ready task, current task will be
 * preempted. Otherwise, CheckPreemption will simply return.
 *
 * PREEMPT_ISR or PREEMPT_SCHED must be specified as the
 * calling context. If PREEMPT_ISR is specified, source
 * task's context might not be preserved since the ISR
 * prologue could have saved it already.
 *
 * @param[in] flag
 *   Preemption flag: PREEMPT_ISR or PREEMPT_SCHED
 *
 * @return
 *   None
 */
void CheckPreemption (FlagType flag);

/**
 * @brief Interrupt Management Initialization.
 */
void InterruptInit (void);

/**
 * @brief Alarm/Counter Service Initialization.
 */
void AlarmInit (void);

/**
 * @brief Internal service implementation for ActivateTask
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Task reference
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_LIMIT
 *   (Standard) Too many task activations of tid
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 */
StatusType Sys_ActivateTask (TaskType tid);

/**
 * @brief
 * Internal service implementation for ActivateTask with
 * preemption.
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Task reference
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_LIMIT
 *   (Standard) Too many task activations of tid
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 */
StatusType Sys_ActivateTask_Preempt (TaskType tid);

/**
 * @brief Internal service implementation for TerminateTask
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @retval None
 *   (Standard) Nothing
 * @retval E_OS_RESOURCE
 *   (Extended) Task still occupies resource
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Sys_TerminateTask (void);

/**
 * @brief Internal service implementation for ChainTask
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Reference to the sequential succeeding task to be
 *   activated
 *
 * @retval None
 *   (Standard) Nothing
 * @retval E_OS_LIMIT
 *   (Standard) Too many task activations of tid
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 * @retval E_OS_RESOURCE
 *   (Extended) Calling task still occupies resources
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Sys_ChainTask (TaskType tid);

/**
 * @brief Internal service implementation for Schedule
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_RESOURCE
 *   (Extended) Calling task still occupies resources
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Sys_Schedule (void);

/**
 * @brief Internal service implementation for GetTaskID
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid_ref
 *   Reference to the task which is currently running
 *
 * @retval E_OK
 *   No error
 */
StatusType Sys_GetTaskID (TaskRefType tid_ref);

/**
 * @brief Internal service implementation for GetTaskState
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Task reference
 * @param[out] state_ref
 *   Reference to the state of the task tid
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) Task tid is invalid
 */ 
StatusType Sys_GetTaskState (TaskType tid,
                             TaskStateRefType state_ref);

/**
 * @brief Internal service implementation for
 * GetActiveApplicationMode.
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @return
 *   Application mode
 */
AppModeType Sys_GetActiveApplicationMode (void);

/**
 * @brief Internal service implementation for GetResource
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] rid
 *   Reference to resource
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) Resource rid is invalid
 * @retval E_OS_ACCESS
 *   (Extended) Attempt to get a resource which is already
 *   occupied by any task or ISR, or the statically assigned
 *   priority of the calling task or interrupt routine is
 *   higher than the calculated ceiling priority
 */
StatusType Sys_GetResource (ResourceType rid);

/**
 * @brief Internal service implementation for ReleaseResource
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] rid
 *   Reference to resource
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) Resource rid is invalid
 * @retval E_OS_NOFUNC
 *   (Extended) Attempt to release a resource which is not
 *   occupied by any task or ISR, or another resource shall
 *   be released before.
 * @retval E_OS_ACCESS
 *   (Extended) Attempt to release a resource which has a
 *   lower ceiling priority than the statically assigned
 *   priority of the calling task or interrupt routine.
 */
StatusType Sys_ReleaseResource (ResourceType rid);

/**
 * @brief
 * Internal service implementation for ReleaseResource
 * with preemption.
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] rid
 *   Reference to resource
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) Resource rid is invalid
 * @retval E_OS_NOFUNC
 *   (Extended) Attempt to release a resource which is not
 *   occupied by any task or ISR, or another resource shall
 *   be released before.
 * @retval E_OS_ACCESS
 *   (Extended) Attempt to release a resource which has a
 *   lower ceiling priority than the statically assigned
 *   priority of the calling task or interrupt routine.
 */
StatusType Sys_ReleaseResource_Preempt (ResourceType rid);

/**
 * @brief Internal service implementation for SetEvent
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Reference to the task for which one or several events
 *   are to be set.
 * @param[in] mask
 *   Mask of the events to be set.
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 * @retval E_OS_ACCESS
 *   (Extended) Referenced task is no extended task
 * @retval E_OS_STATE
 *   (Extended) Events can not be set as the referenced
 *   task is in the suspended state
 */
StatusType Sys_SetEvent (TaskType tid, EventMaskType mask);

/**
 * @brief
 * Internal service implementation for SetEvent with
 * preemption.
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Reference to the task for which one or several events
 *   are to be set.
 * @param[in] mask
 *   Mask of the events to be set.
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 * @retval E_OS_ACCESS
 *   (Extended) Referenced task is no extended task
 * @retval E_OS_STATE
 *   (Extended) Events can not be set as the referenced
 *   task is in the suspended state
 */
StatusType Sys_SetEvent_Preempt (TaskType tid, EventMaskType mask);

/**
 * @brief Internal service implementation for ClearEvent
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] mask
 *   Mask of the events to be cleared
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ACCESS
 *   (Extended) Call not from extended task
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Sys_ClearEvent (EventMaskType mask);

/**
 * @brief Internal service implementation for GetEvent
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] tid
 *   Task whose event mask is to be returned
 * @param[out] event
 *   Reference to the memory of the return data
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) tid is invalid
 * @retval E_OS_ACCESS
 *   (Extended) Referenced task is not an extended task
 * @retval E_OS_STATE
 *   (Extended) Referenced task is in suspended state
 */
StatusType Sys_GetEvent (TaskType tid, EventMaskRefType event);

/**
 * @brief Internal service implementation for WaitEvent
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] mask
 *   Mask of the events waited for
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ACCESS
 *   (Extended) Calling task is not an extended task
 * @retval E_OS_RESOURCE
 *   (Extended) Calling task occupies resource
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Sys_WaitEvent (EventMaskType mask);

/**
 * @brief Internal service implementation for GetAlarmBase
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] alarm
 *   Reference to alarm
 * @param[out] info
 *   Reference to structure with constants of the alarm base
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) Alarm <alarm> is invalid
 */
StatusType Sys_GetAlarmBase (AlarmType alarm,
                             AlarmBaseRefType info);

/**
 * @brief Internal service implementation for GetAlarm
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] alarm
 *   Reference to an alarm
 * @param[out] tick
 *   Relative value in ticks before the alarm <alarm> expires
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_NOFUNC
 *   (Standard) Alarm <alarm> is not used
 * @retval E_OS_ID
 *   (Extended) Alarm <alarm> is invalid
 */
StatusType Sys_GetAlarm (AlarmType alarm, TickRefType tick);

/**
 * @brief Internal service implementation for SetRelAlarm
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] alarm
 *   Reference to the alarm element
 * @param[in] inc
 *   Relative value in ticks
 * @param[in] cycle
 *   Cycle value in case of cyclic alarm. In case of single
 *   alarms, cycle shall be zero.
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_STATE
 *   (Standard) Alarm <alarm> is already in use
 * @retval E_OS_ID
 *   (Extended) Alarm <alarm> is invalid
 * @retval E_OS_VALUE
 *   (Extended) Value of <inc> outside of the admissible
 *   limits
 * @retval E_OS_VALUE
 *   (Extended) Value of <cycle> unequal to 0 and outside
 *   of the admissible counter limits
 */
StatusType Sys_SetRelAlarm (AlarmType alarm, TickType inc,
                            TickType cycle);

/**
 * @brief Internal service implementation for SetAbsAlarm
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] alarm
 *   Reference to the alarm element
 * @param[in] start
 *   Absolute value in ticks
 * @param[in] cycle
 *   Cycle value in case of cyclic alarm. In case of single
 *   alarms, cycle shall be zero.
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_STATE
 *   (Standard) Alarm <alarm> is already in use
 * @retval E_OS_ID
 *   (Extended) Alarm <alarm> is invalid
 * @retval E_OS_VALUE
 *   (Extended) Value of <start> outside of the admissible
 *   limits
 * @retval E_OS_VALUE
 *   (Extended) Value of <cycle> unequal to 0 and outside
 *   of the admissible counter limits
 */
StatusType Sys_SetAbsAlarm (AlarmType alarm, TickType start,
                            TickType cycle);

/**
 * @brief Internal service implementation for CancelAlarm
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] alarm
 *   Reference to an alarm
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_NOFUNC
 *   (Standard) Alarm <alarm> is not used
 * @retval E_OS_ID
 *   (Extended) Alarm <alarm> is invalid
 */
StatusType Sys_CancelAlarm (AlarmType alarm);

/**
 * @brief
 * Internal service implementation for EnableAllInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_EnableAllInterrupts (void);

/**
 * @brief
 * Internal service implementation for DisableAllInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_DisableAllInterrupts (void);

/**
 * @brief
 * Internal service implementation for ResumeAllInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_ResumeAllInterrupts (void);

/**
 * @brief
 * Internal service implementation for SuspendAllInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_SuspendAllInterrupts (void);

/**
 * @brief
 * Internal service implementation for ResumeOSInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_ResumeOSInterrupts (void);

/**
 * @brief
 * Internal service implementation for SuspendOSInterrupts
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 */
void Sys_SuspendOSInterrupts (void);

/**
 * @brief Internal service implementation for ShutdownOS
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] error
 *   Error occurred
 */
void Sys_ShutdownOS (StatusType error);

/**
 * @brief Architectural specific timer initialization
 *
 * This routine is responsible for initializing the
 * hardware timer used for generating system heart
 * beat (in unit of tick).
 */
void ArchTimerInit (void);

/**
 * @brief Internal service implementation for IncrementCounter
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] CounterID
 *   Counter to be updated
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid or counter is implemented
 *   in hardware and can not be incremented by software
 */
StatusType Sys_IncrementCounter (CounterType CounterID);

/**
 * @brief
 * Internal service implementation for IncrementCounter with
 * preemption
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] CounterID
 *   Counter to be updated
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid or counter is implemented
 *   in hardware and can not be incremented by software
 */
StatusType Sys_IncrementCounter_Preempt (CounterType CounterID);

/**
 * @brief Internal service implementation for GetCounterValue
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] CounterID
 *   The counter whose tick value should be read
 * @param[out] Value
 *   Contains the current tick value of the counter
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid
 */
StatusType Sys_GetCounterValue (CounterType CounterID,
                                TickRefType Value);

/**
 * @brief Internal service implementation for GetElapsedValue
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] CounterID
 *   The counter to be read
 * @param[in,out] Value
 *   The previously read tick value of the counter
 *   The current tick value of the counter
 * @param[out]
 *   The difference to the previous read value
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid
 * @retval E_OS_VALUE
 *   (Extended) The given Value was not valid
 */
StatusType Sys_GetElapsedValue (CounterType CounterID,
                                TickRefType Value,
                                TickRefType ElapsedValue);
/**
 * @brief Callback of system timer interrupt
 *
 * TickHandler manages system tick and triggers alarms when
 * necessary. Every time TickHandler gets called, the global
 * system tick will be incremented by 1.
 */
void TickHandler (void);

/**
 * @brief Process running schedule tables of a counter
 *
 * CheckScheduleTables iterates through all the running
 * schedule tables of a counter. Expiry points of running
 * schedule tables will be processed according to current
 * counter count value. Schedule table status will also be
 * updated.
 *
 * @param[in] counter
 *   Reference to a counter
 */
void CheckScheduleTables (Counter * counter);

/**
 * @brief
 * Internal service implementation for StartScheduleTableRel
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 * @param[in] Offset
 *   Number of ticks on the counter before the the schedule
 *   table processing is started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_VALUE
 *   (Extended) Offset is greater than
 *   (OsCounterMaxAllowedValue - InitialOffset) or is
 *   equal to 0
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */
StatusType Sys_StartScheduleTableRel (
  ScheduleTableType ScheduleTableID,
  TickType Offset);

/**
 * @brief
 * Internal service implementation for StartScheduleTableAbs
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 * @param[in] Start
 *   Absolute counter tick value at which the schedule table
 *   is started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_VALUE
 *   (Extended) Start is greater than OsCounterMaxAllowedValue
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */

StatusType Sys_StartScheduleTableAbs (
  ScheduleTableType ScheduleTableID,
  TickType Start);

/**
 * @brief
 * Internal service implementation for StopScheduleTable
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be stopped
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_NOFUNC
 *   (Standard) Schedule table was already stopped
 */
StatusType Sys_StopScheduleTable (
  ScheduleTableType ScheduleTableID);

/**
 * @brief
 * Internal service implementation for NextScheduleTable
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID_From
 *   Currently processed schedule table
 * @param[in] ScheduleTableID_To
 *   Schedule table that provides its series of expiry points
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID_From or ScheduleTableID_To
 *   not valid
 * @retval E_OS_NOFUNC
 *   (Standard) ScheduleTableID_From not started
 * @retval E_OS_STATE
 *   (Standard) ScheduleTableID_To is started or next
 */
StatusType Sys_NextScheduleTable (
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To);

#ifdef SCHEDTBL_SYNC
/**
 * @brief
 * Internal service implementation for StartScheduleTableSynchron
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */
StatusType Sys_StartScheduleTableSynchron (
  ScheduleTableType ScheduleTableID);

/**
 * @brief
 * Internal service implementation for SyncScheduleTable
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be synchronized
 * @param[in] Value
 *   The current value of the synchronization counter
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) The ScheduleTableID was not valid or
 *   schedule table can not be synchronized
 *   (OsScheduleTblSyncStrategy not set or
 *   OsScheduleTblSyncStrategy = IMPLICIT)
 * @retval E_OS_VALUE
 *   (Extended) The <Value> is out of range
 * @retval E_OS_STATE
 *   (Standard) The state of schedule table <ScheduleTableID>
 *   is equal to SCHEDULETABLE_STOPPED
 */
StatusType Sys_SyncScheduleTable (
  ScheduleTableType ScheduleTableID,
  TickType Value);

/**
 * @brief
 * Internal service implementation for SetScheduletableAsync
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table whose sychonization is to be stopped
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 */
StatusType Sys_SetScheduleTableAsync (
  ScheduleTableType ScheduleTableID);
#endif

/**
 * @brief
 * Internal service implementation for GetScheduleTableStatus
 *
 * Should only be called in kernel context. See syscall.c
 * for actual service interface.
 *
 * @param[in] ScheduleTableID
 *   Schedule table for which status is requested
 * @param[out] ScheduleStatus
 *   Reference to ScheduleTableStatusType
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 */
StatusType Sys_GetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus);

/**
 * @brief Schedule Table Service Initialization.
 */
void ScheduleTableInit (void);

/**
 * @brief Board specific initialization routine
 */
void BoardInit (void);

/**
 * @brief Driver initialization routine
 */
void DriversInit (void);

/**
 * @brief Busy loop delay
 *
 * Loop based delay for drivers. This routine can be used
 * when interrupt is not enabled (Unlike Delay).
 *
 * @param[in] ms
 *   Delay interval in milliseconds
 */
void LoopDelay (uint16_t ms);

#endif

/* vi: set et ai sw=2 sts=2: */
