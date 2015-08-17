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
 * @file   src/include/assert.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  C Static and Runtime Assert
 */
#ifndef _ASSERT_H_
#define _ASSERT_H_

#include <sdvos_printf.h>

/**
 * @def CASSERT(p, s)
 * @brief C Static Assert
 *
 * C static assert for compile time assertion. When
 * predicate is true, no run time overhead will be
 * added. If predicate is false, compilation error
 * will be triggered.
 *
 * @param[in] p
 *   Predicate
 * @param[in] s
 *   String or message for debugging
 */
#define CASSERT(p, s) __CASSERT_LINE(p,__LINE__,s)

/**
 * @def __PASTE(a,b)
 * @brief Pastes two strings together
 */
#define __PASTE(a,b) a##b

/**
 * @def __CASSERT_LINE(p, line, s)
 * @brief A typdef that triggers the compilation error
 */
#define __CASSERT_LINE(p, line, s) \
    typedef char __PASTE(assertion_failed_on_##s##_,line)[2*!!(p)-1];

/**
 * @def ASSERT(a)
 * @brief C Runtime Assert
 *
 * C runtime assertion. Prints file name and line number.
 *
 * @param[in]
 *   Predicate
 */
#define ASSERT(p)  do {                                             \
  if ((p) == 0) {                                                   \
    sdvos_printf ("Assertion failed: %s:%d\n", __FILE__, __LINE__); \
    panic ();                                                       \
  }                                                                 \
} while (0)

#endif

/* vi: set et ai sw=2 sts=2: */
