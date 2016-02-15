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
 * @file   src/arch/armv7m/idle.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Architectural Dependant Idle Loop
 */
#include <arch/armv7m/barrier.h>

/**
 * @brief Architectural dependant idle loop
 *
 * WFI instruction is used to put system into sleep.
 * We do not use deep sleep since SYSTICK will stop in this
 * mode. See InterruptInit () for initialization.
 */
void
IdleLoop ()
{
#ifdef __IDLE_WFI__
  DSB ();
  while (1) __asm__ volatile ("wfi\n\t");
#else
  while (1);
#endif
}

/* vi: set et ai sw=2 sts=2: */
