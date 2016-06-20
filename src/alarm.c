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
 * @file   src/alarm.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Alarms
 */
#include <sdvos.h>
#include <osek/osek.h>
#include <config/config.h>

/** Global queue for all counters in the system */
extern Counter counters[NUM_COUNTERS];
/** Global queue for all alarms in the system */
extern AlarmQueueType alarms[NUM_ALARMS];

/**
 * @brief Setting alarm parameters
 *
 * This function sets alarm specific parameters (OVF bit,
 * expiration, cycle) based on input provided. If exp is
 * smaller than (or equal to) current counter count, it
 * will be treated as an overflow. This is consistent with
 * SetAbs/RelAlarm specification.
 *
 * @param[in] alarm
 *   Reference to alarm
 * @param[in] exp
 *   Alarm expiration time in ticks
 * @param[in] cycle
 *   Cycle value in case of cyclic alarm
 */
static void
SetAlarm (AlarmType alarm, TickType exp, TickType cycle)
{
  Counter * counter = alarms[alarm].counter;

  if (exp <= counter->count) {
    /* exp is an overflow of the counter count */
    ToggleAlarmOVF (alarm);
  } else {
    /* exp does not overflow */
    SetAlarmOVF (alarm);
  }

  alarms[alarm].cycle = cycle;
  alarms[alarm].exp = exp;
}

/**
 * @brief Activate an alarm
 *
 * ActivateAlarm inserts the specified alarm into its
 * counter's alarm queue and sets the status of the alarm
 * to "active". If the given alarm is already active,
 * ActivateAlarm will do nothing and return.
 *
 * Alarms in counter alarm queue are sorted by their
 * expiration time with the earliest being the head of the
 * queue. An overflow bit in each alarm and counter is used
 * to simplify the queue management.
 *
 * @param[in] alarm
 *   Reference to an alarm
 */
static void
ActivateAlarm (AlarmType alarm)
{
  AlarmQueueType * queue = alarms[alarm].counter->alarms;
  AlarmQueueType * alm = &alarms[alarm];
  AlarmQueueType * last = NULL;
  Counter * counter = alm->counter;

  if (!queue) {
    alm->counter->alarms = alm;
    alm->next = alm->prev = NULL;
    goto finish;
  }

  /*
   * Now, insert the alarm into alarm queue. The alarm queue
   * is ordered by absolute expiration time. Here we assume
   * that *NO* alarm with counter UNDERFLOW will exist in
   * the queue at the time of ActivateAlarm. Overflow is
   * allowed.
   *
   * This assumption is correct because whenever a counter
   * gets updated through IncrementCounter, all the alarms
   * with an underflow will be triggered and removed from
   * the queue. So, only CheckAlarms will encounter the
   * rare case of having an alarm underflow.
   */
  while (queue) {
    if (((queue->status ^ alm->status) & ALARM_STATUS_OVF) &&
        (CounterOVF (counter) == AlarmOVF (alarm))) {
      /* The expiration time of 'queue' is an overflow. */
      /* Expiration time of alarm is not an overflow. */
      /* Alarm should be inserted in front of 'queue'. */
      break;
    } else if (!((queue->status ^ alm->status) &
               ALARM_STATUS_OVF) && (alm->exp < queue->exp)) {
      /* Alarm and 'queue' have the same OVF bit. */
      /* A comparison of expiration time is enough. */
      break;
    } else {
      if (!queue->next) last = queue;
      queue = queue->next;
    }
  }

  if (!queue) {
    /* alarm will be inserted to the tail of the queue */
    last->next = alm;
    alm->prev = last;
    alm->next = NULL;
  } else {
    /*
     * Otherwise, queue is the alarm where our alarm should
     * be put in front of.
     */
    if (queue->prev == NULL) {
      /* Need to put alarm at the head */
      queue->counter->alarms = alm;
    } else {
      queue->prev->next = alm;
    }
    alm->next = queue;
    alm->prev = queue->prev;
    queue->prev = alm;
  }

finish:
  /* Sets alarm to active */
  alm->status |= ALARM_STATUS_ACTIVE;
  return;
}

/**
 * @brief Deactivate an alarm
 *
 * RemoveAlarm does the opposite of ActivateAlarm, it
 * removes an alarm from its counter alarm queue and
 * mark it as not active.
 *
 * If the specified alarm is not active, RemoveAlarm will
 * do nothing and return.
 *
 * @param[in] alarm
 *   Reference to an alarm
 */
static void
RemoveAlarm (AlarmType alarm)
{
  AlarmQueueType * alm = &alarms[alarm];

  if (!AlarmIsActive (alarm)) return;
  if (alm->prev) {
    /* alarm is not the head of the queue */
    alm->prev->next = alm->next;
  } else {
    /* alarm is the head of the queue */
    alm->counter->alarms = alm->next;
  }
  if (alm->next) alm->next->prev = alm->prev;
  alm->next = alm->prev = NULL;
  /* Clear alarm active bit */
  alm->status &= (~((FlagType) ALARM_STATUS_ACTIVE));
}

