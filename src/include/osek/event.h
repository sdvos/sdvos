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
 * @file   src/include/osek/event.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Event Control
 */
#ifndef _OSEK_EVENT_H_
#define _OSEK_EVENT_H_

#include <osek/types.h>
#include <osek/error.h>

/**
 * @brief
 * DeclareEvent serves as an external declaration of an
 * event.
 *
 * The function and use of this service are similar to that
 * of the external declaration of variables.
 * 
 * Conformance:
 *   ECC1, ECC2
 *
 * @param[in] EventIdentifier
 *   Event identifier (C-identifier)
 */
#define DeclareEvent(EventIdentifier)

/**
 * @brief
 * The service may be called from an interrupt service
 * routine and from the task level, but not from hook
 * routines.
 *
 * The events of task <tid> are set according to the event
 * mask <mask>. Calling SetEvent causes the task <tid> to be
 * transferred to the ready state, if it was waiting for at
 * least one of the events specified in <mask>.
 *
 * Any events not set in the event mask remain unchanged.
 *
 * Conformance:
 *   ECC1, ECC2
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
StatusType SetEvent (TaskType tid, EventMaskType mask);

/**
 * @brief
 * The events of the extended task calling ClearEvent are
 * cleared according to the event mask <mask>.
 *
 * The system service ClearEvent is restricted to extended
 * tasks which own the event.
 *
 * Conformance:
 *   ECC1, ECC2
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
StatusType ClearEvent (EventMaskType mask);

/**
 * @brief
 * This service returns the current state of all event bits
 * of the task <tid>, not the events that the task is
 * waiting for.
 *
 * The service may be called from interrupt service routines,
 * task level and some hook routines.
 *
 * The current status of the event mask of task <tid> is
 * copied to <event>.
 * 
 * The referenced task shall be an extended task.
 *
 * Conformance:
 *   ECC1, ECC2
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
StatusType GetEvent (TaskType tid, EventMaskRefType event);

/**
 * @brief
 * The state of the calling task is set to waiting, unless
 * at least one of the events specified in <mask> has
 * already been set.
 *
 * This call enforces rescheduling, if the wait condition
 * occurs. If rescheduling takes place, the internal
 * resource of the task is released while the task is in
 * the waiting state.
 *
 * This service shall only be called from the extended task
 * owning the event.
 *
 * Conformance:
 *   ECC1, ECC2
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
StatusType WaitEvent (EventMaskType mask);

#endif
/* vi: set et ai sw=2 sts=2: */
