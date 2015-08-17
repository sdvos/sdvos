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
 * @file   src/schedtbl.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  AUTOSAR Schedule Table
 */
#include <sdvos.h>
#include <autosar/autosar.h>
#include <config/config.h>
#include <assert.h>

/** Global queue for all schedule tables in the system */
extern ScheduleTableStructType schedtbls[NUM_SCHED_TBLS];

/** Global queue for auto-start schedule tables */
extern ScheduleTableAutoStartType
  auto_sched_tbls[NUM_AUTOSTART_SCHED_TBLS];

/**
 * @brief Setting schedule table next event tick
 *
 * This function sets the next event tick for the given
 * schedule table according to "tick". If tick is smaller
 * than (or equal to) current counter count, it will be
 * treated as an overflow. This is similar to SetAlarm
 * function in alarm.c.
 *
 * @param[in] stid
 *   Schedule table ID
 * @param[in] tick
 *   Next event tick
 */
static void
SetScheduleTableNextTick (ScheduleTableType stid, TickType tick)
{
  Counter * counter = schedtbls[stid].counter;

  if (tick <= counter->count) {
    /* Next tick is an overflow of the current counter count */
    ToggleScheduleTableOVF (stid);
  } else {
    /* Next tick is not an overflow */
    SetScheduleTableOVF (stid);
  }

  /* Set next event tick */
  schedtbls[stid].next_tick = tick;
}

/**
 * @brief Increment schedule table next event tick
 *
 * IncScheduleTableNextTick increments schedule table next
 * event tick by inc. Overflow will be handled properly.
 * This function is similar to IncAlarm in alarm.c.
 *
 * @param[in] stid
 *   Schedule table ID
 * @param[in] inc
 *   Increment in ticks
 */
static void
IncScheduleTableNextTick (ScheduleTableType stid, TickType inc)
{
  Counter * counter = schedtbls[stid].counter;
  TickType max = counter->properties.maxallowedvalue;

  if (inc) {
    /* Update next event tick */
    if ((max - inc) < schedtbls[stid].next_tick) {
      schedtbls[stid].next_tick =
        inc - (max - schedtbls[stid].next_tick) - 1;
      ToggleScheduleTableOVF (stid);
    } else {
      schedtbls[stid].next_tick += inc;
    }
  }
}

#ifdef SCHEDTBL_SYNC
/**
 * @brief Decrement schedule table next event tick
 *
 * DecScheduleTableNextTick decrements schedule table next
 * event tick by dec. Overflow will be handled properly.
 *
 * @param[in] stid
 *   Schedule table ID
 * @param[in] dec
 *   Decrements in ticks
 */
static void
DecScheduleTableNextTick (ScheduleTableType stid, TickType dec)
{
  Counter * counter = schedtbls[stid].counter;
  TickType max = counter->properties.maxallowedvalue;

  if (dec) {
    /* Update next event tick */
    if (schedtbls[stid].next_tick >= dec) {
      schedtbls[stid].next_tick -= dec;
    } else {
      schedtbls[stid].next_tick =
        max - (dec - schedtbls[stid].next_tick - 1);
      ToggleScheduleTableOVF (stid);
    }
  }
}
#endif

/**
 * @brief Update schedule table next event tick
 *
 * UpdateScheduleTableNextTick updates schedule table next
 * event tick according to current counter count and inc.
 * Overflow will be handled properly. This function is
 * similar to UpdateAlarm in alarm.c.
 *
 * @param[in] stid
 *   Schedule table ID
 * @param[in] inc
 *   Increment in ticks
 */
static void
UpdateScheduleTableNextTick (ScheduleTableType stid, TickType inc)
{
  Counter * counter = schedtbls[stid].counter;
  TickType max = counter->properties.maxallowedvalue;

  SetScheduleTableOVF (stid);

  if (inc) {
    /* Update next event tick */
    if ((max - inc) < counter->count) {
      schedtbls[stid].next_tick = inc - (max - counter->count) - 1;
      ToggleScheduleTableOVF (stid);
    } else {
      schedtbls[stid].next_tick = inc + counter->count;
    }
  }
}

/*
 * Helper macro to start a schedule table either in the
 * "NEXT" status or for repeating. Only used in
 * RemoveScheduleTable and ProcessScheduleTable.
 *
 * @param[in] tbl
 *   Reference to Schedule table
 * @param[in] s
 *   Schedule table status
 * @param[in] next
 *   Start next table? (TRUE or FALSE)
 */
#define StartTable(tbl, s, next)  do {                \
  /* Set status to s */                               \
  (tbl)->status = (s);                                \
  /* Set SCHEDULETABLE_PROCESSING flag */             \
  (tbl)->flag |= SCHEDULETABLE_PROCESSING;            \
  /* Set next_exp to the first expiry point */        \
  (tbl)->next_exp = 0;                                \
  /* Update next_tick and OVF */                      \
  if (next) {                                         \
    UpdateScheduleTableNextTick ((tbl)->id,           \
      (tbl)->exps[0].offset);                         \
  } else {                                            \
    IncScheduleTableNextTick ((tbl)->id,              \
      (tbl)->exps[0].offset);                         \
  }                                                   \
} while (0)

