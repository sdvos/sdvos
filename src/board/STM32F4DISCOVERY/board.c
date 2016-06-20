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
 * @file   board/STM32F4DISCOVERY/board.c
 * @author Ye Li (liye@sdvos.org)
 * @brief STM32F4DISCOVERY board specific configuration
 */
#include "board.h"

/** Regulator voltage output Scale 1 mode */
#define PWR_REGULATOR_VOLTAGE_SCALE1    ((uint32_t)0x0000C000)
/** Regulator voltage output Scale 2 mode */
#define PWR_REGULATOR_VOLTAGE_SCALE2    ((uint32_t)0x00008000)
/** Regulator voltage output Scale 3 mode */
#define PWR_REGULATOR_VOLTAGE_SCALE3    ((uint32_t)0x00004000)

/** APB1 Bus Clock Frequency */
uint32_t APB1Clock = 0;

/** System Clock (Cortex System Timer) Frequency */
uint32_t SysTick_ONESEC = 0;

/** Constant that makes delay_loop delay 1 ms */
#define CONST_LOOPS 21000

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
 *     SYSCLK(Hz)                     = 168000000
 *     HCLK(Hz)                       = 168000000
 *     AHB Prescaler                  = 1
 *     APB1 Prescaler                 = 4
 *     APB2 Prescaler                 = 2
 *     HSE Frequency(Hz)              = 8000000
 *     PLL_M                          = 8
 *     PLL_N                          = 336
 *     PLL_P                          = 2
 *     PLL_Q                          = 7
 *     VDD(V)                         = 3.3
 *     Main regulator output voltage  = Scale1 mode
 *     Flash Latency(WS)              = 5
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

  /* Power interface clock enable */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  /* Select regulator voltage output Scale 1 mode */
  PWR->CR &= (~PWR_CR_VOS);
  PWR->CR |= PWR_REGULATOR_VOLTAGE_SCALE1;

  /* HCLK = SYSCLK / 1 */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

   /* PCLK1 = HCLK / 4 */
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;

  /* PCLK2 = HCLK / 2 */
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;

  /* Configure the main PLL */
  RCC->PLLCFGR = 0x8 /* PLLM 8 */ | 0x150 << 6 /* PLLN 336 */ |
                 0x0 << 16 /* PLLP 2 */ | 0x7 << 24 /* PLLQ 7 */ |
                 RCC_PLLCFGR_PLLSRC_HSE;

  /* Enable the main PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till the main PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0);

  /*
   * Configure Flash prefetch, Instruction cache,
   * Data cache and wait state
   */
  FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |
               FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

  /* Select the main PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  /* Wait till the main PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);

  /* APB1 Clock = AHB Clock / 4 */
  APB1Clock = 168000000 >> 2;
  /* Cortex Timer Clock = AHB Clock / 8 */
  SysTick_ONESEC = 168000000 >> 3;
}

void
BoardInit (void)
{
  /* Set HSION bit */
  RCC->CR |= (uint32_t) 0x00000001;

  /* Reset CFGR register */
  RCC->CFGR = 0x00000000;

  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t) 0xFEF6FFFF;

  /* Reset PLLCFGR register */
  RCC->PLLCFGR = 0x24003010;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t) 0xFFFBFFFF;

  /* System Clock Setup */
  ClockInit ();
}

/* vi: set et ai sw=2 sts=2: */
