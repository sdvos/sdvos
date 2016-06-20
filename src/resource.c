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
 * @file   src/resource.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Resource Management
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <config/config.h>

/** Global queue for all resources in system */
extern Resource resources[NUM_RESOURCES];

/**
 * @brief Pushes a resource to the top of current task's
 * resource stack.
 *
 * PushResource is used internally by GetResource only.
 * It is used to abstract resource stack data structure
 * and does not do any sanity check such as resource
 * validity or occupation status.
 *
 * @param[in] res
 *   Reference to resource object
 */
static void
PushResource (Resource * res)
{
#ifdef DEBUG_SDVOS
  if (!cur_task) return;
#endif
  res->next = cur_task->res;
  cur_task->res = res;
}

/**
 * @brief Pops the resource at the top of current task's
 * resource stack.
 *
 * PopResource is used internally by ReleaseResource only.
 * It is used to abstract resource stack data structure and
 * does not do any sanity check such as resource validity
 * or occupation status.
 *
 * @return
 *   Reference to the resource popped
 */
static Resource *
PopResource ()
{
  Resource * res = NULL;
#ifdef DEBUG_SDVOS
  if (!cur_task) return NULL;
#endif
  res = cur_task->res;
  cur_task->res = res->next;
  res->next = NULL;
  return res;
}

#ifdef OSEK_EXTENDED
/**
 * @brief Helper function to check the validity of resource.
 *
 * @param[in] rid
 *   Reference to resource
 * @retval TRUE
 *   Resource is valid
 * @retval FALSE
 *   Resource is invalid
 */
static bool
IsResourceValid (ResourceType rid)
{
  if (rid >= NUM_RESOURCES) return FALSE;
  return TRUE;
}
#endif

StatusType
Sys_GetResource (ResourceType rid)
{
  StatusType ret = E_OK;
  Resource * res = NULL;

#ifdef OSEK_EXTENDED
  if (!IsResourceValid (rid)) {
    ret = E_OS_ID;
    goto std_ret;
  }
  res = &resources[rid];
  if (res->occupied) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  if (cur_task->orig_prio > res->cprio) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Set occupation flag of resource for current task */
  res->occupied = TRUE;
#else
  res = &resources[rid];
#endif
  PushResource (res);
  /* Change task's priority to resource ceiling priority */
  cur_task->priority = res->cprio;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetResource, rid, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_ReleaseResource (ResourceType rid)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  Resource * res = NULL;
  if (!IsResourceValid (rid)) {
    ret = E_OS_ID;
    goto std_ret;
  }
  res = &resources[rid];
  if (!res->occupied) {
    ret = E_OS_NOFUNC;
    goto std_ret;
  }
  /* Is res on top of current task's resource stack? */
  if (res != cur_task->res) {
    ret = E_OS_NOFUNC;
    goto std_ret;
  }
  if (cur_task->orig_prio > res->cprio) {
    ret = E_OS_ACCESS;
    goto std_ret;
  }
  /* Clear occupation flag of resource for current task */
  res->occupied = FALSE;
#endif
  PopResource ();
  if (cur_task->res) {
    /*
     * Nested resource occupation. Restore to the ceiling
     * priority of previously occupied resource.
     */
    cur_task->priority = cur_task->res->cprio;
  } else if (cur_task->ires != INVALID_PRIO) {
    /*
     * No nested resource occupation, but task has internal
     * resource. Restore task priority to internal resource
     * ceiling priority.
     */
    cur_task->priority = cur_task->ires;
  } else {
    /*
     * No nested resource occupation nor internal resource.
     * Restore task priority to original statically
     * assigned priority.
     */
    cur_task->priority = cur_task->orig_prio;
  }

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_ReleaseResource, rid, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_ReleaseResource_Preempt (ResourceType rid)
{
  StatusType ret = E_OK;
  ret = Sys_ReleaseResource (rid);
  if (ret == E_OK) CheckPreemption (PREEMPT_SCHED);
  return ret;
}

/* vi: set et ai sw=2 sts=2: */
