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
 * @file   src/include/counter.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Counter Related Data Structures
 *
 * Counter interface is not specified directly in OSEK.
 */
#ifndef _COUNTER_H_
#define _COUNTER_H_

#include <osek/types.h>
#include <osek/alarm.h>
#include <autosar/schedtbl.h>

struct counter_t;

/** Flag for alarm overflow */
#define ALARM_STATUS_OVF       (0x1U << 0)
/** Flag for alarm activity */
#define ALARM_STATUS_ACTIVE    (0x1U << 1)

/** This data type represents an alarm object in alarm queue */
typedef struct alarm_queue_t {
  AlarmType id;                /**< Alarm ID */
  struct counter_t * counter;  /**< Counter object */
  FlagType status;             /**< Status flag */
  TickType cycle;              /**< Period of alarm */
  TickType exp;                /**< Next alarm expiration*/
  AlarmActionType action;      /**< Alarm expiration action */
  struct alarm_queue_t * next; /**< Next alarm in queue */
  struct alarm_queue_t * prev; /**< Previous alarm in queue */
} AlarmQueueType;

/* Schedule table flags */

/* Schedule table synchronization strategy */
/** The schedule table is implicitly synchronized */
#define SCHEDULETABLE_SYNC_IMPLICIT       (0x1U << 0)
/** The schedule table is explicitly synchronized */
#define SCHEDULETABLE_SYNC_EXPLICIT       (0x1U << 1)

/** The schedule table next event tick overflow flag */
#define SCHEDULETABLE_OVF                 (0x1U << 2)
/**
 * The schedule table processing is started. This is
 * different from "RUNNING". This flag will be set when
 * the initial waiting time is past. This initial waiting time
 * is not the initial offset of the table. It is instead
 * the relative offset when starting a table with
 * StartScheduleTableRel or the offset between the absolute
 * start time and current time when the schedule table is
 * started with StartScheduleTableAbs. When a schedule
 * table is started with either interfaces, the next_tick
 * in the schedule table will be set to the waiting time
 * offset. When next_tick is reached, this flag will be set
 * for the schedule table.
 */
#define SCHEDULETABLE_PROCESSING          (0x1U << 3)
/** The schedule table is in final delay */
#define SCHEDULETABLE_DELAYING            (0x1U << 4)
/** The schedule table is repeating */
#define SCHEDULETABLE_REPEATING           (0x1U << 5)
/** The schedule table is deviation sign bit (0 +, 1 -) */
#define SCHEDULETABLE_DEVIATION_SIGN      (0x1U << 6)
/** The schedule table need synchronization */
#define SCHEDULETABLE_SYNCING             (0x1U << 7)

/**
 * This data type represents an event list of a schedule
 * table expiry point
 */
typedef struct expiry_point_event_list {
  TaskType tid;                          /**< Task ID */
  EventMaskType mask;                    /**< Event mask */
} ExpiryPointEventList;

/** This data type represents a schedule table expiry point */
typedef struct expiry_point_t {
  TickType offset;                /**< Expiry point offset */
  TaskType * tasks;               /**< Expiry point tasks */
  udata_word_t num_tasks;         /**< Number of tasks */
  ExpiryPointEventList * events;  /**< Expiry point events */
  udata_word_t num_events;        /**< Number of events */
#ifdef SCHEDTBL_SYNC
  TickType max_shorten;           /**< Max tick can be substracted */
  TickType max_lengthen;          /**< Max tick can be added */
#endif
} ExpiryPointType;

/** Invalid expiry point index */
#define INVALID_EP    ((udata_word_t) ~(0UL))