#ifdef MULTI_SCHEDTBL_PER_CNTR
/*
 * Internal macro to remove a schedule table from counter
 * schedule table queue. Only used when
 * MULTI_SCHEDTBL_PER_CNTR is defined.
 */
#define UNLINKTABLE(t)  do {                          \
    if ((t)->prev) {                                  \
      (t)->prev->next = (t)->next;                    \
      if ((t)->next) {                                \
        (t)->next->prev = (t)->prev;                  \
      }                                               \
    } else {                                          \
      /* sched_tbl is the first in queue */           \
      (t)->counter->schedtbl = (t)->next;             \
      if ((t)->next) {                                \
        (t)->next->prev = NULL;                       \
      }                                               \
    }                                                 \
} while (0)
#endif

#ifdef SCHEDTBL_SYNC
/**
 * @brief Adjust schedule table next event for synchronization
 *
 * AdjustScheduleTable adjusts the next event tick for a
 * schedule table for synchronization. The schedule table
 * must have a valid next expiry point.
 *
 * @param[in] sched_tbl
 *   Reference to a ScheduleTableStructType structure
 */
static void
AdjustScheduleTable (ScheduleTableStructType * sched_tbl)
{
  TickType adj = 0;

  if (!(sched_tbl->flag & SCHEDULETABLE_SYNCING)) {
    /*
     * Do nothing if schedule table does not require
     * synchronization.
     */
    return;
  } else if (sched_tbl->deviation == 0) {
    /* Deviation is 0, synchronized */
    sched_tbl->status = SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;
  } else {
    /* Adjustment is needed */
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
    /* There should be a next expiry point */
    ASSERT (sched_tbl->next_exp != INVALID_EP);
#endif
    if (sched_tbl->flag & SCHEDULETABLE_DEVIATION_SIGN) {
      /* Deviation is positive */
      /* Delay next event tick */
      adj = MIN (sched_tbl->deviation,
        sched_tbl->exps[sched_tbl->next_exp].max_lengthen);
      IncScheduleTableNextTick (sched_tbl->id, adj);
      /* Update deviation */
      sched_tbl->deviation -= adj;
    } else {
      /* Deviation is negative */
      /* Bring next event tick forward */
      adj = MIN (sched_tbl->deviation,
        sched_tbl->exps[sched_tbl->next_exp].max_shorten);
      DecScheduleTableNextTick (sched_tbl->id, adj);
      /* Update deviation */
      sched_tbl->deviation -= adj;
    }

    if (sched_tbl->deviation > sched_tbl->precision) {
      /* Deviation greater than precision, table not sync'ed */
      sched_tbl->status = SCHEDULETABLE_RUNNING;
    } else {
      /* Deviation within precision, table synchronized */
      sched_tbl->status = SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;
    }
  }
}
#endif

/**
 * @brief Remove a schedule table from counter table queue
 *
 * RemoveScheduleTable removes a schedule table from its
 * counter schedule table queue. It takes care of updating
 * schedule table status and handling the schedule table
 * in the "NEXT" status.
 *
 * @param[in] sched_tbl
 *   Reference to a ScheduleTableStructType structure
 * @return
 *   NULL or sched_tbl->to_tbl. If to_tbl is returned,
 *   initial offset of to_tbl is 0 and actions need to
 *   be handled right away.
 */
