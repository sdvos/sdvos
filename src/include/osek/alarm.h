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
 * @file   src/include/osek/alarm.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Alarms
 */
#ifndef _OSEK_ALARM_H_
#define _OSEK_ALARM_H_

#include <osek/types.h>
#include <osek/error.h>
#include <config/config.h>

/** Constant of data type AlarmType for a not defined alarm */
#define INVALID_ALARM    ((AlarmType) ~(0UL))

#define ALARM_ACTION_SETEVENT      0x0U
#define ALARM_ACTION_ACTIVATETASK  0x1U
#define ALARM_ACTION_CALLBACK      0x2U

/** This data type represents types of alarm actions */
typedef udata_word_t ActionType;

/** This data type represents an alarm action */
typedef struct alarm_action_t {
  TaskType task;             /**< Task ID */
  EventMaskType event;       /**< Event mask */
  void (* callback) (void);  /**< Callback function */
  ActionType type;           /**< Action type */
} AlarmActionType;

/** Auto start alarms for each application mode */
extern AlarmType auto_alarms[][NUM_ALARMS];

/**
 * @brief
 * DeclareAlarm serves as external declaration of an alarm
 * element.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] AlarmIdentifier
 *   Alarm identifier (C-identifier)
 */
#define DeclareAlarm(AlarmIdentifier)

/**
 * @brief
 * The system service GetAlarmBase reads the alarm base
 * characteristics. The return value <info> is a structure
 * in which the information of data type AlarmBaseType is
 * stored.
 *
 * Allowed on task level, ISR, and in several hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType GetAlarmBase (AlarmType alarm, AlarmBaseRefType info);

/**
 * @brief
 * The system service GetAlarm returns the relative value in
 * ticks before the alarm <alarm> expires.
 *
 * It is up to the application to decide whether for example
 * a CancelAlarm may still be useful. If <alarm> is not in
 * use, <tick> is not defined. Allowed on task level, ISR,
 * and in several hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType GetAlarm (AlarmType alarm, TickRefType tick);

/**
 * @brief
 * The system service occupies the alarm <alarm> element.
 * After <inc> ticks have elapsed, the task assigned to the
 * alarm <alarm> is activated or the assigned event (only for
 * extended tasks) is set or the alarm-callback routine is
 * called.
 *
 * The behaviour of <inc> equal to 0 is up to the
 * implementation. If the relative value <inc> is very small,
 * the alarm may expire, and the task may become ready or the
 * alarm-callback may be called before the system service
 * returns to the user. If <cycle> is unequal zero, the alarm
 * element is logged on again immediately after expiry with
 * the relative value <cycle>.
 *
 * The alarm <alarm> must not already be in use. To change
 * values of alarms already in use the alarm shall be
 * cancelled first. If the alarm is already in use, this
 * call will be ignored and the error E_OS_STATE is returned.
 *
 * Allowed on task level and in ISR, but not in hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2
 *
 * AUTOSAR [SWS_Os_00304]: SetRelAlarm shall return E_OS_VALUE
 * in standart and extended status if increment is set to 0.
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
StatusType SetRelAlarm (AlarmType alarm, TickType inc,
                        TickType cycle);

/**
 * @brief
 * The system service occupies the alarm <alarm> element.
 * When <start> ticks are reached, the task assigned to the
 * alarm <alarm> is activated or the assigned event (only
 * for extended tasks) is set or the alarm-callback routine
 * is called.
 *
 * If the absolute value <start> is very close to the current
 * counter value, the alarm may expire, and the task may
 * become ready or the alarm-callback may be called before
 * the system service returns to the user.
 *
 * If the absolute value <start> already was reached before
 * the system call, the alarm shall only expire when the
 * absolute value <start> is reached again, i.e. after the
 * next overrun of the counter.
 *
 * If <cycle> is unequal zero, the alarm element is logged
 * on again immediately after expiry with the relative value
 * <cycle>.
 *
 * The alarm <alarm> shall not already be in use. To change
 * values of alarms already in use the alarm shall be
 * cancelled first. If the alarm is already in use, this
 * call will be ignored and the error E_OS_STATE is returned.
 *
 * Allowed on task level and in ISR, but not in hook
 * routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2
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
StatusType SetAbsAlarm (AlarmType alarm, TickType start,
                        TickType cycle);

/**
 * @brief
 * The system service cancels the alarm <alarm>.
 *
 * Allowed on task level and in ISR, but not in hook
 * routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
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
StatusType CancelAlarm (AlarmType alarm);

#endif

/* vi: set et ai sw=2 sts=2: */
