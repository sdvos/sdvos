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
 * @file   src/include/task.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Task Management Data Structure
 */
#ifndef _TASK_H_
#define _TASK_H_

/** Flag for task preemption context */
#define TASK_PREEMPT_CTX    (0x1 << 0)
/** Flag for preemptable task */
#define TASK_PREEMPTABLE    (0x1 << 1)
/** Flag for extended task */
#define TASK_EXTENDED       (0x1 << 2)
/** Flag for privileged task */
#define TASK_PRIVILEGED     (0x1 << 3)

#ifndef __ASSEMBLER__

#include <osek/types.h>
/* Auto generated configuration header */
#include <config/config.h>
#include <assert.h>
#include <cc.h>

/**
 * @brief Task Control Block
 *
 * TCB for SDVOS tasks. Fields may vary based on different
 * configurations.
 */
typedef struct task_struct {
  data_addr_t sp;              /**< Task stack pointer */
  data_addr_t bp;              /**< Task stack base pointer */
  task_context context;        /**< Task processor context */
  FlagType flag;               /**< Task property flag */
  TaskType tid;                /**< Task ID */
  PrioType priority;           /**< Task Priority */
  PrioType orig_prio;          /**< Task Original Priority */
  PrioType ires;               /**< Task Internal Resource */
  Resource * res;              /**< Task Resources */
  TaskStateType state;         /**< Task State */
  EventMaskType cevent;        /**< Current status of event */
  EventMaskType wevent;        /**< Events task is waiting for */
  data_addr_t sp_end;          /**< Task stack pointer end */
  code_addr_t start;           /**< Task entry point */
#ifdef MULTI_ACTIVATION
  ActivationNumType act;       /**< Current number of activations */
  ActivationNumType max_act;   /**< Max activation */
#endif
#ifdef MULTI_TASK_PER_PRIO
  struct task_struct * next;   /**< Next Task */
#endif
} ALIGNED(sizeof(udata_word_t)) TCB;

/**
 * @def TCB_KEY_FIELDS_PACKED()
 * @brief Check whether the key fields of PCB are packed
 *
 * Key fields in TCB are sp, bp, context and flag. These
 * fields could be accessed in assembly code and assumed
 * contiguous without implicit padding.
 *
 * @retval TRUE
 *   Key fields are packed
 * @retval FALSE
 *   Key fields are not packed
 */
#define TCB_KEY_FIELDS_PACKED()                        \
  (((data_addr_t) (&(((TCB *)0)->flag))) ==            \
   (sizeof (data_addr_t) * 2 + sizeof (task_context)))

/*
 * Open the following macro to check whether TCB is packed.
 * Unaligned memory access is not supported on some architectures
 * and it is usually slower. If possible, we should not pack TCB.
 * However, making the FIRST 4 FIELDS (sp, bp, context and flag)
 * packed is often *NECESSARY* (depending on architecture)!
 */
#ifdef PACK_TCB
CASSERT (sizeof (TCB) ==
         ((sizeof (data_addr_t) * 2 + sizeof (TaskType) +
         sizeof (PrioType) * 2 + sizeof (PrioType) +
         sizeof (TaskStateType) + sizeof (ResourceType *) +
         sizeof (EventMaskType) * 2 + sizeof (FlagType) +
         sizeof (code_addr_t) +
#ifdef MULTI_ACTIVATION
         sizeof (ActivationNumType) * 2 +
#endif
#ifdef MULTI_TASK_PER_PRIO
         sizeof (struct task_struct *) +
#endif
         sizeof (task_context) +
         sizeof (udata_word_t) - 1) &
         (~(sizeof (udata_word_t) - 1))), TCB);
#endif

/**
 * @brief Priority Queue
 *
 * Priority Queue is essentially the ready queue. It is
 * an array of link lists if multiple task per priority is
 * allowed. The index of the array is the numeric value of
 * task priority. In each link list, TCBs are ordered based
 * on their arrival time. The head of the list will be the
 * oldest ready task.
 *
 * In case multiple task per priority is not configured,
 * priority queue becomes an array of TCBs.
 */
#ifdef MULTI_TASK_PER_PRIO
typedef struct prio_queue {
  TCB * head;
  TCB * tail;
} prio_queue_t;
#else
typedef TCB * prio_queue_t;
#endif

/** Array of all application tasks in system */
extern TCB tasks[];
/** Auto start tasks for each application mode */
extern TaskType auto_tasks[][NUM_TASKS];
/** Priority (Ready) Queue */
extern prio_queue_t prio_queue[];
/** Current running task */
extern TCB * cur_task;

/**
 * @brief SDVOS idle task
 *
 * This is idle task. IdleTask calls IdleLoop which is
 * architectural dependant. Ideally, this loop will put the
 * processor into certain mode that is power efficient.
 */
extern void IdleTask (void);

#endif

#endif

/* vi: set et ai sw=2 sts=2: */