static ScheduleTableStructType *
RemoveScheduleTable (ScheduleTableStructType * sched_tbl)
{
  ScheduleTableStructType * next_tbl = sched_tbl->to_tbl;
  ScheduleTableStructType * ret_tbl = NULL;

  /* Change schedule table status */
  sched_tbl->status = SCHEDULETABLE_STOPPED;

  if (next_tbl) {
#ifdef MULTI_SCHEDTBL_PER_CNTR
    /* Next table exists, link it into queue */
    if (sched_tbl->prev) {
      sched_tbl->prev->next = next_tbl;
      next_tbl->prev = sched_tbl->prev;
      if (sched_tbl->next) {
        sched_tbl->next->prev = next_tbl;
        next_tbl->next = sched_tbl->next;
      }
    } else {
      /* sched_tbl is head of queue */
      sched_tbl->counter->schedtbl = next_tbl;
      /* Set both next and prev to NULL */
      next_tbl->next = next_tbl->prev = NULL;
    }
#else
    /* Next table exists */
    sched_tbl->counter->schedtbl = next_tbl;
#endif

    /* Start processing next table */
    if (next_tbl->exps[0].offset == 0) {
      /*
       * Next table has 0 initial offset, should this
       * really happen? The specification does not
       * forbid it. So, we will notify the caller by
       * returning next_tbl. ProcessScheduleTable will
       * handle it appropriately.
       */
      ret_tbl = next_tbl;
      next_tbl->status = SCHEDULETABLE_RUNNING;
      next_tbl->flag |= SCHEDULETABLE_PROCESSING;
      next_tbl->next_exp = 0;
      /* Set next_tick and OVF to current */
      SetScheduleTableOVF (next_tbl->id);
      next_tbl->next_tick = next_tbl->counter->count;
    } else {
      StartTable (next_tbl, SCHEDULETABLE_RUNNING, TRUE);
    }
#ifdef SCHEDTBL_SYNC
    /*
     * If OsScheduleTblSyncStrategy of the schedule tables
     * <ScheduleTableID_From> and <ScheduleTableID_To> in a
     * call of NextScheduleTable() is EXPLICIT and the
     * Operating System module already synchronizes
     * <ScheduleTableID_From>, NextScheduleTable() shall
     * continue synchonization after the start of processing
     * <ScheduleTableID_To>. [SWS_Os_00505]
     *
     * According to the state transition chart in the
     * specification, a schedule table would not change its
     * state from NEXT to SYNCHRONOUS directly. When
     * starting the next table here, we will first set the
     * status to RUNNING. The synchronization of the table
     * will be begin at the time the first expiry point is
     * processed. If the next table has a 0 initial offset,
     * the first expiry point will be processed immediately
     * after this function returns and synchronization will
     * kick in right after that too. This is essentially
     * allowing a schedule table to switch from NEXT to
     * SYNCHRONOUS directly. But this should not cause any
     * troubles.
     */
    if ((next_tbl->flag & SCHEDULETABLE_SYNC_EXPLICIT) &&
        (sched_tbl->flag & SCHEDULETABLE_SYNCING)) {
      /* Set synchronization flag to request synchronization */
      next_tbl->flag |= SCHEDULETABLE_SYNCING;
      /* Copy deviation value from ScheduleTableID_From */
      next_tbl->deviation = sched_tbl->deviation;
      /* Copy deviation sign flag from ScheduleTableID_From */
      next_tbl->flag |=
        (sched_tbl->flag & SCHEDULETABLE_DEVIATION_SIGN);
    }

    /*
     * If synchronization should (optional) start immediately
     * after switching to the next table, comment out the
     * following three lines. Basically, we will synchronize
     * right away if the initial offset of the next table is
     * not 0. Otherwise, we cannot synchronize right away
     * because of next_tick consistency.
     */
    /*
     * if (!ret_tbl) {
     *   AdjustScheduleTable (next_tbl);
     * }
     */
#endif
  } else {
#ifdef MULTI_SCHEDTBL_PER_CNTR
    /* No table in NEXT status, remove sched_tbl */
    UNLINKTABLE (sched_tbl);
#else
    /* No table in NEXT status, just clear schedtbl field */
    sched_tbl->counter->schedtbl = NULL;
#endif
  }

  return ret_tbl;
}

static void
DoScheduleTableActions (ScheduleTableStructType * sched_tbl,
                        udata_word_t epid)
{
  int i = 0;
  TaskType * tasks = NULL;
  ExpiryPointEventList * events = NULL;

  /*
   * Task activations will be processed before setting
   * events. [SWS_Os_00412]
   */
  tasks = sched_tbl->exps[epid].tasks;
  events = sched_tbl->exps[epid].events;

  /* Process all task activations */
  for (i = 0; i < sched_tbl->exps[epid].num_tasks; i++) {
    Sys_ActivateTask (tasks[i]);
  }

  /* Set all events */
  for (i = 0; i < sched_tbl->exps[epid].num_events; i++) {
    Sys_SetEvent (events[i].tid, events[i].mask);
  }
}

/**
 * @brief Process a single schedule table
 *
 * ProcessScheduleTable processes a single schedule table.
 * It checks the current counter count and the schedule
 * table next_tick to see whether the next event has
 * arrived. If this is the case, it will process the event
 * and handle the schedule table management automatically.
 * After processing, the status, flags, next_tick, next_exp
 * of the schedule table will be updated. Counter schedule
 * table queue will also be modified accordingly (including
 * removing the table when it is stopped and moving the
 * table in the NEXT state up into the linked list).
 *
 * @param[in] sched_tbl
 *   Reference to a ScheduleTableStructType structure
 */
