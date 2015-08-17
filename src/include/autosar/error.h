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
 * @file   src/include/autosar/error.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AUTOSAR Error Values
 */
#ifndef _AUTOSAR_ERROR_H_
#define _AUTOSAR_ERROR_H_

/* Include all OSEK error codes */
#include <osek/error.h>

/* OSEK error code ends with 0x8 */
#define E_OS_SERVICEID             0x09
#define E_OS_ILLEGAL_ADDRESS       0x0A
#define E_OS_MISSINGEND            0x0B
#define E_OS_DISABLEDINT           0x0C
#define E_OS_STACKFAULT            0x0D
#define E_OS_PROTECTION_MEMORY     0x0E
#define E_OS_PROTECTION_TIME       0x0F
#define E_OS_PROTECTION_ARRIVAL    0x10
#define E_OS_PROTECTION_LOCKED     0x11
#define E_OS_PROTECTION_EXCEPTION  0x12
#define E_OS_CORE                  0x13
#define E_OS_SPINLOCK              0x14
#define E_OS_INTERFERENCE_DEADLOCK 0x15
#define E_OS_NESTING_DEADLOCK      0x16
#define E_OS_PARAM_POINTER         0x17

#endif

/* vi: set et ai sw=2 sts=2: */