/** This data type represents a schedule table object */
typedef struct sched_tbl_struct_t {
  ScheduleTableType id;           /**< Schedule table ID */
  struct counter_t * counter;     /**< Counter object */
  ScheduleTableStatusType status; /**< Schedule table status */
  FlagType flag;                  /**< Schedule table flag */
  TickType next_tick;             /**< Schedule table next event tick */
  TickType delay;                 /**< Schedule table final delay */
  ExpiryPointType * exps;         /**< Schedule table expiry points */
  udata_word_t next_exp;          /**< Next expiry points index */
  udata_word_t num_exps;          /**< Number of expiry points */
#ifdef SCHEDTBL_SYNC
  TickType precision;             /**< Precision bound */
  /* Duration is max offset + delay */
  TickType duration;              /**< Schedule table duration */
  TickType deviation;             /**< Synchronization deviation */
#endif
#ifdef MULTI_SCHEDTBL_PER_CNTR
  /**
   * @brief Next current schedule table
   *
   * All the schedule tables currently being processed for
   * a counter will be double linked with this field and
   * prev.
   */
  struct sched_tbl_struct_t * next;
  /**
   * @brief Previous current schedule table
   *
   * All the schedule tables currently being processed for
   * a counter will be double linked with this field and
   * next.
   */
  struct sched_tbl_struct_t * prev;

#endif
  /**
   * @brief Next schedule table to switch to
   *
   * This field points to the schedule table that will be
   * processed after the current schedule table. A schedule
   * table switched to SCHEDULETABLE_NEXT state will be
   * linked to a corresponding current schedule table
   * through this link.
   */
  struct sched_tbl_struct_t * to_tbl;
  /**
   * @brief Current schedule table to be switched from
   *
   * This field points to the current schedule table being
   * processed for a schedule table in the SCHEDULETABLE_NEXT
   * status. It should be ignored for schedule table in any
   * other status.
   */
  struct sched_tbl_struct_t * from_tbl;
} ScheduleTableStructType;

/** This data type represents schedule table auto start mode */
typedef enum sched_tbl_start_mode {
  SCHEDTBL_STARTMODE_ABSOLUTE,    /**< Absolute start mode */
  SCHEDTBL_STARTMODE_RELATIVE,    /**< Relative start mode */
  SCHEDTBL_STARTMODE_SYNCHRON     /**< Sychronized start mode */
} ScheduleTableStartModeType;

/** This data type represents auto start schedule table entry */
typedef struct sched_tbl_autostart_t {
  ScheduleTableType id;             /**< Schedule table ID */
  ScheduleTableStartModeType mode;  /**< Auto start mode */
  TickType value;                   /**< Auto start value */
  AppModeType appmode;              /**< AppMode */
} ScheduleTableAutoStartType;

/** Flag for counter overflow */
#define COUNTER_STATUS_OVF     (0x1U << 0)

/** This data type represents a counter object */
typedef struct counter_t {
  TickType count;                     /**< Counter count */
  AlarmBaseType properties;           /**< Counter properties */
  FlagType status;                    /**< Counter status flag */
  AlarmQueueType * alarms;            /**< Alarm queue */
#ifdef USE_SCHEDTBL
  ScheduleTableStructType * schedtbl; /**< Schedule table(s) */
#endif
} Counter;

/**
 * @def AlarmIsActive
 * @brief Check whether an alarm is active
 * 
 * AlarmIsActive returns 0 if alarm is not in use, returns
 * 1 if alarm is in use.
 *
 * param[in] alarm
 *   Alarm ID of type AlarmType
 */
#define AlarmIsActive(alarm)                   \
  ((alarms[alarm].status & ALARM_STATUS_ACTIVE) ? 1 : 0)

/**
 * @def CouterOVF
 * @brief Retrieves the OVF bit of a counter
 *
 * Retrieves the value of OVF bit in status flag of a
 * counter.
 *
 * param[in] counter
 *   Reference to a counter object
 */
#define CounterOVF(counter)                    \
  ((counter->status & COUNTER_STATUS_OVF) ? 1 : 0)

/**
 * @def AlarmOVF
 * @brief Retrieves the OVF bit of an alarm
 *
 * Retrieves the value of OVF bit in status flag of an
 * alarm.
 *
 * param[in] alarm
 *   Alarm ID of type AlarmType
 */
#define AlarmOVF(alarm)                        \
  ((alarms[alarm].status & ALARM_STATUS_OVF) ? 1 : 0)

/**
 * @def ScheduleTableOVF
 * @brief Retrieves the OVF bit of a schedule table
 *
 * Retrieves the value of OVF bit in flag of a schedule
 * table.
 *
 * param[in] tbl
 *   Schedule table ID of type ScheduleTableType
 */
#define ScheduleTableOVF(tbl)                  \
  ((schedtbls[tbl].flag & SCHEDULETABLE_OVF) ? 1 : 0)

/**
 * @def ToggleCounterOVF
 * @brief Toggles the OVF bit of a counter
 *
 * Toggles the OVF bit in the status flag of a counter.
 * The OVF bit of a counter will be toggled every time
 * its count overflows.
 *
 * param[in] counter
 *   Reference to a counter object
 */