/**
 * @brief Perform alarm action
 *
 * Do alarm action performs action specified by an alarm
 * when it expires. There are three different actions
 * allowed: Set an event, activate a task and call a
 * callback function.
 *
 * @param[in] alarm
 *   Reference to an alarm
 */
static void
DoAlarmAction (AlarmType alarm)
{
  AlarmActionType * action = &(alarms[alarm].action);
  void (* callback) (void);

  /* Perform alarm action based on action type */
  switch (action->type) {
    case ALARM_ACTION_SETEVENT :
      DEBUG_PRINTFV ("Alarm %d, SetEvent (%d, %d)\n",
                     action->task, action->event);
      Sys_SetEvent (action->task, action->event);
      break;
    case ALARM_ACTION_ACTIVATETASK :
      DEBUG_PRINTFV ("Alarm %d, ActivateTask (%d)\n",
                     alarm, action->task);
      Sys_ActivateTask (action->task);
      break;
    case ALARM_ACTION_CALLBACK :
      DEBUG_PRINTFV ("Alarm %d, Calling callback\n", alarm);
      callback = action->callback;
      callback ();
      break;
    default :
      DEBUG_PRINTFV ("Unknow alarm action: %d\n",
                     action->type);
      panic ();
  }
}

/**
 * @brief Increment alarm expiration time
 *
 * IncAlarm increments alarm expiration time by inc.
 * Overflow will be handled properly.
 *
 * @param[in] alarm
 *   Reference to an alarm
 * @param[in] inc
 *   Increment in ticks
 */
static void
IncAlarm (AlarmType alarm, TickType inc)
{
  Counter * counter = alarms[alarm].counter;
  TickType max = counter->properties.maxallowedvalue;

  if (inc) {
    /* Update next expiration time */
    if ((max - inc) < alarms[alarm].exp) {
      alarms[alarm].exp = inc - (max - alarms[alarm].exp) - 1;
      ToggleAlarmOVF (alarm);
    } else {
      alarms[alarm].exp += inc;
    }
  }
}

/**
 * @brief Update alarm expiration time
 *
 * UpdateAlarm updates alarm expiration time according to
 * current counter count and inc. Overflow will be handled
 * properly.
 *
 * @param[in] alarm
 *   Reference to an alarm
 * @param[in] inc
 *   Increment in ticks
 * @param[in] cycle
 *   Cycle value in case of cyclic alarm
 */
static void
UpdateAlarm (AlarmType alarm, TickType inc, TickType cycle)
{
  Counter * counter = alarms[alarm].counter;
  TickType max = counter->properties.maxallowedvalue;

  SetAlarmOVF (alarm);
  alarms[alarm].cycle = cycle;

  if (inc) {
    /* Update next expiration time */
    if ((max - inc) < counter->count) {
      alarms[alarm].exp = inc - (max - counter->count) - 1;
      ToggleAlarmOVF (alarm);
    } else {
      alarms[alarm].exp = counter->count + inc;
    }
  }
}

/**
 * @brief Trigger an alarm
 *
 * FireAlarm triggers an alarm. It performs the action
 * specified by the alarm, deactivates it, and inserts it
 * back to alarm queue if it is cyclic.
 *
 * FireAlarm does not check whether the alarm expires. The
 * caller is responsible for that.
 *
 * @param[in] alarm
 *   Reference to an alarm
 */
static void
FireAlarm (AlarmType alarm)
{
  /* Perform alarm action */
  DoAlarmAction (alarm);
  /* Remove alarm from counter queue */
  RemoveAlarm (alarm);

  /* Activate alarm again if it is cyclic */
  if (alarms[alarm].cycle) {
    IncAlarm (alarm, alarms[alarm].cycle);
    /* Put alarm back into counter alarm queue */
    ActivateAlarm (alarm);
  }
}

/**
 * @brief Trigger all expired alarms of a counter
 *
 * CheckAlarms iterates through all the alarms in a counter
 * alarm queue and trigger all the expired alarms. (Not
 * every alarm in the queue is checked because they are
 * ordered.)
 *
 * @param[in] counter
 *   Reference to a counter
 */
