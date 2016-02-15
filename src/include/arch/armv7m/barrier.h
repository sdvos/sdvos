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
 * @file   src/include/arch/armv7m/barrier.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Memory Barriers
 */
#ifndef _ARMV7M_BARRIER_H_
#define _ARMV7M_BARRIER_H_

/** Data Synchronization Barrier */
#define DSB()                                \
  __asm__ volatile ("dsb\n\t")

/** Instruction Synchronization Barrier */
#define ISB()                                \
  __asm__ volatile ("isb\n\t")

/** Data Memory Barrier */
#define DMB()                                \
  __asm__ volatile ("dmb\n\t")

#endif

/* vi: set et ai sw=2 sts=2: */
