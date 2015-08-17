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
 * @file   src/arch/linux/panic.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux panic
 */
#include <arch/linux/utils.h>
#include <arch/linux/types.h>
#include <debug.h>
#include <stdlib.h>

void
panic ()
{
  DEBUG_PRINTF ("SDVOS Panic\n");
  exit (1);
}

/* vi: set et ai sw=2 sts=2: */
