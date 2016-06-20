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
 * @file   src/task.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Task Management
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <config/config.h>

void
IdleTask (void)
{
  extern void IdleLoop (void);
  IdleLoop ();
}

void
EnqueueTaskTail (TaskType tid)
{
  prio_queue_t * queue = NULL;
  queue = &prio_queue[tasks[tid].priority];

#ifdef MULTI_TASK_PER_PRIO
  tasks[tid].next = NULL;
  if (!queue->head) {
    /* Priority queue is empty */
    queue->head = queue->tail = &tasks[tid];
  } else {
    /* Add task to bottom of priority queue */
    queue->tail->next = &tasks[tid];
    queue->tail = &tasks[tid];
  }
#else
  *queue = &tasks[tid];
#endif
}

void
EnqueueTaskHead (TaskType tid)
{
  prio_queue_t * queue = NULL;
  queue = &prio_queue[tasks[tid].priority];

#ifdef MULTI_TASK_PER_PRIO
  tasks[tid].next = queue->head;
  if (!queue->head) {
    /* Priority queue is empty */
    queue->head = queue->tail = &tasks[tid];
  } else {
    /* Add task to top of priority queue */
    queue->head = &tasks[tid];
  }
#else
  *queue = &tasks[tid];
#endif
}

void
MakeRunning (TCB * task)
{
  task->state = RUNNING;
  /* Always clear preemption context flag for running task */
  task->flag &= (~((FlagType) TASK_PREEMPT_CTX));
  /* Need to handle internal resource */
  if (task->ires != INVALID_PRIO) {
    /* Task has internal resource */
    if (task->res == NULL) {
      task->priority = task->ires;
    }
  }
}

/*
 * Return the next highest priority ready task in the
 * priority queue and remove it from the queue.
 */
void
NextTask (TaskRefType tid_ref, PrioType max, PrioType min)
{
  int i = 0;
  prio_queue_t * queue = NULL;
  *tid_ref = INVALID_TASK;

  /* Find the next highest priority ready task */
  for (i = max; i >= min; i--) {
    queue = &prio_queue[i];
    /* If task is found, remove it from the queue */
#ifdef MULTI_TASK_PER_PRIO
    if (queue->head) {
      *tid_ref = queue->head->tid;
      queue->head = queue->head->next;
      /* Is queue empty? */
      if (!(queue->head)) {
        queue->tail = NULL;
      }
      break;
    }
#else
    if (*queue) {
      *tid_ref = (*queue)->tid;
      *queue = NULL;
      break;
    }
#endif
  }
}

/**
 * @brief Internal task activation.
 *
 * Internal use only. Called by ChainTask and ActivateTask.
 *
 * @param[in] tid
 *   Task to be activated.
 */
static inline void
DoActivateTask (TaskType tid)
{
#ifdef MULTI_ACTIVATION
  /* Increase activation count */
  tasks[tid].act++;
  /* If task is SUSPENDED, do actual activation */
  if (tasks[tid].state == SUSPENDED) {
#endif
    /* Normal activation */
    InitTask (&tasks[tid]);
    tasks[tid].state = READY;
    EnqueueTaskTail (tid);
#ifdef MULTI_ACTIVATION
  }
#endif
}

/**
 * @brief Internal task termination.
 *
 * Internal use only. Called by ChainTask and TerminateTask.
 */
static inline void
DoTerminateTask (void)
{
  /*
   * PostTaskHook is called after executing the current
   * task, but before leaving the task's running state.
   */
  POSTTASKHOOK ();

  /*
   * Restore task priority. Necessary when task has an
   * internal resource. Internal resource should be
   * released in TerminateTask, ChainTask, Schedule and
   * WaitEvent.
   */
  cur_task->priority = cur_task->orig_prio;

#ifdef MULTI_ACTIVATION
  if ((--(cur_task->act)) > 0) {
    /* Has pending activation */
    InitTask (cur_task);
    cur_task->state = READY;
    EnqueueTaskTail (cur_task->tid);
  } else {
    cur_task->state = SUSPENDED;
  }
#else
  cur_task->state = SUSPENDED;
#endif

  /* Re-schedule */
  JumpNext ();
}

