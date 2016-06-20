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
 * @file   src/include/osek/types.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Type Definitions
 */
#ifndef _OSEK_TYPES_H_
#define _OSEK_TYPES_H_

#ifdef __ARCH_I386__
#include <arch/i386/types.h>
#include <arch/i386/atomic.h>
#elif defined __ARCH_AVR5__
#include <arch/avr5/types.h>
#include <arch/avr5/atomic.h>
#elif defined __ARCH_ARMV7M__
#include <arch/armv7m/types.h>
#include <arch/armv7m/atomic.h>
#elif defined __ARCH_LINUX__
#include <arch/linux/types.h>
#include <arch/linux/atomic.h>
#else
/* Use i386 simulation as default */
#include <arch/i386/types.h>
#include <arch/i386/atomic.h>
#endif

/* Auto generated configuration header */
#include <config/config.h>

/** This data type represents OSEK service return status */
typedef udata_word_t StatusType;

/* Task Management Data Types */
/** This data type identifies a task */
typedef udata_word_t TaskType;
/** This data type points to a variable of TaskType */
typedef TaskType* TaskRefType;
/** This data type identifies the state of a task */
typedef udata_word_t TaskStateType;
/** This data type points to a variable of TaskStateRefType */
typedef TaskStateType* TaskStateRefType;

/* Interrupt Management */
/**
 * This data type identifies an interrupt API call counter
 * or nested interrupt level counter
 */
typedef udata_word_t IntCntType;

/* OS Execution Control Types */
/** This data type represents the application mode */
typedef udata_word_t AppModeType;

/* Resource Management */
/** Internal data type for a resource */
struct resource_t {
  PrioType cprio;            /**< Ceiling priority */
#ifdef OSEK_EXTENDED
  bool occupied;             /**< Resource occupation */
#endif
  struct resource_t * next;  /**< Next resource */
};

/** Data type for a resource */
typedef struct resource_t Resource;

/** Data type of the event mask */
typedef event_mask_t EventMaskType;

/** Reference to an event mask */
typedef EventMaskType * EventMaskRefType;

/* Alarm/Counter Management */
/** This data type represents count value in ticks */
typedef uatomic_t TickType;

/** This data points to the data type TickType */
typedef TickType * TickRefType;

/**
 * This data type represents a structure for storage of
 * counter characteristics.
 */
typedef struct alarm_base_t {
  TickType maxallowedvalue;  /**< Maximum allowed count value */
  TickType ticksperbase;     /**<
                              *   Number of ticks required to
                              *   reach a counter specific unit
                              */
  TickType mincycle;         /**< Smallest allowed cycle value */
} AlarmBaseType;

/** This data type points to the data type AlarmBaseType */
typedef AlarmBaseType * AlarmBaseRefType;

/** This data type represents an alarm object */
typedef alarm_t AlarmType;

/**
 * This data type represents the identification of
 * system services
 */
typedef udata_word_t OSServiceIdType;

#endif

/* vi: set et ai sw=2 sts=2: */
