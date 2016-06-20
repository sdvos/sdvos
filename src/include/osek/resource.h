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
 * @file   src/include/osek/resource.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Resource Management
 */
#ifndef _OSEK_RESOURCE_H_
#define _OSEK_RESOURCE_H_

#include <osek/types.h>
#include <osek/error.h>

/**
 * @brief
 * DeclareResource serves as an external declaration of a
 * resource. The function and use of this service are
 * similar to that of the external declaration of variables.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] ResourceIdentifier
 *   Resource identifier (C-identifier)
 */
#define DeclareResource(ResourceIdentifier)

/**
 * @brief
 * This call serves to enter critical sections in the code
 * that are assigned to the resource referenced by rid. A
 * critical section shall always be left using
 * ReleaseResource.
 *
 * The OSEK priority ceiling protocol for resource
 * management is described in chapter 8.5.
 *
 * Nested resource occupation is only allowed if the inner
 * critical sections are completely executed within the
 * surrounding critical section (strictly stacked, see
 * chapter 8.2, Restrictions when using resources). Nested
 * occupation of one and the same resource is also
 * forbidden! It is recommended that corresponding calls
 * to GetResource and ReleaseResource appear within the
 * same function. It is not allowed to use services which
 * are points of rescheduling for non preemptable tasks
 * (TerminateTask, ChainTask, Schedule and WaitEvent, see
 * chapter 4.6.2) in critical sections. Additionally,
 * critical sections are to be left before completion of
 * an interrupt service routine. Generally speaking,
 * critical sections should be short. The service may be
 * called from an ISR and from task level.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType GetResource (ResourceType rid);

/**
 * @brief
 * ReleaseResource is the counterpart of GetResource and
 * serves to leave critical sections in the code that are
 * assigned to the resource referenced by rid.
 *
 * For information on nesting conditions, see particularities
 * of GetResource. The service may be called from an ISR and
 * from task level.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType ReleaseResource (ResourceType rid);

#endif
/* vi: set et ai sw=2 sts=2: */
