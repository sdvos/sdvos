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
 * @file   src/arch/avr5/idle.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Architectural Dependant Idle Loop
 */
#include <arch/avr5/io.h>

/**
 * @brief Architectural dependant idle loop
 *
 * For AVR5, we want to put the processor into sleep mode.
 * This will not affect any of the timers we use for alarm.
 */
void
IdleLoop ()
{
  while (1) {
    /*
     * Set SM0...3 to 0 and SE to 1 in SMCR. With a sleep
     * instruction, we will enter idle mode in this setting.
     */
    SMCR = (1 << SE);
    /* Now, enter idle mode. */
    __asm__ volatile ("sleep");
  }
}

/* vi: set et ai sw=2 sts=2: */