/* Task state initialization */
void
InitTask (TCB * task)
{
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
  if (task->priority != task->orig_prio) panic ();
  if (task->res) panic ();
#endif

  /* Reset stack */
  task->sp = task->bp;
  /* Clear events */
  task->wevent = (EventMaskType) 0;
  task->cevent = (EventMaskType) 0;
  /* Architectural dependent context initialization */
  InitContext (task);
}

/*
 * CheckPreemption has to be called from ISR2 or scheduler
 * level. cur_task must be valid. If current task is
 * preemptable and there is higher priority ready task,
 * preemption will occur. Otherwise this function does
 * nothing and returns.
 */
void
CheckPreemption (FlagType flag)
{
  TaskType tid = INVALID_TASK;

#ifdef DEBUG_SDVOS
  if (!cur_task) panic ();
#endif

  /*
   * Non preemptive tasks has an internal resource with
   * highest ceiling priority. CheckPreemption will not
   * affect non preemptive tasks because NextTask will not
   * be able to find a valid tid.
   */

  /* Find ready task with higher priority than current task */
  NextTask (&tid, MAX_PRIO, cur_task->priority + 1);

  /* If tid is valid, preempting task exists */
  if (tid != INVALID_TASK) {
    /*
     * PostTaskHook is called after executing the current
     * task, but before leaving the task's running state.
     */
    POSTTASKHOOK ();
    cur_task->state = READY;
    /* Put preempted task back to priority queue */
    /* A preempted task is considered to be the first task
     * in its priority queue. */
    EnqueueTaskHead (cur_task->tid);
    if (flag == PREEMPT_ISR) {
      /* Set task preemption context flag (ISR) */
      cur_task->flag |= TASK_PREEMPT_CTX;
    } else if (flag == PREEMPT_SCHED) {
      /* Clear task preemption context flag (SCHED) */
      cur_task->flag &= (~((FlagType) TASK_PREEMPT_CTX));
    } else {
      /* Should not be called in other processing levels */
      panic ();
    }

    /*
     * No matter whether we are preempted from scheduler level
     * or ISR level, DISPATCH_BLOCK will be used. In most cases,
     * context would probably already have been saved if
     * preempted from ISR level. However, this is not always
     * true (e.g. FPU states). As a result, we will let
     * SwitchTask decide what to do based on the preemption
     * context flag in TCB.
     */
    Dispatch (tid, DISPATCH_BLOCK);
  }

  return;
}

void
JumpNext ()
{
  TaskType tid = 0;
  NextTask (&tid, MAX_PRIO, 0);
  Dispatch (tid, DISPATCH_DISCARD);
}

void
Dispatch (TaskType tid, FlagType flag)
{
  TCB * src_task = NULL;

  if (tid == INVALID_TASK) {
    /* TODO: Idle task? */
    panic ();
  }

  switch (flag) {
    case DISPATCH_DISCARD :
      /* Handling task termination */
      cur_task = &tasks[tid];
      src_task = NULL;
      break;
    case DISPATCH_BLOCK :
      /* Handling Schedule */
      src_task = cur_task;
      cur_task = &tasks[tid];
      break;
  }

  MakeRunning (cur_task);
  /*
   * PreTaskHook is called before executing a task, but
   * after the transition of the task to the running state.
   */
  PRETASKHOOK ();
  SwitchTask (src_task, &tasks[tid]);
}

StatusType
Sys_ActivateTask (TaskType tid)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid valid? */
  ValidateTaskID (tid);
#endif

#ifdef MULTI_ACTIVATION
  /*
   * If multiple activation is allowed, check whether the
   * max activation count is reached.
   */
  if (tasks[tid].act >= tasks[tid].max_act) {
    DEBUG_PRINTFV ("Too many activations Task %d\n", tid);
    ret = E_OS_LIMIT;
    goto std_ret;
  }
#else
  /* Otherwise, check whether target task is suspended */
  if (tasks[tid].state != SUSPENDED) {
    DEBUG_PRINTFV ("Task not suspended\n");
    ret = E_OS_LIMIT;
    goto std_ret;
  }
#endif

  DoActivateTask (tid);

