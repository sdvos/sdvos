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
 * @file   src/include/arch/avr5/vector.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Interrupt Vectors
 */
#ifndef _AVR5_VECTOR_H_
#define _AVR5_VECTOR_H_

/*
 * Vector definitions for Atmega328p. Do not include if
 * avr/interrupt.h in avr-libc is used.
 */

/* AVR5 has 26 interrupt vectors */
#define NUM_IVT_ENTRY    0x1A

#define _VECTOR(N) __vector_ ## N

#define INT0_vect_num     1
/** @brief External Interrupt Request 0 Vector */
#define INT0_vect         _VECTOR(1)

#define INT1_vect_num     2
/** @brief External Interrupt Request 1 Vector */
#define INT1_vect         _VECTOR(2)

#define PCINT0_vect_num   3
/** @brief Pin Change Interrupt Request 0 Vector */
#define PCINT0_vect       _VECTOR(3)

#define PCINT1_vect_num   4
/** @brief Pin Change Interrupt Request 1 Vector */
#define PCINT1_vect       _VECTOR(4)

#define PCINT2_vect_num   5
/** @brief Pin Change Interrupt Request 2 Vector */
#define PCINT2_vect       _VECTOR(5)

#define WDT_vect_num      6
/** @brief Watchdog Time-out Interrupt Vector */
#define WDT_vect          _VECTOR(6)

#define TIMER2_COMPA_vect_num 7
/** @brief Timer/Counter2 Compare Match A Vector */
#define TIMER2_COMPA_vect _VECTOR(7)

#define TIMER2_COMPB_vect_num 8
/** @brief Timer/Counter2 Compare Match B Vector */
#define TIMER2_COMPB_vect _VECTOR(8)

#define TIMER2_OVF_vect_num   9
/** @brief Timer/Counter2 Overflow Vector */
#define TIMER2_OVF_vect   _VECTOR(9)

#define TIMER1_CAPT_vect_num  10
/** @brief Timer/Counter1 Capture Event Vector */
#define TIMER1_CAPT_vect  _VECTOR(10)

#define TIMER1_COMPA_vect_num 11
/** @brief Timer/Counter1 Compare Match A Vector */
#define TIMER1_COMPA_vect _VECTOR(11)

#define TIMER1_COMPB_vect_num 12
/** @brief Timer/Counter1 Compare Match B Vector */
#define TIMER1_COMPB_vect _VECTOR(12)

#define TIMER1_OVF_vect_num   13
/** @brief Timer/Counter1 Overflow Vector */
#define TIMER1_OVF_vect   _VECTOR(13)

#define TIMER0_COMPA_vect_num 14
/** @brief TimerCounter0 Compare Match A Vector */
#define TIMER0_COMPA_vect _VECTOR(14)

#define TIMER0_COMPB_vect_num 15
/** @brief TimerCounter0 Compare Match B Vector */
#define TIMER0_COMPB_vect _VECTOR(15)

#define TIMER0_OVF_vect_num  16
/** @brief Timer/Couner0 Overflow Vector */
#define TIMER0_OVF_vect   _VECTOR(16)

#define SPI_STC_vect_num  17
/** @brief SPI Serial Transfer Complete Vector */
#define SPI_STC_vect      _VECTOR(17)

#define USART_RX_vect_num 18
/** @brief USART Rx Complete Vector */
#define USART_RX_vect     _VECTOR(18)

#define USART_UDRE_vect_num   19
/** @brief USART, Data Register Empty Vector */
#define USART_UDRE_vect   _VECTOR(19)

#define USART_TX_vect_num 20
/** @brief USART Tx Complete Vector */
#define USART_TX_vect     _VECTOR(20)

#define ADC_vect_num      21
/** @brief ADC Conversion Complete Vector */
#define ADC_vect          _VECTOR(21)

#define EE_READY_vect_num 22
/** @brief EEPROM Ready Vector */
#define EE_READY_vect     _VECTOR(22)

#define ANALOG_COMP_vect_num  23
/** @brief Analog Comparator Vector */
#define ANALOG_COMP_vect  _VECTOR(23)

#define TWI_vect_num      24
/** @brief Two-wire Serial Interface Vector */
#define TWI_vect          _VECTOR(24)

#define SPM_READY_vect_num    25
/** @brief Store Program Memory Read Vector */
#define SPM_READY_vect    _VECTOR(25)

#endif

/* vi: set et ai sw=2 sts=2: */
