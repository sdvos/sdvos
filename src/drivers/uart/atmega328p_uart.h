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
 * @file   drivers/usart/atmega328p_usart.h
 * @author Ye Li (liye@sdvos.org)
 * @brief
 * USART Driver header for ATmega48A/PA/88A/PA/168A/PA/328/P
 */
#ifndef _ATMEGA328P_USART_H_
#define _ATMEGA328P_USART_H_

#include <arch/avr5/types.h>

/**
 * @def _TO_DATA
 * @brief Data address for C code.
 */
#define _TO_DATA(x) (*((volatile uint8_t *) (x)))

/**
 * @def _TO_DATA16
 * @brief 16-bit Data address for C code.
 */
#define _TO_DATA16(x) (*((volatile uint16_t *) (x)))

#define UCSR0A _TO_DATA(0xC0)
#define MPCM0 0
#define U2X0 1
#define UPE0 2
#define DOR0 3
#define FE0 4
#define UDRE0 5
#define TXC0 6
#define RXC0 7

#define UCSR0B _TO_DATA(0xC1)
#define TXB80 0
#define RXB80 1
#define UCSZ02 2
#define TXEN0 3
#define RXEN0 4
#define UDRIE0 5
#define TXCIE0 6
#define RXCIE0 7

#define UCSR0C _TO_DATA(0xC2)
#define UCPOL0 0
#define UCSZ00 1
#define UCPHA0 1
#define UCSZ01 2
#define UDORD0 2
#define USBS0 3
#define UPM00 4
#define UPM01 5
#define UMSEL00 6
#define UMSEL01 7

#define UBRR0 _TO_DATA16(0xC4)
#define UBRR0L _TO_DATA(0xC4)
#define UBRR0H _TO_DATA(0xC5)

#define UDR0 _TO_DATA(0xC6)
#define UDR0_0 0
#define UDR0_1 1
#define UDR0_2 2
#define UDR0_3 3
#define UDR0_4 4
#define UDR0_5 5
#define UDR0_6 6
#define UDR0_7 7

#endif

/* vi: set et ai sw=2 sts=2: */
