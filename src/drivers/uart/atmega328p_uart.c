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
 * @file   src/drivers/uart/atmega328p_uart.c
 * @author Ye Li (liye@sdvos.org)
 * @brief UART Driver for ATmega48A/PA/88A/PA/168A/PA/328/P
 */
#include "atmega328p_uart.h"
#include <sdvos_printf.h>

#define BAUD 38400
#define BAUD_TOL 2

#define UBRR (((F_CPU) + 8UL * (BAUD)) / (16UL * (BAUD)) -1UL)

#if 100 * (F_CPU) > \
  (16 * ((UBRR) + 1)) * (100 * (BAUD) + (BAUD) * (BAUD_TOL))
#define USE_2X
#elif 100 * (F_CPU) < \
  (16 * ((UBRR) + 1)) * (100 * (BAUD) - (BAUD) * (BAUD_TOL))
#define USE_2X
#else
#undef USE_2X
#endif

#if defined(USE_2X)
#undef UBRR
#define UBRR (((F_CPU) + 4UL * (BAUD)) / (8UL * (BAUD)) -1UL)
#endif

int
uart_putchar (char c)
{
  if (c == '\n') {
    uart_putchar('\r');
  }
  /* Wait until UDRE0 is set */
  while (!(UCSR0A & (1 << UDRE0)));
  UDR0 = c;
  return (unsigned char) c;
}

int
uart_getchar ()
{
  /* Wait until data exists. */
  while (!(UCSR0A & (1 << RXC0)));
  return UDR0;
}

void
atmega328p_uart_init (void)
{
  UBRR0H = (UBRR >> 8);
  UBRR0L = (UBRR & 0xFF);

#if defined(USE_2X)
  UCSR0A |= (1 << U2X0);
#else
  UCSR0A &= ~(1 << U2X0);
#endif

  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); /* 8-bit data */ 
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);   /* Enable RX and TX */

  sdvos_init_printf ((void (*) (char)) uart_putchar);
}

/* vi: set et ai sw=2 sts=2: */
