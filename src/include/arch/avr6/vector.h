/*
 *         Standard Dependable Vehicle Operating System
 *
 * Copyright (C) 2016 Ye Li (liye@sdvos.org)
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
 * @file   src/include/arch/avr6/vector.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR6 Interrupt Vectors
 */
#ifndef _AVR6_VECTOR_H_
#define _AVR6_VECTOR_H_

/*
 * Vector definitions for Atmega2560. Do not include if
 * avr/interrupt.h in avr-libc is used.
 */

/* AVR6 has 57 interrupt vectors */
#define NUM_IVT_ENTRY    0x39

#define _VECTOR(N) __vector_ ## N

#define INT0_vect_num     1
/** @brief External Interrupt Request 0 Vector */
#define INT0_vect         _VECTOR(1)

#define INT1_vect_num     2
/** @brief External Interrupt Request 1 Vector */
#define INT1_vect         _VECTOR(2)

#define INT2_vect_num     3
/** @brief External Interrupt Request 2 Vector */
#define INT2_vect         _VECTOR(3)

#define INT3_vect_num     4
/** @brief External Interrupt Request 3 Vector */
#define INT3_vect         _VECTOR(4)

#define INT4_vect_num     5
/** @brief External Interrupt Request 4 Vector */
#define INT4_vect         _VECTOR(5)

#define INT5_vect_num     6
/** @brief External Interrupt Request 5 Vector */
#define INT5_vect         _VECTOR(6)

#define INT6_vect_num     7
/** @brief External Interrupt Request 6 Vector */
#define INT6_vect         _VECTOR(7)

#define INT7_vect_num     8
/** @brief External Interrupt Request 7 Vector */
#define INT7_vect         _VECTOR(8)

#define PCINT0_vect_num   9
/** @brief Pin Change Interrupt Request 0 Vector */
#define PCINT0_vect       _VECTOR(9)

#define PCINT1_vect_num   10
/** @brief Pin Change Interrupt Request 1 Vector */
#define PCINT1_vect       _VECTOR(10)

#define PCINT2_vect_num   11
/** @brief Pin Change Interrupt Request 2 Vector */
#define PCINT2_vect       _VECTOR(11)

#define WDT_vect_num      12
/** @brief Watchdog Time-out Interrupt Vector */
#define WDT_vect          _VECTOR(12)

#define TIMER2_COMPA_vect_num 13
/** @brief Timer/Counter2 Compare Match A Vector */
#define TIMER2_COMPA_vect _VECTOR(13)

#define TIMER2_COMPB_vect_num 14
/** @brief Timer/Counter2 Compare Match B Vector */
#define TIMER2_COMPB_vect _VECTOR(14)

#define TIMER2_OVF_vect_num   15
/** @brief Timer/Counter2 Overflow Vector */
#define TIMER2_OVF_vect   _VECTOR(15)

#define TIMER1_CAPT_vect_num  16
/** @brief Timer/Counter1 Capture Event Vector */
#define TIMER1_CAPT_vect  _VECTOR(16)

#define TIMER1_COMPA_vect_num 17
/** @brief Timer/Counter1 Compare Match A Vector */
#define TIMER1_COMPA_vect _VECTOR(17)

#define TIMER1_COMPB_vect_num 18
/** @brief Timer/Counter1 Compare Match B Vector */
#define TIMER1_COMPB_vect _VECTOR(18)

#define TIMER1_COMPC_vect_num 19
/** @brief Timer/Counter1 Compare Match C Vector */
#define TIMER1_COMPC_vect _VECTOR(19)

#define TIMER1_OVF_vect_num   20
/** @brief Timer/Counter1 Overflow Vector */
#define TIMER1_OVF_vect   _VECTOR(20)

#define TIMER0_COMPA_vect_num 21
/** @brief TimerCounter0 Compare Match A Vector */
#define TIMER0_COMPA_vect _VECTOR(21)

#define TIMER0_COMPB_vect_num 22
/** @brief TimerCounter0 Compare Match B Vector */
#define TIMER0_COMPB_vect _VECTOR(22)

#define TIMER0_OVF_vect_num  23
/** @brief Timer/Couner0 Overflow Vector */
#define TIMER0_OVF_vect   _VECTOR(23)

#define SPI_STC_vect_num  24
/** @brief SPI Serial Transfer Complete Vector */
#define SPI_STC_vect      _VECTOR(24)

#define USART0_RX_vect_num 25
/** @brief USART0 Rx Complete Vector */
#define USART0_RX_vect     _VECTOR(25)

#define USART0_UDRE_vect_num   26
/** @brief USART0, Data Register Empty Vector */
#define USART0_UDRE_vect   _VECTOR(26)

#define USART0_TX_vect_num 27
/** @brief USART0 Tx Complete Vector */
#define USART0_TX_vect     _VECTOR(27)