static void
ProcessScheduleTable (ScheduleTableStructType * sched_tbl)
{
  Counter * cur_counter = sched_tbl->counter;
  TickType max = cur_counter->properties.maxallowedvalue;
  TickType tpb = cur_counter->properties.ticksperbase;
  TickType ntick = sched_tbl->next_tick;
  udata_word_t epid = INVALID_EP;

  /* Have we arrived at next_tick? */
  if (CounterOVF (cur_counter) ==
      ScheduleTableOVF (sched_tbl->id)) {
    if (cur_counter->count >= ntick) {
      /* Next event arrived */
      goto handle_next_event;
    }
  } else if (max - ntick + cur_counter->count + 1 < tpb) {
      /* Next event arrived */
      goto handle_next_event;
  }

  /* Next event in the future, no processing needed */
  return;

handle_next_event:
  /*
   * Handle next event for the schedule table. Possible
   * events for a schedule table involves:
   *
   * 1. Next expiry point has been reached.
   * 2. The initial waiting period after calling
   *    StartScheduleTableRel or StartScheduleTableAbs
   *    has past.
   * 3. The final delay has just finished.
   */
  epid = sched_tbl->next_exp;

  if (epid != INVALID_EP) {
    /* next_exp exists, need to process expiry point */
    DoScheduleTableActions (sched_tbl, epid);

    if ((epid + 1) < sched_tbl->num_exps) {
      /* We have a next expiry point */
      /* Update next_tick and OVF bit */
      IncScheduleTableNextTick (sched_tbl->id,
                                sched_tbl->exps[epid + 1].offset -
                                sched_tbl->exps[epid].offset);
      /* Update next_exp */
      sched_tbl->next_exp++;

#ifdef SCHEDTBL_SYNC
      AdjustScheduleTable (sched_tbl);
#endif
    } else {
      /* next_exp is the last expiry point */
      if (sched_tbl->delay) {
        /* Final delay needed, set DELAYING flag */
        sched_tbl->flag |= SCHEDULETABLE_DELAYING;
        /* Update next_tick and OVF */
        IncScheduleTableNextTick (sched_tbl->id, sched_tbl->delay);
        /* Clear next_exp */
        sched_tbl->next_exp = INVALID_EP;
      } else {
        /*
         * No final delay, stop the table. Schedule table
         * is single shot. Repeating schedule table must
         * have a final delay.
         */
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
        ASSERT ((sched_tbl->flag & SCHEDULETABLE_REPEATING) == 0);
#endif
        if ((sched_tbl = RemoveScheduleTable (sched_tbl))) {
          /* next table has 0 initial offset */
          goto handle_next_event;
        }
      }
    }
  } else if (!(sched_tbl->flag & SCHEDULETABLE_PROCESSING)) {
    /* Initial waiting period is over */
    StartTable (sched_tbl, sched_tbl->status, FALSE);

#ifdef SCHEDTBL_SYNC
    AdjustScheduleTable (sched_tbl);
#endif
  } else if (sched_tbl->flag & SCHEDULETABLE_DELAYING) {
    /* Delay is over */
    /* Clear the SCHEDULETABLE_DELAYING flag */
    sched_tbl->flag &= (FlagType) ~SCHEDULETABLE_DELAYING;

    if ((sched_tbl->to_tbl) ||
        (!(sched_tbl->flag & SCHEDULETABLE_REPEATING))) {
      /*
       * Next table exists or current table is not repeating.
       * When next table exists, the table switch will happen
       * even if current table is REPEATING. [SWS_Os_00284]
       * NextScheduleTable() is a service to "switch" table.
       */
      if ((sched_tbl = RemoveScheduleTable (sched_tbl))) {
        /* next table has 0 initial offset */
        goto handle_next_event;
      }
    } else {
      /* Schedule table is repeating and no next table */
      if (sched_tbl->exps[0].offset == 0) {
        /* Initial offset is 0. Need to handle actions now. */
        /* Point next expiry point to first expiry point */
        sched_tbl->next_exp = 0;

        /*
         * Go back to event handling entry point. Next tick
         * and OVF do not need to be updated since they
         * should be "current". Endless loop would not
         * happen here since a final delay must exist for
         * repeating schedule table.
         */
        goto handle_next_event;
      }

      /* Otherwise, enter initial offset */
      StartTable (sched_tbl, sched_tbl->status, FALSE);

#ifdef SCHEDTBL_SYNC
      AdjustScheduleTable (sched_tbl);
#endif
    }
  }

  return;
}

void
CheckScheduleTables (Counter * counter)
{
  ScheduleTableStructType * sched_tbl = counter->schedtbl;

#ifdef MULTI_SCHEDTBL_PER_CNTR
  ScheduleTableStructType * next_tbl = NULL;
#endif

  /* No running schedule table, return */
  if (!sched_tbl) return;

#ifdef MULTI_SCHEDTBL_PER_CNTR
  while (sched_tbl) {
    /*
     * We need to save next before calling
     * ProcessScheduleTable. sched_tbl might be removed
     * afther this function and we will loose the next
     * expiry point.
     */
    next_tbl = sched_tbl->next;
    /*
     * Process sched_tbl. This will process the expiry
     * point, or stop the table if it is finished. Table
     * in the NEXT status will also be handled properly.
     */
    ProcessScheduleTable (sched_tbl);
    sched_tbl = next_tbl;
  }
#else
  /* Only one table possibly with a "NEXT" table */
  ProcessScheduleTable (sched_tbl);
#endif
}

/*
 * Internal macro used to initialize part of the schedule
 * table flags and fields.
 */
#ifdef SCHEDTBL_SYNC

#define INITTABLE(t)  do {                                \
  /* Clear SCHEDULETABLE_PROCESSING for waiting period */ \
  (t)->flag &= (FlagType) (~SCHEDULETABLE_PROCESSING);    \
  /* Clear SCHEDULETABLE_DELAYING flag */                 \
  (t)->flag &= (FlagType) (~SCHEDULETABLE_DELAYING);      \
  /* Clear SCHEDULETABLE_SYNCING flag */                  \
  (t)->flag &= (FlagType) (~SCHEDULETABLE_SYNCING);       \
  /* No next expiry point in waiting period */            \
  (t)->next_exp = INVALID_EP;                             \
  /* Clear to_tbl, there should be no next table */       \
  (t)->to_tbl = NULL;                                     \
  /* Clear from_table */                                  \
  (t)->from_tbl = NULL;                                   \
  /* Clear deviation */                                   \
  (t)->deviation = 0;                                     \
} while (0)

