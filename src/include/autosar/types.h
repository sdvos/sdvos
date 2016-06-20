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
 * @file   src/include/autosar/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AUTOSAR Type Definitions
 */
#ifndef _AUTOSAR_TYPES_H_
#define _AUTOSAR_TYPES_H_

/** First include all OSEK types */
#include <osek/types.h>

/* AUTOSAR Schedule Table */
/** This data type identifies a schedule table */
typedef sched_tbl_t ScheduleTableType;
/** This type describes the status of a schedule */
typedef FlagType ScheduleTableStatusType;

/**
 * This data type points to a variable of the data type
 * ScheduleTableStatusType
 */
typedef ScheduleTableStatusType* ScheduleTableStatusRefType;

#endif

/* vi: set et ai sw=2 sts=2: */
