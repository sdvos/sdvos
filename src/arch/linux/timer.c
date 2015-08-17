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
 * @file   src/arch/linux/timer.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Timer/Counter
 */
#include <arch/linux/vector.h>
#include <arch/linux/interrupt.h>
#include <sdvos.h>
#include <signal.h>
#include <time.h>
#include <features.h>

void
ArchTimerInit ()
{
#ifdef _POSIX_C_SOURCE
  if (_POSIX_C_SOURCE >= 199309L) {
    timer_t timer_id;
    struct itimerspec its;

    timer_create (CLOCK_MONOTONIC, NULL, &timer_id);
    /* 1 ms initial delay */
    its.it_value.tv_sec = 0;
    its.it_value.tv_nsec = 1000000L;
    its.it_interval.tv_sec = 0;
    /* 1 ms interval */
    its.it_interval.tv_nsec = 1000000L;
    timer_settime (timer_id, 0, &its, NULL);
  } else {
    DEBUG_PRINTF ("POSIX per-process timer not supported!\n");
    panic ();
  }
#else
  DEBUG_PRINTF ("POSIX per-process timer not supported!\n");
  panic ();
#endif
}

/* vi: set et ai sw=2 sts=2: */