#else

#define INITTABLE(t)  do {                                \
  /* Clear SCHEDULETABLE_PROCESSING for waiting period */ \
  (t)->flag &= (FlagType) (~SCHEDULETABLE_PROCESSING);    \
  /* Clear SCHEDULETABLE_DELAYING flag */                 \
  (t)->flag &= (FlagType) (~SCHEDULETABLE_DELAYING);      \
  /* No next expiry point in waiting period */            \
  (t)->next_exp = INVALID_EP;                             \
  /* Clear to_tbl, there should be no next table */       \
  (t)->to_tbl = NULL;                                     \
  /* Clear from_table */                                  \
  (t)->from_tbl = NULL;                                   \
} while (0)

#endif

/**
 * @brief
 * Link a schedule table into its counter table queue
 *
 * LinkTable inserts a schedule table into its counter
 * table queue. This is only used internally in the kernel.
 *
 * @param[in] sched_tbl
 *   Reference to a ScheduleTableStructType structure
 * @param[in] cur_counter
 *   Reference to sched_tbl's counter structure
 */
static void
LinkTable (ScheduleTableStructType * sched_tbl,
           Counter * cur_counter) {
#ifdef MULTI_SCHEDTBL_PER_CNTR
  ScheduleTableStructType * tbl_index = NULL;

  /* sched_tbl will be at the end of the queue */
  sched_tbl->next = NULL;

  if (cur_counter->schedtbl) {
    /* Find tail of the schedule table queue */
    tbl_index = cur_counter->schedtbl;
    while (tbl_index->next) tbl_index = tbl_index->next;
    /* tbl_index points to the tail */
    tbl_index->next = sched_tbl;
    sched_tbl->prev = tbl_index;
  } else {
    /* First in the queue */
    cur_counter->schedtbl = sched_tbl;
    sched_tbl->prev = NULL;
  }
#else
  /*
   * If no MULTI_SCHEDTBL_PER_CNTR, a counter will not have
   * multiple schedule tables. And since we checked table
   * status earlier, this table will not be on the running
   * table queue. As a result, counter->schedtbl will be
   * NULL. If this is not the case, something really bad
   * happened...
   */
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
  ASSERT (cur_counter->schedtbl == NULL);
#endif

  cur_counter->schedtbl = sched_tbl;
#endif
}

StatusType
Sys_StartScheduleTableRel (ScheduleTableType ScheduleTableID,
                           TickType Offset)
{
  ScheduleTableStructType * sched_tbl = NULL;
  StatusType ret = E_OK;
  Counter * cur_counter = NULL;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00275] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];
  cur_counter = sched_tbl->counter;

#ifdef OSEK_EXTENDED
  /* Is offset invalid? [SWS_Os_00332], [SWS_Os_00276] */
  if ((Offset == 0) ||
      ((cur_counter->properties.maxallowedvalue -
       sched_tbl->exps->offset) < Offset)) {
    /*
     * Initial offset + relative offset <= counter max value
     * This property makes it possible to calculate the
     * actual initial offset with possible overflow right
     * now. However, this is not the case for the
     * StartScheduleTableAbs interface.
     */
    ret = E_OS_VALUE;
    goto std_ret;
  }

  if (sched_tbl->flag & SCHEDULETABLE_SYNC_IMPLICIT) {
    /* Schedule table is implicitly synchronized, [SWS_Os_00452] */
    ret =  E_OS_ID;
    goto std_ret;
  }
