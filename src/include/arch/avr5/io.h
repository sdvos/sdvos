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
 * @file   src/include/arch/avr5/io.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 I/O Ports
 */
#ifndef _AVR5_IO_H_
#define _AVR5_IO_H_

/*
 * IO Macros used in C for Atmega328p. Do not include if
 * avr/io.h in avr-libc is used.
 */

#include <arch/avr5/types.h>

/**
 * @def _IO_TO_DATA
 * @brief Convert I/O address to data address for C code.
 */
#define _IO_TO_DATA(x) (*((volatile uint8_t *) (x + 0x20)))
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

#define MCUCR _IO_TO_DATA(0x35)
#define IVCE 0
#define IVSEL 1
#define PUD 4
#define BODSE 5
#define BODS 6

#define EICRA _TO_DATA(0x69)
#define ISC00 0
#define ISC01 1
#define ISC10 2
#define ISC11 3

#define EIMSK _IO_TO_DATA(0x1D)
#define INT0 0
#define INT1 1

#define EIFR _IO_TO_DATA(0x1C)
#define INTF0 0
#define INTF1 1

#define PCICR _TO_DATA(0x68)
#define PCIE0 0
#define PCIE1 1
#define PCIE2 2

#define PCIFR _IO_TO_DATA(0x1B)
#define PCIF0 0
#define PCIF1 1
#define PCIF2 2

#define PCMSK0 _TO_DATA(0x6B)
#define PCINT0 0
#define PCINT1 1
#define PCINT2 2
#define PCINT3 3
#define PCINT4 4
#define PCINT5 5
#define PCINT6 6
#define PCINT7 7

#define PCMSK1 _TO_DATA(0x6C)
#define PCINT8 0
#define PCINT9 1
#define PCINT10 2
#define PCINT11 3
#define PCINT12 4
#define PCINT13 5
#define PCINT14 6

#define PCMSK2 _TO_DATA(0x6D)
#define PCINT16 0
#define PCINT17 1
#define PCINT18 2
#define PCINT19 3
#define PCINT20 4
#define PCINT21 5
#define PCINT22 6
#define PCINT23 7

#define TIFR0 _IO_TO_DATA(0x15)
#define TOV0 0
#define OCF0A 1
#define OCF0B 2

#define TIFR1 _IO_TO_DATA(0x16)
#define TOV1 0
#define OCF1A 1
#define OCF1B 2
#define ICF1 5

#define TCCR0A _IO_TO_DATA(0x24)
#define WGM00 0
#define WGM01 1
#define COM0B0 4
#define COM0B1 5
#define COM0A0 6
#define COM0A1 7

#define TCCR0B _IO_TO_DATA(0x25)
#define CS00 0
#define CS01 1
#define CS02 2
#define WGM02 3
#define FOC0B 6
#define FOC0A 7

#define TCNT0 _IO_TO_DATA(0x26)
#define TCNT0_0 0
#define TCNT0_1 1
#define TCNT0_2 2
#define TCNT0_3 3
#define TCNT0_4 4
#define TCNT0_5 5
#define TCNT0_6 6
#define TCNT0_7 7

#define OCR0A _IO_TO_DATA(0x27)
#define OCR0A_0 0
#define OCR0A_1 1
#define OCR0A_2 2
#define OCR0A_3 3
#define OCR0A_4 4
#define OCR0A_5 5
#define OCR0A_6 6
#define OCR0A_7 7

#define OCR0B _IO_TO_DATA(0x28)
#define OCR0B_0 0
#define OCR0B_1 1
#define OCR0B_2 2
#define OCR0B_3 3
#define OCR0B_4 4
#define OCR0B_5 5
#define OCR0B_6 6
#define OCR0B_7 7

#define SMCR _IO_TO_DATA(0x33)
#define SE   0
#define SM0  1
#define SM1  2
#define SM2  3

#define TIMSK0 _TO_DATA(0x6E)
#define TOIE0 0
#define OCIE0A 1
#define OCIE0B 2

#define TIMSK1 _TO_DATA(0x6F)
#define TOIE1 0
#define OCIE1A 1
#define OCIE1B 2
#define ICIE1 5

#define TCCR1A _TO_DATA(0x80)
#define WGM10 0
#define WGM11 1
#define COM1B0 4
#define COM1B1 5
#define COM1A0 6
#define COM1A1 7

#define TCCR1B _TO_DATA(0x81)
#define CS10 0
#define CS11 1
#define CS12 2
#define WGM12 3
#define WGM13 4
#define ICES1 6
#define ICNC1 7

#define TCCR1C _TO_DATA(0x82)
#define FOC1B 6
#define FOC1A 7

#define TCNT1 _TO_DATA16(0x84)

#define TCNT1L _TO_DATA(0x84)
#define TCNT1L0 0
#define TCNT1L1 1
#define TCNT1L2 2
#define TCNT1L3 3
#define TCNT1L4 4
#define TCNT1L5 5
#define TCNT1L6 6
#define TCNT1L7 7

#define TCNT1H _TO_DATA(0x85)
#define TCNT1H0 0
#define TCNT1H1 1
#define TCNT1H2 2
#define TCNT1H3 3
#define TCNT1H4 4
#define TCNT1H5 5
#define TCNT1H6 6
#define TCNT1H7 7

#define ICR1 _TO_DATA16(0x86)

#define ICR1L _TO_DATA(0x86)
#define ICR1L0 0
#define ICR1L1 1
#define ICR1L2 2
#define ICR1L3 3
#define ICR1L4 4
#define ICR1L5 5
#define ICR1L6 6
#define ICR1L7 7

#define ICR1H _TO_DATA(0x87)
#define ICR1H0 0
#define ICR1H1 1
#define ICR1H2 2
#define ICR1H3 3
#define ICR1H4 4
#define ICR1H5 5
#define ICR1H6 6
#define ICR1H7 7

#define OCR1A _TO_DATA16(0x88)

#define OCR1AL _TO_DATA(0x88)
#define OCR1AL0 0
#define OCR1AL1 1
#define OCR1AL2 2
#define OCR1AL3 3
#define OCR1AL4 4
#define OCR1AL5 5
#define OCR1AL6 6
#define OCR1AL7 7

#define OCR1AH _TO_DATA(0x89)
#define OCR1AH0 0
#define OCR1AH1 1
#define OCR1AH2 2
#define OCR1AH3 3
#define OCR1AH4 4
#define OCR1AH5 5
#define OCR1AH6 6
#define OCR1AH7 7

#define OCR1B _TO_DATA16(0x8A)

#define OCR1BL _TO_DATA(0x8A)
#define OCR1BL0 0
#define OCR1BL1 1
#define OCR1BL2 2
#define OCR1BL3 3
#define OCR1BL4 4
#define OCR1BL5 5
#define OCR1BL6 6
#define OCR1BL7 7

#define OCR1BH _TO_DATA(0x8B)
#define OCR1BH0 0
#define OCR1BH1 1
#define OCR1BH2 2
#define OCR1BH3 3
#define OCR1BH4 4
#define OCR1BH5 5
#define OCR1BH6 6
#define OCR1BH7 7

#endif

/* vi: set et ai sw=2 sts=2: */
