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
 * @file   src/drivers/uart/stm32f7xx_uart.c
 * @author Ye Li (liye@sdvos.org)
 * @brief UART Driver for STM32F75xxx/74xxx
 */
#include <sdvos_printf.h>
#include <board.h>

#ifdef TERM_BAUD
#define BAUD TERM_BAUD
#else
#define BAUD 38400UL
#endif

extern uint32_t APB1Clock;

int
uart_putchar (char c)
{
  if (c == '\n') {
    uart_putchar('\r');
  }

  USART3->TDR = c;
  /* Wait for TX */
  while ((USART3->ISR & USART_ISR_TXE) == 0);
  return (unsigned char) c;
}

int
uart_getchar ()
{
  /* Wait for RX */
  while ((USART3->ISR & USART_ISR_RXNE) == 0);
  return USART3->RDR & 0x1FF;
}

void stm32f7xx_uart_init (void)
{
  uint32_t brr = 0, mantissa = 0, frac = 0;

  /* Enable USART3 Clock */
  RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
  /* Enable GPIOD */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

  /* Setup TX (PD8) pin for Alternate Function Mode */
  GPIOD->MODER |= (0x2 << 16);
  /* Setup RX (PD9) pin for Alternate Function Mode */
  GPIOD->MODER |= (0x2 << 18);

  /* Setup the Alternate Function of PD8 to USART */
  GPIOD->AFR[1] |= (7 << 0);
  /* Setup the Alternate Function of PD9 to USART */
  GPIOD->AFR[1] |= (7 << 4);

  /* Enable USART (8N1, OVER8=0) */
  USART3->CR1 = USART_CR1_UE;
  USART3->CR2 = 0;
  USART3->CR3 = 0;

  /* Set up BRR (OVER8=0) */
  mantissa = ((25 * APB1Clock) / (4 * BAUD));
  brr = (mantissa / 100) << 4;
  frac = mantissa - (100 * (brr >> 4));
  brr |= (((((frac * 16) + 50) / 100)) & 0xF);

  USART3->BRR = (uint16_t) brr;

  /* Enable TX and RX */
  USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE);

  sdvos_init_printf ((void (*) (char)) uart_putchar);
}

/* vi: set et ai sw=2 sts=2: */