#endif

  if (sched_tbl->status != SCHEDULETABLE_STOPPED) {
    /* Schedule table is not stopped [SWS_Os_00277] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  /* Set schedule table next event tick to current count + Offset */
  UpdateScheduleTableNextTick (sched_tbl->id, Offset);

  /* [SWS_Os_00278] */
  sched_tbl->status = SCHEDULETABLE_RUNNING;

  /* Initialize table flags and some fields */
  INITTABLE (sched_tbl);

  /* Initial offset is 0. Special case. */
  if (sched_tbl->exps[0].offset == 0) {
    /* Skip table initial wait, treat Offset as initial offset */
    sched_tbl->flag |= SCHEDULETABLE_PROCESSING;
    /* Point next expiry point to first expiry point */
    sched_tbl->next_exp = 0;
  }

  /* Link schedule table to counter schedule table queue */
  LinkTable (sched_tbl, cur_counter);

std_ret:
  SaveOSErrorService (OSServiceId_StartScheduleTableRel,
                      ScheduleTableID, Offset, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_StartScheduleTableAbs (ScheduleTableType ScheduleTableID,
                           TickType Start)
{
  ScheduleTableStructType * sched_tbl = NULL;
  StatusType ret = E_OK;
  Counter * cur_counter = NULL;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00348] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];
  cur_counter = sched_tbl->counter;

#ifdef OSEK_EXTENDED
  /* Is Start invalid? [SWS_Os_00349] */
  if (cur_counter->properties.maxallowedvalue < Start) {
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  if (sched_tbl->status != SCHEDULETABLE_STOPPED) {
    /* Schedule table is not stopped [SWS_Os_00350] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  /* Set schedule table next event tick to Start */
  /*
   * If Start is in the past but current time is somewhere
   * within the duration of the schedule table, we still
   * wait until the counter count overflows and reach Start
   * before starting processing the first expiry point.
   * This should be the right behavior according to
   * [SWS_Os_00351]. Check the line that says:
   *
   *   ... when the underlying counter next equals
   *   <Start> ...
   */
  SetScheduleTableNextTick (sched_tbl->id, Start);

  /* [SWS_Os_00351] */
  if (sched_tbl->flag & SCHEDULETABLE_SYNC_IMPLICIT) {
    /* Implicitly sychronized schedule table starts at 0 */
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
    ASSERT (Start == 0);
#endif
    sched_tbl->status = SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;
  } else {
    sched_tbl->status = SCHEDULETABLE_RUNNING;
  }

  /* Initialize table flags and some fields */
  INITTABLE (sched_tbl);

  /* Initial offset is 0. Special case. */
  if (sched_tbl->exps[0].offset == 0) {
    /* Skip table initial wait, treat Offset as initial offset */
    sched_tbl->flag |= SCHEDULETABLE_PROCESSING;
    /* Point next expiry point to first expiry point */
    sched_tbl->next_exp = 0;
  }

  /* Link schedule table to counter schedule table queue */
  LinkTable (sched_tbl, cur_counter);

std_ret:
  SaveOSErrorService (OSServiceId_StartScheduleTableAbs,
                      ScheduleTableID, Start, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_StopScheduleTable (ScheduleTableType ScheduleTableID)
{
  ScheduleTableStructType * sched_tbl = NULL;
  ScheduleTableStructType * next_tbl = NULL;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00279] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];

  if (sched_tbl->status == SCHEDULETABLE_STOPPED) {
    /* Schedule table is stopped [SWS_Os_00280] */
    ret = E_OS_NOFUNC;
    goto std_ret;
  }

#ifdef SCHEDTBL_SYNC
  if (sched_tbl->status == SCHEDULETABLE_WAITING) {
    /*
     * Schedule table is waiting for synchronization. It is
     * not in the counter schedule table queue yet. Just
     * change schedule table status to STOPPED.
     */
    sched_tbl->status = SCHEDULETABLE_STOPPED;
    goto std_ret;
  }
#endif

  if (sched_tbl->status == SCHEDULETABLE_NEXT) {
    /* Schedule table is in "NEXT" status */
    /* Change schedule table status to STOPPED */
    sched_tbl->status = SCHEDULETABLE_STOPPED;
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
    /* from_tbl should not be NULL */
    ASSERT (sched_tbl->from_tbl != NULL);
    /* to_tbl should be NULL */
    ASSERT (sched_tbl->to_tbl == NULL);
#endif
    /* Remove table from current table's to_tbl */
    sched_tbl->from_tbl->to_tbl = NULL;
  } else {
    /* Schedule table is not in "NEXT" status */
    /* Change schedule table status to STOPPED */
    sched_tbl->status = SCHEDULETABLE_STOPPED;

    /* Remove sched_tbl from queue */
#ifdef MULTI_SCHEDTBL_PER_CNTR
    UNLINKTABLE (sched_tbl);
#else
    sched_tbl->counter->schedtbl = NULL;
#endif

    /*
     * If sched_tbl has a "NEXT" table in the to_tbl field,
     * we should also stop the next table. [SWS_Os_00453]
     */
    if ((next_tbl = sched_tbl->to_tbl) != NULL) {
      /* Change status to STOPPED */
      next_tbl->status = SCHEDULETABLE_STOPPED;
    }
  }

std_ret:
  SaveOSErrorService (OSServiceId_StopScheduleTable,
                      ScheduleTableID, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_NextScheduleTable (ScheduleTableType ScheduleTableID_From,
                       ScheduleTableType ScheduleTableID_To)
{
  ScheduleTableStructType * sched_tbl = NULL;
  ScheduleTableStructType * next_tbl = NULL;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  if ((ScheduleTableID_From >= NUM_SCHED_TBLS) ||
      (ScheduleTableID_To >= NUM_SCHED_TBLS)) {
    /* ScheduleTableID not valid [SWS_Os_00282] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID_From];
  next_tbl = &schedtbls[ScheduleTableID_To];

#ifdef OSEK_EXTENDED
  if (sched_tbl->counter != next_tbl->counter) {
    /*
     * ScheduleTableID_From and ScheduleTableID_To are
     * dirven by different counters. [SWS_Os_00330]
     */
    ret = E_OS_ID;
    goto std_ret;
  }

  if ((sched_tbl->flag & (SCHEDULETABLE_SYNC_IMPLICIT |
      SCHEDULETABLE_SYNC_EXPLICIT)) !=
      (next_tbl->flag & (SCHEDULETABLE_SYNC_IMPLICIT |
      SCHEDULETABLE_SYNC_EXPLICIT))) {
    /* Different synchronization strategy. [SWS_Os_00484] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  if ((sched_tbl->status == SCHEDULETABLE_STOPPED) ||
      (next_tbl->status == SCHEDULETABLE_NEXT)) {
    /* ScheduleTableID_From not stopped. [SWS_Os_00283] */
    ret = E_OS_NOFUNC;
    goto std_ret;
  }

  if (next_tbl->status != SCHEDULETABLE_STOPPED) {
    /* ScheduleTableID_To is started or next. [SWS_Os_00309] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  if (sched_tbl->to_tbl) {
    /*
     * ScheduleTableID_From already has a next table. It
     * should be stopped and replaced with
     * ScheduleTableID_To. [SWS_Os_00324]
     */
    sched_tbl->to_tbl->status = SCHEDULETABLE_STOPPED;
  }

  /* Initialize ScheduleTableID_To states */
  INITTABLE (next_tbl);

  /* Link next_tbl to sched_tbl */
  sched_tbl->to_tbl = next_tbl;
  next_tbl->from_tbl = sched_tbl;

  next_tbl->status = SCHEDULETABLE_NEXT;

std_ret:
  SaveOSErrorService (OSServiceId_NextScheduleTable,
                      ScheduleTableID_From, ScheduleTableID_To, 0);
  ERRORHOOK (ret);
  return ret;
}

#ifdef SCHEDTBL_SYNC
StatusType
Sys_StartScheduleTableSynchron (ScheduleTableType ScheduleTableID)
{
  ScheduleTableStructType * sched_tbl = NULL;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00387] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];

#ifdef OSEK_EXTENDED
  if (!(sched_tbl->flag & SCHEDULETABLE_SYNC_EXPLICIT)) {
    /* ScheduleTableID not explicitly synchronized [SWS_Os_00387] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  if (sched_tbl->status != SCHEDULETABLE_STOPPED) {
    /* Schedule table is not stopped [SWS_Os_00388] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  /* [SWS_Os_00389] */
  sched_tbl->status = SCHEDULETABLE_WAITING;

  /* Initialize table flags and some fields */
  INITTABLE (sched_tbl);

  /*
   * We do not need to do anything else here since the
   * table will not be inserted into the counter table
   * queue. This will be done when the first sync count
   * is provided througb SyncScheduleTable (). The flag
   * to request synchronization (SCHEDULETABLE_SYNCING)
   * will also be set later if necessary.
   */

std_ret:
  SaveOSErrorService (OSServiceId_StartScheduleTableSynchron,
                      ScheduleTableID, 0, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_SyncScheduleTable (ScheduleTableType ScheduleTableID,
                       TickType Value)
{
  ScheduleTableStructType * sched_tbl = NULL;
  StatusType ret = E_OK;
  Counter * cur_counter = NULL;
  TickType pos_on_table = 0, diff = 0, diff_ovf = 0;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00454] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];
  cur_counter = sched_tbl->counter;

#ifdef OSEK_EXTENDED
  if (!(sched_tbl->flag & SCHEDULETABLE_SYNC_EXPLICIT)) {
    /* ScheduleTableID not explicitly synchronized [SWS_Os_00454] */
    ret = E_OS_ID;
    goto std_ret;
  }

  if (Value >= sched_tbl->duration) {
    /* Value is out of range [SWS_Os_00455] */
    ret = E_OS_VALUE;
    goto std_ret;
  }
#endif

  if ((sched_tbl->status == SCHEDULETABLE_STOPPED) ||
      (sched_tbl->status == SCHEDULETABLE_NEXT)) {
    /* Schedule table is stopped or next [SWS_Os_00456] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  /* Set synchronization flag */
  sched_tbl->flag |= SCHEDULETABLE_SYNCING;

  if (sched_tbl->status == SCHEDULETABLE_WAITING) {
    /*
     * Schedule table is in waiting state. It is not yet
     * put into the counter table queue. We need to figure
     * out the initial offset of the table (not the first
     * expiry point).
     *
     *   next_tick = drive_ctr.now + duration - Value
     */
    UpdateScheduleTableNextTick (sched_tbl->id,
                                 sched_tbl->duration - Value);

    /* Schedule table is synchronous now */
    sched_tbl->status = SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;

    /* Link schedule table to counter schedule table queue */
    LinkTable (sched_tbl, cur_counter);
  } else {
    /* Schedule table is running (RUNNING or SYNCHRONOUS) */
    /* Calculate deviation, find out current table position */
    if ((!(sched_tbl->flag & SCHEDULETABLE_PROCESSING)) ||
        (sched_tbl->flag & SCHEDULETABLE_DELAYING)) {
      /* Schedule table is still in initial or final delay */
      /* Duration is max table offset + delay */
      pos_on_table = sched_tbl->duration -
        (sched_tbl->next_tick - cur_counter->count);
    } else {
      /* Schedule table is already up and running */
#if defined DEBUG_SDVOS || defined DEBUG_SDVOS_VERBOSE
      ASSERT (sched_tbl->next_exp != INVALID_EP);
#endif
      pos_on_table = sched_tbl->exps[sched_tbl->next_exp].offset -
        (sched_tbl->next_tick - cur_counter->count);
    }

    /*
     * Deviation can be derived from pos_on_table. Overflow
     * makes things a little bit more complicated. We have
     * to compare a "normal" diff and a "overflow" to see
     * which is smaller in absolute value to see the best
     * direction of adjustment for the table synchronization.
     */
    if (pos_on_table >= Value) {
      diff = pos_on_table - Value;
      diff_ovf = sched_tbl->duration - pos_on_table + Value;
      if (diff > diff_ovf) {
        /* Overflow diff is smaller */
        sched_tbl->deviation = diff_ovf;
        /* Deviation is negative */
        sched_tbl->flag |= SCHEDULETABLE_DEVIATION_SIGN;
      } else {
        /* diff is smaller */
        sched_tbl->deviation = diff;
        /* Deviation is positive */
        sched_tbl->flag &= (FlagType) (~SCHEDULETABLE_DEVIATION_SIGN);
      }
    } else {
      diff = Value - pos_on_table;
      diff_ovf = sched_tbl->duration - Value + pos_on_table;
      if (diff > diff_ovf) {
        /* Overflow diff is smaller */
        sched_tbl->deviation = diff_ovf;
        /* Deviation is positive */
        sched_tbl->flag &= (FlagType) (~SCHEDULETABLE_DEVIATION_SIGN);
      } else {
        /* diff is smaller */
        sched_tbl->deviation = diff;
        /* Deviation is negative */
        sched_tbl->flag |= SCHEDULETABLE_DEVIATION_SIGN;
      }
      
      sched_tbl->deviation = Value - pos_on_table;
      /* Deviation is negative */
      sched_tbl->flag |= SCHEDULETABLE_DEVIATION_SIGN;
    }

    /* Update schedule table status according to deviation */
    if (sched_tbl->deviation > sched_tbl->precision) {
      /* Deviation greater than precision, table not sync'ed */
      sched_tbl->status = SCHEDULETABLE_RUNNING;
    } else {
      /* Deviation within precision, table synchronized */
      sched_tbl->status = SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;
    }
  }

std_ret:
  SaveOSErrorService (OSServiceId_SyncScheduleTable,
                      ScheduleTableID, Value, 0);
  ERRORHOOK (ret);
  return ret;
}

StatusType
Sys_SetScheduleTableAsync (ScheduleTableType ScheduleTableID)
{
  ScheduleTableStructType * sched_tbl = NULL;
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00458] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  sched_tbl = &schedtbls[ScheduleTableID];

#ifdef OSEK_EXTENDED
  if (!(sched_tbl->flag & SCHEDULETABLE_SYNC_EXPLICIT)) {
    /* ScheduleTableID not explicitly synchronized [SWS_Os_00458] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  if ((sched_tbl->status == SCHEDULETABLE_STOPPED) ||
      (sched_tbl->status == SCHEDULETABLE_NEXT) ||
      (sched_tbl->status == SCHEDULETABLE_WAITING)) {
    /* [SWS_Os_00483] */
    ret = E_OS_STATE;
    goto std_ret;
  }

  /* Change table status [SWS_Os_00300]. */
  sched_tbl->status = SCHEDULETABLE_RUNNING;
  /* Clear deviation value */
  sched_tbl->deviation = 0;
  /* Clear SCHEDULETABLE_SYNCING flag */
  sched_tbl->flag &= (FlagType) (~SCHEDULETABLE_SYNCING);

std_ret:
  SaveOSErrorService (OSServiceId_SetScheduleTableAsync,
                      ScheduleTableID, 0, 0);
  ERRORHOOK (ret);
  return ret;
}
#endif

StatusType
Sys_GetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus)
{
  StatusType ret = E_OK;

#ifdef OSEK_EXTENDED
  if (ScheduleTableID >= NUM_SCHED_TBLS) {
    /* ScheduleTableID is not valid [SWS_Os_00293] */
    ret = E_OS_ID;
    goto std_ret;
  }
#endif

  *ScheduleStatus = schedtbls[ScheduleTableID].status;

std_ret:
  SaveOSErrorService (OSServiceId_GetScheduleTableStatus,
                      ScheduleTableID, ScheduleStatus, 0);
  ERRORHOOK (ret);
  return ret;
}

void
ScheduleTableInit ()
{
  int i = 0;
  AppModeType mode = Sys_GetActiveApplicationMode ();

  /* Start auto-start schedule tables */
  for (i = 0; i < NUM_AUTOSTART_SCHED_TBLS; i++) {
    if (auto_sched_tbls[i].appmode != mode) break;
    switch (auto_sched_tbls[i].mode) {
      case SCHEDTBL_STARTMODE_ABSOLUTE :
        Sys_StartScheduleTableAbs (auto_sched_tbls[i].id,
                                   auto_sched_tbls[i].value);
        break;
      case SCHEDTBL_STARTMODE_RELATIVE :
        Sys_StartScheduleTableRel (auto_sched_tbls[i].id,
                                   auto_sched_tbls[i].value);
        break;
#ifdef SCHEDTBL_SYNC
      case SCHEDTBL_STARTMODE_SYNCHRON :
        Sys_StartScheduleTableSynchron (auto_sched_tbls[i].id);
        break;
#endif
      default:
        /* Should not happen */
        break;
    }
    DEBUG_PRINTF ("Schedule table %d auto started!\n", i);
  }
}

/* vi: set et ai sw=2 sts=2: */