#define ANALOG_COMP_vect_num   28
/** @brief Analog Comparator Vector */
#define ANALOG_COMP_vect       _VECTOR(28)

#define ADC_vect_num      29
/** @brief ADC Conversion Complete Vector */
#define ADC_vect          _VECTOR(29)

#define EE_READY_vect_num 30
/** @brief EEPROM Ready Vector */
#define EE_READY_vect     _VECTOR(30)

#define TIMER3_CAPT_vect_num    31
/** @brief Timer/Counter3 Capture Event Vector */
#define TIMER3_CAPT_vect        _VECTOR(31)

#define TIMER3_COMPA_vect_num   32
/** @brief Timer/Counter3 Compare Match A Vector */
#define TIMER3_COMPA_vect       _VECTOR(32)

#define TIMER3_COMPB_vect_num   33
/** @brief Timer/Counter3 Compare Match B Vector */
#define TIMER3_COMPB_vect       _VECTOR(33)

#define TIMER3_COMPC_vect_num   34
/** @brief Timer/Counter3 Compare Match C Vector */
#define TIMER3_COMPC_vect       _VECTOR(34)

#define TIMER3_OVF_vect_num     35
/** @brief Timer/Counter3 Overflow Vector */
#define TIMER3_OVF_vect         _VECTOR(35)

#define USART1_RX_vect_num 36
/** @brief USART1 Rx Complete Vector */
#define USART1_RX_vect     _VECTOR(36)

#define USART1_UDRE_vect_num   37
/** @brief USART1, Data Register Empty Vector */
#define USART1_UDRE_vect   _VECTOR(37)

#define USART1_TX_vect_num 38
/** @brief USART1 Tx Complete Vector */
#define USART1_TX_vect     _VECTOR(38)

#define TWI_vect_num      39
/** @brief Two-wire Serial Interface Vector */
#define TWI_vect          _VECTOR(39)

#define SPM_READY_vect_num    40
/** @brief Store Program Memory Read Vector */
#define SPM_READY_vect    _VECTOR(40)

#define TIMER4_CAPT_vect_num    41
/** @brief Timer/Counter4 Capture Event Vector */
#define TIMER4_CAPT_vect        _VECTOR(41)

#define TIMER4_COMPA_vect_num   42
/** @brief Timer/Counter4 Compare Match A Vector */
#define TIMER4_COMPA_vect       _VECTOR(42)

#define TIMER4_COMPB_vect_num   43
/** @brief Timer/Counter4 Compare Match B Vector */
#define TIMER4_COMPB_vect       _VECTOR(43)

#define TIMER4_COMPC_vect_num   44
/** @brief Timer/Counter4 Compare Match C Vector */
#define TIMER4_COMPC_vect       _VECTOR(44)

#define TIMER4_OVF_vect_num     45
/** @brief Timer/Counter4 Overflow Vector */
#define TIMER4_OVF_vect         _VECTOR(45)

#define TIMER5_CAPT_vect_num    46
/** @brief Timer/Counter5 Capture Event Vector */
#define TIMER5_CAPT_vect        _VECTOR(46)

#define TIMER5_COMPA_vect_num   47
/** @brief Timer/Counter5 Compare Match A Vector */
#define TIMER5_COMPA_vect       _VECTOR(47)

#define TIMER5_COMPB_vect_num   48
/** @brief Timer/Counter5 Compare Match B Vector */
#define TIMER5_COMPB_vect       _VECTOR(48)

#define TIMER5_COMPC_vect_num   49
/** @brief Timer/Counter5 Compare Match C Vector */
#define TIMER5_COMPC_vect       _VECTOR(49)

#define TIMER5_OVF_vect_num     50
/** @brief Timer/Counter5 Overflow Vector */
#define TIMER5_OVF_vect         _VECTOR(50)

#define USART2_RX_vect_num 51
/** @brief USART2 Rx Complete Vector */
#define USART2_RX_vect     _VECTOR(51)

#define USART2_UDRE_vect_num   52
/** @brief USART2, Data Register Empty Vector */
#define USART2_UDRE_vect   _VECTOR(52)

#define USART2_TX_vect_num 53
/** @brief USART2 Tx Complete Vector */
#define USART2_TX_vect     _VECTOR(53)

#define USART3_RX_vect_num 54
/** @brief USART3 Rx Complete Vector */
#define USART3_RX_vect     _VECTOR(54)

#define USART3_UDRE_vect_num   55
/** @brief USART3, Data Register Empty Vector */
#define USART3_UDRE_vect   _VECTOR(55)

#define USART3_TX_vect_num 56
/** @brief USART3 Tx Complete Vector */
#define USART3_TX_vect     _VECTOR(56)

#endif

/* vi: set et ai sw=2 sts=2: */