static void
CheckAlarms (Counter * counter)
{
  TickType max = counter->properties.maxallowedvalue;
  TickType tpb = counter->properties.ticksperbase;
  AlarmQueueType * queue = counter->alarms;

  while (queue) {
    if (CounterOVF (counter) == AlarmOVF (queue->id)) {
      if (counter->count >= queue->exp) {
        FireAlarm (queue->id);
      } else {
        break;
      }
    } else if ((max - queue->exp + counter->count + 1) < tpb) {
      /* Should this ever happen? */
      /*
       * This is a special case when an alarm was set to
       * between one counter update and this counter update
       * happens to make the counter overflow.
       */
      FireAlarm (queue->id);
    } else {
      /*
       * Otherwise, we can skip all the other alarms. Their
       * expiration should be in the future.
       */
      break;
    }
    queue = counter->alarms;
  }
}

StatusType
Sys_IncrementCounter (CounterType CounterID)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is counter valid? */
  ValidateCounter (CounterID);
#endif

  Counter * counter = &counters[CounterID];
  TickType max = counter->properties.maxallowedvalue;
  TickType tpb = counter->properties.ticksperbase;

  /* Update counter count by ticks per base */
  if ((max - tpb) < counter->count) {
    /* Counter will overflow */
    counter->count = tpb - (max - counter->count) -1;
    /* Toggle the overflow bit */
    ToggleCounterOVF (counter);
  } else {
    /* No counter overflow */
    counter->count += tpb;
  }

  /* Now, check all alarms */
  CheckAlarms (counter);

#ifdef USE_SCHEDTBL
  /* Process all running schedule tables */
  CheckScheduleTables (counter);
#endif

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_IncrementCounter,
                      CounterID, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_IncrementCounter_Preempt (CounterType CounterID)
{
  StatusType ret = E_OK;
  ret = Sys_IncrementCounter (CounterID);
  if (ret == E_OK) CheckPreemption (PREEMPT_SCHED);
  return ret;
}

StatusType
Sys_GetCounterValue (CounterType CounterID, TickRefType Value)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is counter valid? */
  ValidateCounter (CounterID);
#endif

  *Value = counters[CounterID].count;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetCounterValue,
                      CounterID, Value, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_GetElapsedValue (CounterType CounterID, TickRefType Value,
                     TickRefType ElapsedValue)
{
  StatusType ret = E_OK;
  Counter * counter = NULL;
  TickType max = 0;
  TickType cur = 0;

#ifdef OSEK_EXTENDED
  /* Is counter valid? */
  ValidateCounter (CounterID);
#endif

  counter = &counters[CounterID];
  max = counter->properties.maxallowedvalue;

#ifdef OSEK_EXTENDED
  /* Is Value valid? [SWS_Os_00391] */
  if (*Value > max) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  cur = counter->count;

  if (cur >= *Value) {
    *ElapsedValue = cur - *Value;
  } else {
    /* Counter count wrapped around */
    *ElapsedValue = max - *Value + cur + 1;
  }

  *Value = cur;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetElapsedValue,
                      CounterID, Value, ElapsedValue);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Delay (CounterType CounterID, TickType Counts)
{
  StatusType ret = E_OK;
  Counter * counter = NULL;
  TickType cur = 0, start = 0;
  TickType elapsed = 0, max = 0;

#ifdef OSEK_EXTENDED
  /* Is counter valid? */
  ValidateCounter (CounterID);
#endif

  counter = &counters[CounterID];
  max = counter->properties.maxallowedvalue;

#ifdef OSEK_EXTENDED
  /* Is Counts valid? */
  if (Counts > max) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  start = uatomic_read (&(counter->count));

  while (elapsed < Counts) {
    cur = uatomic_read (&(counter->count));

    if (cur >= start) {
      elapsed = cur - start;
    } else {
      /* Counter count wrapped around */
      elapsed = max - start + cur + 1;
    }
  }

#ifdef OSEK_EXTENDED
std_ret:
#endif
  return ret;
}

void
TickHandler ()
{
  Sys_IncrementCounter (SYS_COUNTER);
}

StatusType
Sys_GetAlarmBase (AlarmType alarm, AlarmBaseRefType info)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  /* Is alarm valid? */
  ValidateAlarm (alarm);
#endif

#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
  if (!info) panic ();
#endif

  AlarmQueueType alm = alarms[alarm];
  info->maxallowedvalue = alm.counter->properties.maxallowedvalue;
  info->ticksperbase = alm.counter->properties.ticksperbase;
  info->mincycle = alm.counter->properties.mincycle;

