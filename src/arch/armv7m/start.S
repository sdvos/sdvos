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

/* Using unified format */
.syntax unified

.global _start

.section .text

/* This makes the next symbol a thumb encoded function. */
.thumb_func
_start:
  /* We should have a working stack at this point. */
  /* No context needs to be saved. */
  /* Initialize data section */
  ldr r0, =virt_start_of_data
  ldr r1, =start_of_data
  ldr r2, =length_of_data
  bl sdvos_memcpy
  /* Initialize bss section */
  ldr r0, =virt_start_of_bss
  movs r1, #0
  ldr r2, =length_of_bss
  bl sdvos_memset
  /* Now, jump to main. */
  b main

/* vi: set et ai sw=2 sts=2: */
