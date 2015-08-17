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
 * @file   src/include/debug.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Debug Utilities.
 */
#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG_SDVOS

/* With debug */
#include <sdvos_printf.h>
#define DEBUG_PRINTF sdvos_printf

#ifdef DEBUG_SDVOS_VERBOSE
#define DEBUG_PRINTFV sdvos_printf
#else
#define DEBUG_PRINTFV(fmts,...) ;
#endif

/**
 * @brief Print all tasks in priority queue.
 *
 * Print all tasks in priority queue for debugging
 * purpose.
 */
extern void dump_prio_queue (void);

#ifdef PRINT_BANNER
/**
 * @brief Print an SDVOS banner.
 *
 * A show case for shell only. Wastes memory.
 */
extern void PrintBanner (void);
#endif

#else

/* No debug */
#define DEBUG_PRINTF(fmts,...) ;
#define DEBUG_PRINTFV(fmts,...) ;
#endif

#endif

/* vi: set et ai sw=2 sts=2: */
