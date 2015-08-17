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
 * @file   src/arch/linux/interrupt.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Interrupt Management
 */
#include <arch/linux/interrupt.h>

void
InterruptInit ()
{
  /* Nothing to do */
}

ISR (SIGALRM)
{
  TickHandler ();
}

/* vi: set et ai sw=2 sts=2: */
