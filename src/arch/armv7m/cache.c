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
 * @file   src/arch/armv7m/cache.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M Cache Management
 */
#include <arch/armv7m/barrier.h>
#include <arch/armv7m/types.h>
#include <arch/armv7m/cache.h>
#include <cc.h>

void
SCB_EnableICache (void)
{
  DSB ();
  ISB ();
  /* Invalidate I-Cache */
  SCB->ICIALLU = 0UL;
  /* Enable I-Cache */
  SCB->CCR |= SCB_CCR_IC;
  DSB ();
  ISB ();
}

void
SCB_EnableDCache (void)
{
  uint32_t sets;
  uint32_t ways;
  uint8_t sw_a = 0, sw_l = 0;

  /*
   * Calculate A and L offsets for setting registers in
   * cache clean/invalidate operations. See B2.2.7 in the
   * ARMv7M ARM (DDI 0403E.b).
   */
  while ((1 << sw_a) < (CCSIDR_WAYS () + 1)) sw_a++;
  while ((1 << sw_l) < (CCSIDR_SETS () + 1)) sw_l++;

  /* Level 1 data cache */
  SCB->CSSELR = (0U << 1U) | 0U;
  DSB ();

  /* Invalidate D-Cache */
  sets = CCSIDR_SETS ();
  do {
    ways = CCSIDR_WAYS ();
    do {
      SCB->DCISW = ((sets << sw_l) | (ways << (32 - sw_a)));
      CC_MB ();
    } while (ways--);
  } while (sets--);
  DSB ();

  /* Enable D-Cache */
  SCB->CCR |= SCB_CCR_DC;
  DSB ();
  ISB ();
}

void
SCB_DisableICache (void)
{
  DSB ();
  ISB ();
  /* Disable I-Cache */
  SCB->CCR &= ~SCB_CCR_IC;
  /* Invalidate I-Cache */
  SCB->ICIALLU = 0UL;
  DSB ();
  ISB ();
}

void
SCB_DisableDCache (void)
{
  uint32_t sets;
  uint32_t ways;
  uint8_t sw_a = 0, sw_l = 0;

  /*
   * Calculate A and L offsets for setting registers in
   * cache clean/invalidate operations. See B2.2.7 in the
   * ARMv7M ARM (DDI 0403E.b).
   */
  while ((1 << sw_a) < (CCSIDR_WAYS () + 1)) sw_a++;
  while ((1 << sw_l) < (CCSIDR_SETS () + 1)) sw_l++;

  /* Level 1 data cache */
  SCB->CSSELR = (0U << 1U) | 0U;
  DSB ();

  /* Disable D-Cache */
  SCB->CCR &= ~SCB_CCR_DC;

  /* clean & Invalidate D-Cache */
  sets = CCSIDR_SETS ();
  do {
    ways = CCSIDR_WAYS ();
    do {
      SCB->DCCISW = ((sets << sw_l) | (ways << (32 - sw_a)));
      CC_MB ();
    } while (ways--);
  } while (sets--);

  DSB ();
  ISB ();
}

/* vi: set et ai sw=2 sts=2: */
