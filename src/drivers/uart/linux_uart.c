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
 * @file   src/drivers/uart/linux_uart.c
 * @author Ye Li (liye@sdvos.org)
 * @brief UART Driver for Linux
 */
#include <sdvos_printf.h>
#include <stdio.h>

int
uart_getchar ()
{
  int c = 0;

  do {
    c = getchar ();
  } while (c == -1);

  return c;
}

int
uart_putchar (char c)
{
  int ret = putchar (c);
  fflush (stdout);
  return ret;
}

void linux_uart_init (void)
{
  sdvos_init_printf ((void (*) (char)) putchar);
}

/* vi: set et ai sw=2 sts=2: */
