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
 * @file   src/drivers/uart/stm32f10x_uart.c
 * @author Ye Li (liye@sdvos.org)
 * @brief UART Driver for STM32F10X
 */
#include <sdvos_printf.h>
#include <board.h>

#ifdef TERM_BAUD
#define BAUD TERM_BAUD
#else
#define BAUD 38400UL
#endif

extern uint32_t APB2Clock;

int
uart_putchar (char c)
{
  if (c == '\n') {
    uart_putchar('\r');
  }

  USART1->DR = c;
  /* Wait for TX */
  while ((USART1->SR & USART_SR_TXE) == 0);
  return (unsigned char) c;
}

int
uart_getchar ()
{
  /* Wait for RX */
  while ((USART1->SR & USART_SR_RXNE) == 0);
  return USART1->DR & 0x1FF;
}

void
stm32f10x_uart_init (void)
{
  /* Turn on USART1 and IO Port A */
  RCC->APB2ENR = RCC_APB2ENR_USART1EN | RCC_APB2ENR_IOPAEN;

  /* Enable USART1 (8-N-1) */
  /* M=0, 8 bits, no parity */
  USART1->CR1 = USART_CR1_UE;

  /* 1 stop bit */
  USART1->CR2 = 0;
  USART1->CR3 = 0;

  /* Configure PA9 (TX) and PA10 (RX) */
  GPIOA->CRH = 0x000004B0;

  /* Configure BRR (bus clock / baud rate) */
  USART1->BRR = APB2Clock / BAUD;

  /* Enable TX and RX */
  USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;

  sdvos_init_printf ((void (*) (char)) uart_putchar);
}

/* vi: set et ai sw=2 sts=2: */
