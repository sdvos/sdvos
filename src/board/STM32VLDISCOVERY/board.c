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
 * @file   board/STM32VLDISCOVERY/board.c
 * @author Ye Li (liye@sdvos.org)
 * @brief STM32VLDISCOVERY board specific configuration
 */
#include "board.h"

/** APB1 Bus Clock Frequency */
uint32_t APB1Clock = 0;

/** APB2 Bus Clock Frequency */
uint32_t APB2Clock = 0;

/** System Clock (Cortex System Timer) Frequency */
uint32_t SysTick_ONESEC = 0;

/** Constant that makes delay_loop delay 1 ms */
#define CONST_LOOPS 3000

/**
 * @def delay_loop
 * @brief Basic delay loop used in LoopDelay
 *
 * This loop should take 8 cycles in the body. Based on
 * which we can calculate the CONST_LOOPS macro and
 * provide a benchmark 1 ms loop.
 *
 * @param[in] n
 *   Loop count
 */
#define delay_loop(n)                         \
  __asm__ volatile ("movs r0, #0\n\t"         \
                    ".align 4\n\t"            \
                    "1:\n\t"                  \
                    "adds r0, r0, #1\n\t"     \
                    "cmp r0, %0\n\t"          \
                    "nop\n\t"                 \
                    "nop\n\t"                 \
                    "nop\n\t"                 \
                    "nop\n\t"                 \
                    "bne 1b\n\t"              \
                    :: "l" (n) : "r0")

void
LoopDelay (uint16_t ms)
{
  if (!ms) return;

  delay_loop (ms * CONST_LOOPS);
}

/**
 * @brief  System Clock Configuration
 *
 * The system Clock is configured as follow : 
 *     System Clock source            = PLL (HSE)
 *     SYSCLK(Hz)                     = 24000000
 *     HCLK(Hz)                       = 24000000
 *     AHB Prescaler                  = 1
 *     APB1 Prescaler                 = 1
 *     APB2 Prescaler                 = 1
 *     HSE Frequency(Hz)              = 8000000
 *     HSE PREDIV1                    = 2
 *     PLLMUL                         = 6
 *     Flash Latency(WS)              = 0
 *
 * @param  None
 * @retval None
 */
static void
ClockInit (void)
{
  /* Enable HSE */
  RCC->CR |= RCC_CR_HSEON;

  /* Wait till the HSE is ready */
  while((RCC->CR & RCC_CR_HSERDY) == 0);

  /* HSE PREDIV1 = 2 */
  RCC->CFGR2 |= RCC_CFGR2_PREDIV1_DIV2;

  /* Set PLL source to HSE */
  RCC->CFGR |= RCC_CFGR_PLLSRC;

  /* Set PLL Multiplication Factor to 6 */
  RCC->CFGR |= RCC_CFGR_PLLMULL6;

  /* HCLK = SYSCLK / 1 */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

   /* PCLK1 = HCLK / 1 */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;

  /* PCLK2 = HCLK / 1 */
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0);

  /* Select the main PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);

  /* APB1 Clock = AHB Clock */
  APB1Clock = 24000000;
  /* APB2 Clock = AHB Clock */
  APB2Clock = 24000000;
  /* Cortex Timer Clock = AHB Clock / 8 */
  SysTick_ONESEC = 24000000 >> 3;
}

void
BoardInit (void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t) 0x00000001;

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
  RCC->CFGR &= (uint32_t) 0xF0FF0000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t) 0xFEF6FFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t) 0xFFFBFFFF;

  /* Reset CFGR2 register */
  RCC->CFGR2 = (uint32_t) 0x00000000;

  /* System Clock Setup */
  ClockInit ();
}

/* vi: set et ai sw=2 sts=2: */
