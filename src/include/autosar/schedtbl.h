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
 * @file   src/include/autosar/schedtbl.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AUTOSAR Schedule Tables
 */
#ifndef _AUTOSAR_SCHEDTBL_H_
#define _AUTOSAR_SCHEDTBL_H_

#include <autosar/types.h>
#include <autosar/error.h>

/**
 * Constant of data type ScheduleTableType for a not
 * defined schedule table
 */
#define INVALID_SCHEDULETABLE  ((ScheduleTableType) ~(0UL))

/* Schedule table status */
/** The schedule table is not started */
#define SCHEDULETABLE_STOPPED                    0x0U
/**
 * The schedule table will be started after the end of
 * currently running schedule table (schedule table was
 * used in NextScheduleTable() service)
 */
#define SCHEDULETABLE_NEXT                       0x1U
/**
 * The schedule table uses explicit synchronization,
 * has been started and is waiting for the global time
 */
#define SCHEDULETABLE_WAITING                    0x2U
/**
 * The schedule table is running, but is currently not
 * synchronous to a global time source
 */
#define SCHEDULETABLE_RUNNING                    0x3U
/**
 * The schedule table is running and is synchronous to
 * a global time source
 */
#define SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS    0x4U

/**
 * @brief
 * This service starts the processing of a schedule table
 * at "Offset" relative to the "Now" value on the underlying
 * counter.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 * @param[in] Offset
 *   Number of ticks on the counter before the the schedule
 *   table processing is started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_VALUE
 *   (Extended) Offset is greater than
 *   (OsCounterMaxAllowedValue - InitialOffset) or is
 *   equal to 0
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */
StatusType StartScheduleTableRel (ScheduleTableType ScheduleTableID,
                                  TickType Offset);

/**
 * @brief
 * This service starts the processing of a schedule table at
 * an absolute value "Start" on the underlying counter.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 * @param[in] Start
 *   Absolute counter tick value at which the schedule table
 *   is started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_VALUE
 *   (Extended) Start is greater than OsCounterMaxAllowedValue
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */
StatusType StartScheduleTableAbs (ScheduleTableType ScheduleTableID,
                                  TickType Start);

/**
 * @brief
 * This service cancels the processing of a schedule table
 * immediately at any point while the schedule table is
 * running.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be stopped
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_NOFUNC
 *   (Standard) Schedule table was already stopped
 */
StatusType StopScheduleTable (ScheduleTableType ScheduleTableID);

/**
 * @brief
 * This service switches the processing from one schedule
 * table to another schedule table.
 *
 * @param[in] ScheduleTableID_From
 *   Currently processed schedule table
 * @param[in] ScheduleTableID_To
 *   Schedule table that provides its series of expiry points
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID_From or ScheduleTableID_To
 *   not valid
 * @retval E_OS_NOFUNC
 *   (Standard) ScheduleTableID_From not started
 * @retval E_OS_STATE
 *   (Standard) ScheduleTableID_To is started or next
 */
StatusType NextScheduleTable (
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To);

/**
 * @brief
 * This service starts an explicitly synchronized schedule
 * table synchronously.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be started
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 * @retval E_OS_STATE
 *   (Standard) Schedule table was already started
 */
StatusType StartScheduleTableSynchron (
  ScheduleTableType ScheduleTableID);

/**
 * @brief
 * This service provides the schedule table with a
 * synchronization count and start synchronization.
 *
 * @param[in] ScheduleTableID
 *   Schedule table to be synchronized
 * @param[in] Value
 *   The current value of the synchronization counter
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) The ScheduleTableID was not valid or
 *   schedule table can not be synchronized
 *   (OsScheduleTblSyncStrategy not set or
 *   OsScheduleTblSyncStrategy = IMPLICIT)
 * @retval E_OS_VALUE
 *   (Extended) The <Value> is out of range
 * @retval E_OS_STATE
 *   (Standard) The state of schedule table <ScheduleTableID>
 *   is equal to SCHEDULETABLE_STOPPED
 */
StatusType SyncScheduleTable (ScheduleTableType ScheduleTableID,
                              TickType Value);

/**
 * @brief
 * This service stops synchronization of a schedule table.
 *
 * @param[in] ScheduleTableID
 *   Schedule table whose sychonization is to be stopped
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 */
StatusType SetScheduleTableAsync (
  ScheduleTableType ScheduleTableID);

/**
 * @brief
 * This service queries the state of a schedule table (also
 * with respect to synchronization).
 *
 * @param[in] ScheduleTableID
 *   Schedule table for which status is requested
 * @param[out] ScheduleStatus
 *   Reference to ScheduleTableStatusType
 *
 * @retval E_OK
 *   (Standard) No error
 * @retval E_OS_ID
 *   (Extended) ScheduleTableID not valid
 */
StatusType GetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus);

#endif

/* vi: set et ai sw=2 sts=2: */
