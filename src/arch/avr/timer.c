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
 * @file   src/arch/avr/timer.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  AVR5 Timer/Counter
 */
#include <arch/avr/io.h>
#include <arch/avr/vector.h>
#include <arch/avr/interrupt.h>
#include <sdvos.h>

void
ArchTimerInit ()
{
  TCCR1A = 0;
  TCCR1B = 0;

  /*
   * For 16MHz Processor:
   *
   * --------------------------------
   * | OCR1A  |  Prescale  |  Tick  |
   * --------------------------------
   * | 15624  |  1024      |  1s    |
   * | 625    |  256       |  10ms  |
   * | 250    |  64        |  1ms   |
   * | 2      |  8         |  1us   |
   * --------------------------------
   *
   * ---------------------------------
   * | Prescale  |  CS12  CS11  CS10 |
   * ---------------------------------
   * | 1         |  0     0     1    |
   * | 8         |  0     1     0    |
   * | 64        |  0     1     1    |
   * | 256       |  1     0     0    |
   * | 1024      |  1     0     1    |
   * ---------------------------------
   *
   * For 1MHz Internal Oscillator:
   *
   * --------------------------------
   * | OCR1A  |  Prescale  |  Tick  |
   * --------------------------------
   * | 125    |  8         |  1ms   |
   * ---------------------------------
   */

#if F_CPU == 16000000UL
  /* set compare match register to desired timer count */
  OCR1A = 250;
  /* turn on CTC mode */
  TCCR1B |= (1 << WGM12);
  /* Enable Timer Compare Interrupt */
  TIMSK1 = (1 << OCIE1A);
  /* Set prescale */
  TCCR1B |= (1 << CS11) | (1 << CS10);
#elif F_CPU == 1000000UL
  /* set compare match register to desired timer count */
  OCR1A = 125;
  /* turn on CTC mode */
  TCCR1B |= (1 << WGM12);
  /* Enable Timer Compare Interrupt */
  TIMSK1 = (1 << OCIE1A);
  /* Set prescale */
  TCCR1B |= (1 << CS11);
#else
#error F_CPU not supported!
#endif
}

/*
 * Timer interrupt configured for global system tick.
 * Timer 1 in AVR5 is used for this purpose. Since avr
 * libc uses Timer 1 for servo, there might be potential
 * timer conflict.
 */
ISR (TIMER1_COMPA_vect)
{
  TickHandler ();
}

/* vi: set et ai sw=2 sts=2: */
