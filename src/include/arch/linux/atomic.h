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
 * @file   src/include/arch/linux/atomic.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Atomic Operations
 */
#ifndef _LINUX_ATOMIC_H_
#define _LINUX_ATOMIC_H_

#include <arch/linux/types.h>
#include <cc.h>

/* Signed atomic data type */
typedef int32_t ALIGNED (4) atomic_t;
/* Unsigned atomic data type */
typedef uint32_t ALIGNED (4) uatomic_t;

/**
 * @def ATOMIC_INIT
 * @brief Atomic data static initializer
 *
 * @param[in] i
 *   Initial value
 */
#define ATOMIC_INIT(i) (i)

/**
 * @def atomic_inc(v)
 * @brief Atomic increase
 *
 * Atomically increase the integer stored at v.
 *
 * @param[in] v
 *   Reference to an atomic type data
 */
#define atomic_inc(v)                           \
  __asm__ volatile ("lock; incl %0\n\t"         \
                    :"+m" (*(v)))

/**
 * @def uatomic_inc(v)
 * @brief Unsigned atomic increase
 *
 * Atomically increase the unsigned value stored at v.
 *
 * @param[in] v
 *   Reference to an uatomic type data
 */
#define uatomic_inc atomic_inc

/**
 * @def atomic_dec(v)
 * @brief Atomic decrease
 *
 * Atomically decrease the integer stored at v.
 *
 * @param[in] v
 *   Reference to an atomic type data
 */
#define atomic_dec(v)                           \
  __asm__ volatile ("lock; decl %0\n\t"         \
                    :"+m" (*(v)))

/**
 * @def uatomic_dec(v)
 * @brief Unsigned atomic decrease
 *
 * Atomically decrease the unsigned value stored at v.
 *
 * @param[in] v
 *   Reference to an uatomic type data
 */
#define uatomic_dec atomic_dec

/**
 * @def atomic_read
 * @brief Atomic read
 *
 * Atomically read an atomic type data value
 *
 * @param[in] v
 *   Reference to an atomic type data
 */
#define atomic_read(v)                          \
  (*((volatile int32_t *)(v)))

/**
 * @def uatomic_read
 * @brief Unsigned atomic read
 *
 * Atomically read an unsigned atomic type data value
 *
 * @param[in] v
 *   Reference to an atomic type data
 */
#define uatomic_read(v)                         \
  (*((volatile uint32_t *)(v)))

/**
 * @def atomic_set
 * @brief Atomic set
 *
 * Atomically set an atomic type data value
 *
 * @param[in] v
 *   Reference to an atomic type data
 * @param[in] i
 *   Value to be set to
 */
#define atomic_set(v, i) (*(v) = (i))

/**
 * @def uatomic_set
 * @brief Unsigned atomic set
 *
 * Atomically set an uatomic type data value
 *
 * @param[in] v
 *   Reference to an uatomic type data
 * @param[in] i
 *   Value to be set to
 */
#define uatomic_set atomic_set

#endif

/* vi: set et ai sw=2 sts=2: */
