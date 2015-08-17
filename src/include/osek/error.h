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
 * @file   src/include/osek/error.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Error Values
 */
#ifndef _OSEK_ERROR_H_
#define _OSEK_ERROR_H_

/* Normal return value */
#define E_OK             0x0

/* Errors of API services */
#define E_OS_ACCESS      0x1
#define E_OS_CALLEVEL    0x2
#define E_OS_ID          0x3
#define E_OS_LIMIT       0x4
#define E_OS_NOFUNC      0x5
#define E_OS_RESOURCE    0x6
#define E_OS_STATE       0x7
#define E_OS_VALUE       0x8

#endif

/* vi: set et ai sw=2 sts=2: */
