/*
 *         Standard Dependable Vehicle Operating System
 *
 * Copyright (C) 2016 Ye Li (liye@sdvos.org)
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
 * @file   src/include/arch/armv7m/cache.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Cache Management
 */
#ifndef _ARMV7M_CACHE_H_
#define _ARMV7M_CACHE_H_

#include <arch/armv7m/scs.h>

/** Number of ways in cache */
#define CCSIDR_WAYS()                                \
    ((SCB->CCSIDR & SCB_CCSIDR_ASSOCIATIVITY) >> 3U)

/** Number of sets in cache */
#define CCSIDR_SETS()                                \
    ((SCB->CCSIDR & SCB_CCSIDR_NUMSETS) >> 13U)

/**
 * @brief Enable instruction cache.
 *
 * @return
 *   None
 */
void SCB_EnableICache (void);

/**
 * @brief Enable data cache.
 *
 * @return
 *   None
 */
void SCB_EnableDCache (void);

/**
 * @brief Disable instruction cache.
 *
 * @return
 *   None
 */
void SCB_DisableICache (void);

/**
 * @brief Disable data cache.
 *
 * @return
 *   None
 */
void SCB_DisableDCache (void);

#endif

/* vi: set et ai sw=2 sts=2: */
