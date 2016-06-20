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
 * @file   src/arch/armv7m/timer.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7m Systick Timer
 */
#include <arch/armv7m/interrupt.h>
#include <arch/armv7m/scs.h>
#include <arch/armv7m/utils.h>
#include <arch/armv7m/atomic.h>
#include <cc.h>
#include <sdvos.h>

extern uint32_t SysTick_ONESEC;

void
ArchTimerInit ()
{
  uint32_t rval = 0;

  /* Disable SYSTICK */
  /*
   * Always use SysTick_ONESEC from board initialization
   * if possible. If it is not available, try check for
   * external clock. In case external clock is not present,
   * we can only rely on user defined reload value. This
   * should almost never happen.
   */
  SysTick->CTRL = 0;

  if (SysTick_ONESEC) {
    /* SysTick frequency available from board initialization */
    rval = SysTick_ONESEC / 1000;
  } else {
    if (SysTick->CALIB & SysTick_CALIB_NOREF) {
      /* No external reference clock (STCLK) */
      DEBUG_PRINTF ("No reference clock!\n");
    } else {
      /* Set reload value (1ms by default) */
      /* Read calibration value for 1 ms */
      rval = SysTick->CALIB & SysTick_CALIB_TENMS / 10;

#ifdef DEBUG_SDVOS_VERBOSE
      if (SysTick->CALIB & SysTick_CALIB_SKEW) {
        DEBUG_PRINTF ("Calibration not accurate!\n");
      }
#endif
    }
  }

  /* If calibration is not available, use config value */
  if (!rval) {
    DEBUG_PRINTF ("SysTick frequency not available!\n");
    rval = SYSTICK_RELOAD_VALUE;
  }

  SysTick->LOAD = (rval & SysTick_LOAD_RELOAD);
  DEBUG_PRINTFV ("SysTick reload value: 0x%X\n", SysTick->LOAD);

  /* Clear current value */
  SysTick->VAL = 0;
  /* Enable SYSTICK */
  SysTick->CTRL |= (SysTick_CTRL_ENABLE |
                    SysTick_CTRL_TICKINT);
}

/**
 * @brief Internal Tick Handler
 *
 * Internal Tick Handler function used by the SystickHandler.
 * This is necessary because SystickHandler is NAKED. And
 * we do not save callee saved registers (r4-r11). These
 * registers will be saved by the compiler at the beginning
 * of a normal function, but not NAKED functions. NOINLINE
 * is used to avoid compiler inlining the function for
 * optimization.
 */
void NOINLINE
_SystickHandler ()
{
  uatomic_inc (&NestedISRs);
  TickHandler ();
  uatomic_dec (&NestedISRs);
  CheckPreemption (PREEMPT_ISR);
}

void NAKED
SystickHandler ()
{
  IRQSaveContext ();
  _SystickHandler ();
  IRQRestoreContext ();
  return;
}

/* vi: set et ai sw=2 sts=2: */