std_ret:
  SaveOSErrorService (OSServiceId_ActivateTask, tid, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_ActivateTask_Preempt (TaskType tid)
{
  StatusType ret = E_OK;
  ret = Sys_ActivateTask (tid);
  if (ret == E_OK) CheckPreemption (PREEMPT_SCHED);
  return ret;
}

StatusType
Sys_TerminateTask (void)
{
  StatusType ret = E_OK;

#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
  if (!cur_task) {
    panic ();
  }

  if (cur_task->state != RUNNING) {
    panic ();
  }
#endif

#ifdef OSEK_EXTENDED
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

  DoTerminateTask ();

  /* Should never reach here unless error occurred */
#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_TerminateTask, 0, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_ChainTask (TaskType tid)
{
  StatusType ret = E_OK;

#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
  if (!cur_task) {
    panic ();
  }

  if (cur_task->state != RUNNING) {
    panic ();
  }
#endif

#ifdef OSEK_EXTENDED
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
  /* Is tid valid? */
  ValidateTaskID (tid);
#endif

#ifdef MULTI_ACTIVATION
  /*
   * If multiple activation is allowed, check whether the
   * max activation count is reached.
   */
  if (tasks[tid].act >= tasks[tid].max_act) {
    DEBUG_PRINTFV ("Too many activations Task %d\n", tid);
    ret = E_OS_LIMIT;
    goto std_ret;
  }
#else
  /* Otherwise, check whether target task is suspended */
  if (tasks[tid].state != SUSPENDED) {
    DEBUG_PRINTFV ("Task not suspended\n");
    ret = E_OS_LIMIT;
    goto std_ret;
  }
#endif

  if (tid == cur_task->tid) {
    /*
     * PostTaskHook is called after executing the current
     * task, but before leaving the task's running state.
     */
    POSTTASKHOOK ();
    /* Restore task priority */
    cur_task->priority = cur_task->orig_prio;
    /* Special case: ChainTask self */
    InitTask (cur_task);
    cur_task->state = READY;
    EnqueueTaskTail (cur_task->tid);
    /* Re-schedule */
    JumpNext ();
  } else {
    /* Common case: Activate and Terminate */
    DoActivateTask (tid);
    DoTerminateTask ();
  }

std_ret:
  SaveOSErrorService (OSServiceId_ChainTask, tid, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_Schedule (void)
{
  TaskType tid = INVALID_TASK;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
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

  /*
   * Schedule has no influence on tasks with no internal
   * resource assigned.
   */
  if (cur_task->ires == INVALID_PRIO) {
    ret = E_OK;
    goto std_ret;
  }

  /*
   * The specification for Schedule in OSEK 2.2.3 is almost
   * encrypted... My understanding is this: When a task
   * calls Schedule, it allows another task in the SAME
   * GROUP (which will not have a priority higher than the
   * ceiling priority of the internal resource of the
   * calling task) to be scheduled if it has a higher
   * priority than the original priority of the calling
   * task. This interpretation seems to match both the
   * description in specification and the intention of this
   * service.
   *
   * TODO: What if tasks outside the calling task's group
   * fall into [cprio, orig_prio)?
   */
  NextTask (&tid, cur_task->ires, cur_task->orig_prio + 1);

  if (tid != INVALID_TASK) {
    /* Found eligible task, dispatch */
    /*
     * PostTaskHook is called after executing the current
     * task, but before leaving the task's running state.
     */
    POSTTASKHOOK ();
    cur_task->state = READY;
    /* Release internal resource */
    cur_task->priority = cur_task->orig_prio;
    /* Put calling task back to priority queue */
    EnqueueTaskTail (cur_task->tid);
    Dispatch (tid, DISPATCH_BLOCK);
  }

std_ret:
  SaveOSErrorService (OSServiceId_Schedule, 0, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_GetTaskID (TaskRefType tid_ref)
{
  if (!cur_task) {
    *tid_ref = INVALID_TASK;
  } else {
    *tid_ref = cur_task->tid;
  }
  return E_OK;
}

StatusType
Sys_GetTaskState (TaskType tid, TaskStateRefType state_ref)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is tid valid? */
  ValidateTaskID (tid);
#endif

  *state_ref = tasks[tid].state;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetTaskState, tid, state_ref, 0);
  ERRORHOOK (ret);
  return ret;
}

/* vi: set et ai sw=2 sts=2: */
