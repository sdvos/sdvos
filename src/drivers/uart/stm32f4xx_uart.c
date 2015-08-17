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
 * @file   src/drivers/uart/stm32f4xx_uart.c
 * @author Ye Li (liye@sdvos.org)
 * @brief UART Driver for STM32F405xx/07xx
 */
#include <sdvos_printf.h>
#include <board.h>

#define BAUD 38400UL
extern uint32_t APB1Clock; 

int
uart_putchar (char c)
{
  if (c == '\n') {
    uart_putchar('\r');
  }

  USART2->DR = c;
  /* Wait for TX */
  while ((USART2->SR & USART_SR_TXE) == 0);
  return (unsigned char) c;
}

int
uart_getchar ()
{
  /* Wait for RX */
  while ((USART2->SR & USART_SR_RXNE) == 0);
  return USART2->DR & 0x1FF;
}

void stm32f4xx_uart_init (void)
{
  uint32_t brr = 0, mantissa = 0, frac = 0;

  /* Enable USART2 Clock */
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  /* Enable GPIOA */
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  /* Setup TX (PA2) pin for Alternate Function Mode */
  GPIOA->MODER |= (0x2 << 4);
  /* Setup RX (PA3) pin for Alternate Function Mode */
  GPIOA->MODER |= (0x2 << 6);

  /* Setup the Alternate Function of PA2 to USART */
  GPIOA->AFR[0] |= (7 << 8);
  /* Setup the Alternate Function of PA3 to USART */
  GPIOA->AFR[0] |= (7 << 12);

  /* Enable USART (8N1, OVER8=0) */
  USART2->CR1 = USART_CR1_UE;
  USART2->CR2 = 0;
  USART2->CR3 = 0;

  /* Set up BRR (OVER8=0) */
  mantissa = ((25 * APB1Clock) / (4 * BAUD));    
  brr = (mantissa / 100) << 4;
  frac = mantissa - (100 * (brr >> 4));
  brr |= (((((frac * 16) + 50) / 100)) & 0xF);

  USART2->BRR = (uint16_t) brr;

  /* Enable TX and RX */
  USART2->CR1 |= (USART_CR1_TE | USART_CR1_RE);

  sdvos_init_printf ((void (*) (char)) uart_putchar);
}

/* vi: set et ai sw=2 sts=2: */