#define ToggleCounterOVF(counter)                            \
  ((counter->status & COUNTER_STATUS_OVF) ?                  \
  (counter->status &= (~((FlagType) COUNTER_STATUS_OVF))) :  \
  (counter->status |= COUNTER_STATUS_OVF))

/**
 * @def ToggleAlarmOVF
 * @brief Toggles the OVF bit of an alarm based on its
 * counter OVF status
 *
 * Toggles the OVF bit in the status flag of an alarm
 * according to its counter OVF status.
 *
 * param[in] alarm
 *   Alarm ID of type AlarmType
 */
#define ToggleAlarmOVF(alarm)                                   \
  ((alarms[alarm].counter->status & COUNTER_STATUS_OVF) ?       \
  (alarms[alarm].status &= (~((FlagType) ALARM_STATUS_OVF))) :  \
  (alarms[alarm].status |= ALARM_STATUS_OVF))

/**
 * @def SetAlarmOVF
 * @brief Set the OVF bit of an alarm to its counter
 * OVF status
 *
 * Sets the OVF bit in the status flag of an alarm to its
 * counter OVF value.
 *
 * param[in] alarm
 *   Alarm ID of type AlarmType
 */
#define SetAlarmOVF(alarm)                                      \
  ((alarms[alarm].counter->status & COUNTER_STATUS_OVF) ?       \
  (alarms[alarm].status |= ALARM_STATUS_OVF) :                  \
  (alarms[alarm].status &= (~((FlagType) ALARM_STATUS_OVF))))   \

/**
 * @def ToggleScheduleTableOVF
 * @brief Toggles the OVF bit of a schedule table based on
 * its counter OVF status
 *
 * Toggles the OVF bit in the flag of a schedule table
 * according to its counter OVF status.
 *
 * param[in] tbl
 *   Schedule table ID of type ScheduleTableType
 */
#define ToggleScheduleTableOVF(tbl)                             \
  ((schedtbls[tbl].counter->status & COUNTER_STATUS_OVF) ?      \
  (schedtbls[tbl].flag &= (~((FlagType) SCHEDULETABLE_OVF))) :  \
  (schedtbls[tbl].flag |= SCHEDULETABLE_OVF))

/**
 * @def SetScheduleTableOVF
 * @brief Set the OVF bit of a schedule table to its counter
 * OVF status
 *
 * Sets the OVF bit in the flag of a schedule table to its
 * counter OVF value.
 *
 * param[in] tbl
 *   Schedule table ID of type ScheduleTableType
 */
#define SetScheduleTableOVF(tbl)                                \
  ((schedtbls[tbl].counter->status & COUNTER_STATUS_OVF) ?      \
  (schedtbls[tbl].flag |= SCHEDULETABLE_OVF) :                  \
  (schedtbls[tbl].flag &= (~((FlagType) SCHEDULETABLE_OVF))))   \

/**
 * @brief This service increments a software counter.
 *
 * @param[in] CounterID
 *   The counter to be incremented
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid or counter is implemented
 *   in hardware and can not be incremented by software
 */
StatusType IncrementCounter (CounterType CounterID);

/**
 * @brief
 * This service reads the current count value of a counter
 * (returning either the hardware timer ticks if counter is
 * driven by hardware or the software ticks when user drives
 * counter).
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
StatusType GetCounterValue (CounterType CounterID,
                            TickRefType Value);

/**
 * @brief
 * This service gets the number of ticks between the current
 * tick value and a previously read tick value.
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
StatusType GetElapsedValue (CounterType CounterID,
                            TickRefType Value,
                            TickRefType ElapsedValue);

/**
 * @brief Busy wait delay
 *
 * Delay for a time interval in unit of counts relative to
 * a given counter. Notice that this function can only be
 * used by tasks after the system is initialized. Driver
 * modules cannot use it for busy delay.
 *
 * @param[in] CounterID
 *   Counter specified
 * @param[in] Counts
 *   Delay counts
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) CounterID not valid
 * @retval E_OS_VALUE
 *   (Extended) The given count was not valid
 */
StatusType Delay (CounterType CounterID, TickType Counts);

#endif

/* vi: set et ai sw=2 sts=2: */