#ifdef OSEK_EXTENDED
std_ret:
#endif
  SaveOSErrorService (OSServiceId_GetAlarmBase, alarm, info, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_GetAlarm (AlarmType alarm, TickRefType tick)
{
  Counter * counter = NULL;
  TickType max = 0;
  StatusType ret = E_OK;

  if (!AlarmIsActive (alarm)) {
    ret = E_OS_NOFUNC;
    goto std_ret;
  }

#ifdef OSEK_EXTENDED
  /* Is alarm valid? */
  ValidateAlarm (alarm);
#endif

  counter = alarms[alarm].counter;
  max = counter->properties.maxallowedvalue;

  if (alarms[alarm].exp > counter->count) {
    *tick = alarms[alarm].exp - counter->count;
  } else {
    /* expiration time overflowed */
    *tick = max - counter->count + alarms[alarm].exp + 1;
  }

std_ret:
  SaveOSErrorService (OSServiceId_GetAlarm, alarm, tick, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_SetRelAlarm (AlarmType alarm, TickType inc,
                 TickType cycle)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  Counter * counter = NULL;
  TickType max = 0, mcycle = 0;

  /* Is alarm valid? */
  ValidateAlarm (alarm);
#endif

  /* If alarm is already in use, return E_OS_STATE */
  if (AlarmIsActive (alarm)) {
    ret = E_OS_STATE;
    goto std_ret;
  }

  /*
   * AUTOSAR [SWS_Os_00304]: If increment is set to 0, the
   * service shall return E_OS_VALUE in standard and
   * extended status.
   */
  if (inc == 0) {
    ret = E_OS_VALUE;
    goto std_ret;
  }

#ifdef OSEK_EXTENDED
  counter = alarms[alarm].counter;
  max = counter->properties.maxallowedvalue;
  mcycle = counter->properties.mincycle;

  /* Is start outside of the admissible limit? */
  if (inc > max) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
  /* Is cycle outside of the admissible limit? */
  if ((cycle != 0) && (cycle < mcycle || cycle > max)) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  /* Update expiration by inc from current count. */
  UpdateAlarm (alarm, inc, cycle);

  /*
   * If the relative value inc is very small or 0, the
   * alarm MAY expire, and the task may become ready or the
   * alarm-callback MAY be called before the system service
   * returns to the user.
   *
   * For now, we ignore this early trigger and directly
   * insert the alarm into queue.
   */
  ActivateAlarm (alarm);

std_ret:
  SaveOSErrorService (OSServiceId_SetRelAlarm, alarm, inc, cycle);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_SetAbsAlarm (AlarmType alarm, TickType start,
                 TickType cycle)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  Counter * counter = NULL;
  TickType max = 0, mcycle = 0;

  /* Is alarm valid? */
  ValidateAlarm (alarm);
#endif

  /* If alarm is already in use, return E_OS_STATE */
  if (AlarmIsActive (alarm)) {
    ret = E_OS_STATE;
    goto std_ret;
  }

#ifdef OSEK_EXTENDED
  counter = alarms[alarm].counter;
  max = counter->properties.maxallowedvalue;
  mcycle = counter->properties.mincycle;

  /* Is start outside of the admissible limit? */
  if (start > max) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
  /* Is cycle outside of the admissible limit? */
  if ((cycle != 0) && (cycle < mcycle || cycle > max)) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  SetAlarm (alarm, start, cycle);

  /*
   * If the absolute value start is very close to the
   * current counter value, the alarm MAY expire, and the
   * task may become ready or the alarm-callback MAY be
   * called before the system service returns to the user.
   *
   * For now, we ignore this early trigger and directly
   * insert the alarm into queue.
   */
  /* At this point, alarm is always ahead. See SetAlarm. */
  ActivateAlarm (alarm);

std_ret:
  SaveOSErrorService (OSServiceId_SetAbsAlarm, alarm, start, cycle);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_CancelAlarm (AlarmType alarm)
{
  StatusType ret = E_OK;

  if (!AlarmIsActive (alarm)) {
    ret = E_OS_NOFUNC;
    goto std_ret;
  }

#ifdef OSEK_EXTENDED
  /* Is alarm valid? */
  ValidateAlarm (alarm);
#endif

  RemoveAlarm (alarm);

std_ret:
  SaveOSErrorService (OSServiceId_CancelAlarm, alarm, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

void
AlarmInit ()
{
  int i = 0;
  AppModeType mode = Sys_GetActiveApplicationMode ();

  /* Start auto-start alarms */
  for (i = 0; i < NUM_ALARMS; i++) {
    if (auto_alarms[mode][i] == INVALID_ALARM) break;
    /*
     * OVF bit of all the alarms should be 0 at this
     * point. Which is the same as the counter OVF. Alarm
     * properties (exp, cycle, action, etc.) should all
     * have been properly initialized. As a result, calling
     * SetAlarm is not necessary.
     *
     * ActivateAlarm just inserts the alarm into its queue
     * in expiration order. No early expiration will be
     * handled here. The configuration is responsible to
     * initialize all the auto start alarms correctly.
     */
    ActivateAlarm (auto_alarms[mode][i]);
    DEBUG_PRINTFV ("Alarm %d auto started!\n",
                   auto_alarms[mode][i]);
  }

  ArchTimerInit ();
}

/* vi: set et ai sw=2 sts=2: */
