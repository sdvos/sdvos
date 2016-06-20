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
 * @file   src/include/osek/task.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Task Management
 */
#ifndef _OSEK_TASK_H_
#define _OSEK_TASK_H_

#include <osek/types.h>
#include <osek/error.h>

/** Constant of data type TaskStateType for task state Running */
#define RUNNING         0x0U
/** Constant of data type TaskStateType for task state Waiting */
#define WAITING         0x1U
/** Constant of data type TaskStateType for task state Ready */
#define READY           0x2U
/** Constant of data type TaskStateType for task state Suspended */
#define SUSPENDED       0x3U

/** Constant of data type TaskType for a not defined task */
#define INVALID_TASK    ((TaskType) ~(0UL))

/** Constant of data type PrioType for an invalid priority */
#define INVALID_PRIO    ((PrioType) ~(0UL))

/**
 * @brief
 * DeclareTask serves as asn external declaration of a task.
 * The function and use of this service are similar to that
 * of the external declaration of variables.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] TaskIdentifier
 *   Task identifier (C-identifier)
 */
#define DeclareTask(TaskIdentifier)

/**
 * @def TASK(TaskName)
 * @brief Defines an OSEK task
 *
 * With the macro TASK the user may use the same name for
 * "task identification" and "name of task function". The
 * task identification will be generated from the TaskName
 * by prefixing an "Func" to it.
 *
 * @param[in] TaskName
 *   Name of the task function
 */
#define TASK(TaskName) StatusType Func##TaskName (void)

/**
 * @brief
 * The task <TaskID> is transferred from the suspended state
 * into the ready state. The OS ensures that the task code
 * is being executed from the first statement.
 *
 * In case of multiple activation requests, if the task is not
 * suspended, the activation will only be recorded and performed
 * later.
 *
 * The service may be called from interrupt level and task
 * level. If E_OS_LIMIT is returned the activation is
 * ignored. When an extended task is transferred from
 * suspended state into ready state all its events are
 * cleared.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType ActivateTask (TaskType tid);

/**
 * @brief
 * This service causes the termination of the calling task.
 * The calling task is transferred from the running state
 * into the suspended state.
 *
 * In case of multiple activation
 * requests, terminating the current instance of the task
 * automatically puts the next instance of the same task
 * into the ready state.
 *
 * An internal resource assigned to the calling task is
 * automatically released. Other resources occupied by the
 * task shall have been released before the call to this
 * function. If a resource is still occupied in standard
 * status the behaviour is undefined.
 *
 * If the call was successful, TerminateTask does not return
 * to the call level and the status can not be evaluated. if
 * the version with extended status is used, the service
 * returns in case of error, and provides a status which can
 * be evaluated in the application.
 *
 * If this service is called successfuly, it enforces a
 * rescheduling.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @retval None
 *   (Standard) Nothing
 * @retval E_OS_RESOURCE
 *   (Extended) Task still occupies resource
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType TerminateTask (void);

/**
 * @brief
 * This service causes the termination of the calling task.
 * After termination of the calling task a succeeding task
 * is activated. Using this service, it ensures that the
 * succeeding task starts to run at the earliest after the
 * calling task has been terminated.
 *
 * If the succeeding task is identical with the current
 * task, this does not result in multiple requests. The task
 * is not transferred to the suspended state, but will
 * immediately become ready again.
 *
 * An internal resource assigned to the calling task is
 * automatically released, even if the succeeding task is
 * identical to the current task. Other resources occupied
 * by the calling task shall have been released before
 * ChainTask is called. If a resource is still occupied
 * in standard status the behaviour is undefined.
 *
 * If called successfully, ChainTask does not return to the
 * call level and the status can not be evaluated. In case
 * of error, it returns to the calling task and provides a
 * status which can be evaluated in application.
 *
 * If the service is called successfully, it enforces a
 * rescheduling. If E_OS_LIMIT is returned, the activation
 * is ignored. When an extended task is transferred from
 * suspended state into ready state all its events are
 * cleared.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType ChainTask (TaskType tid);

/**
 * @brief
 * If a higher-priority task is ready, the internal resource
 * of the task is released, the current task is put into the
 * ready state, its context is saved and the higher-priority
 * task is executed. Otherwise the calling task is continued.
 *
 * Rescheduling only takes place if an internal resource is
 * assigned to the calling task during system generation.
 * For these tasks, Schedule enables a processor assignment
 * to other tasks with lower or equal priority than the
 * ceiling priority of the internal resource and higher
 * priority than the priority of the calling task in
 * application-specific locations. When returning from
 * Schedule, the internal resource has been taken again.
 * This service has no influence on tasks with no internal
 * resource assigned (preemptable tasks).
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_RESOURCE
 *   (Extended) Calling task still occupies resources
 * @retval E_OS_CALLEVEL
 *   (Extended) Call at interrupt level
 */
StatusType Schedule (void);

/**
 * @brief
 * GetTaskID returns the information about the TaskID of
 * the task which is currently running.
 *
 * Allowed on task level, ISR level and in several hook
 * routines. This service is intended to be used by library
 * functions and hook routines. If TaskID cannot be
 * evaluated, the service returns INVALID_TASK as TaskType.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] tid_ref
 *   Reference to the task which is currently running
 *
 * @retval E_OK
 *   No error
 */
StatusType GetTaskID (TaskRefType tid_ref);

/**
 * @brief
 * Returns the state of a task at the time of calling
 * GetTaskState.
 *
 * The service may be called from interrupt service
 * routines, task level, and some hook routines. When a
 * call is made from a task in a full preemptive system,
 * the result may already be incorrect at the time of
 * evaluation. When the service is called for a task, which
 * is activated more than once, the state is set to running
 * if any instance of the task is running.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType GetTaskState (TaskType tid, TaskStateRefType state_ref);

#endif

/* vi: set et ai sw=2 sts=2: */
