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
 * @file   src/drivers/spi/stm32f4xx_spi.c
 * @author Ye Li (liye@sdvos.org)
 * @brief SPI Controller Driver for STM32F4xx
 */
#include <board.h>
#include <arch/armv7m/types.h>
#include <arch/armv7m/utils.h>
#include <drivers/nucleof4/gpio.h>
#include <drivers/spi.h>

/* SPI Baudrate Prescaler Definitions */
#define SPI_BAUDRATEPRESCALER_2      ((uint32_t) 0x00000000)
#define SPI_BAUDRATEPRESCALER_4      ((uint32_t) 0x00000008)
#define SPI_BAUDRATEPRESCALER_8      ((uint32_t) 0x00000010)
#define SPI_BAUDRATEPRESCALER_16     ((uint32_t) 0x00000018)
#define SPI_BAUDRATEPRESCALER_32     ((uint32_t) 0x00000020)
#define SPI_BAUDRATEPRESCALER_64     ((uint32_t) 0x00000028)
#define SPI_BAUDRATEPRESCALER_128    ((uint32_t) 0x00000030)
#define SPI_BAUDRATEPRESCALER_256    ((uint32_t) 0x00000038)

/** SPI1 GPIO Port */
#define SPI1_GPIO_PORT    A
/** SPI1 SCK Pin */
#define SPI1_SCK_PIN      5
/** SPI1 MISO Pin */
#define SPI1_MISO_PIN     6
/** SPI1 MOSI Pin */
#define SPI1_MOSI_PIN     7
/** SPI1 GPIO Alternate Function */
#define SPI1_GPIO_AF      GPIO_AF5

/**
 * @def SPI_DISABLE
 * @brief Disable SPI controller
 *
 * @param[in] s
 *   SPI controller name (SPI1, SPI2, SPI3, ...)
 */
#define SPI_DISABLE(s)  s->CR1 &= ~SPI_CR1_SPE

/**
 * @def SPI_ENABLE
 * @brief Enable SPI controller
 *
 * @param[in] s
 *   SPI controller name (SPI1, SPI2, SPI3, ...)
 */
#define SPI_ENABLE(s)  s->CR1 |= SPI_CR1_SPE

/**
 * @def SPI_CLK_ENABLE
 * @brief Enable SPI clock
 *
 * @param[in] s
 *   SPI controller name (SPI1, SPI2, SPI3, ...)
 */
#define SPI_CLK_ENABLE(s)    \
  (RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN))

uint8_t
SpiTransfer (uint8_t data)
{
  /* Wait for TXE */
  while (!(SPI1->SR & SPI_SR_TXE));
  /* Send data */
  SPI1->DR = data;

  /* Wait for RXNE */
  while (!(SPI1->SR & SPI_SR_RXNE));

  return SPI1->DR;
}

void
stm32f4xx_spi_init (void)
{
  /* Disable controller */
  SPI_DISABLE (SPI1);

  /* Enable SPI1 Clock */
  SPI_CLK_ENABLE (SPI1);

  /* Enable GPIO Port */
  GPIOEnable (SPI1_GPIO_PORT);

  /* Initialize SPI1 GPIO pins */
  GPIOAlternateFunction (SPI1_GPIO_PORT, SPI1_SCK_PIN,
                         SPI1_GPIO_AF);
  GPIOSetMode (SPI1_GPIO_PORT, SPI1_SCK_PIN, GPIO_MODE_AF);
  GPIOSetOutput (SPI1_GPIO_PORT, SPI1_SCK_PIN, GPIO_OUTPUT_PP);
  GPIOSetSpeed (SPI1_GPIO_PORT, SPI1_SCK_PIN, GPIO_HIGH_SPEED);
  GPIOSetPUPD (SPI1_GPIO_PORT, SPI1_SCK_PIN, GPIO_PULLDOWN);

  GPIOAlternateFunction (SPI1_GPIO_PORT, SPI1_MISO_PIN,
                         SPI1_GPIO_AF);
  GPIOSetMode (SPI1_GPIO_PORT, SPI1_MISO_PIN, GPIO_MODE_AF);
  GPIOSetOutput (SPI1_GPIO_PORT, SPI1_MISO_PIN, GPIO_OUTPUT_PP);
  GPIOSetSpeed (SPI1_GPIO_PORT, SPI1_MISO_PIN, GPIO_HIGH_SPEED);
  GPIOSetPUPD (SPI1_GPIO_PORT, SPI1_MISO_PIN, GPIO_PULLDOWN);

  GPIOAlternateFunction (SPI1_GPIO_PORT, SPI1_MOSI_PIN,
                         SPI1_GPIO_AF);
  GPIOSetMode (SPI1_GPIO_PORT, SPI1_MOSI_PIN, GPIO_MODE_AF);
  GPIOSetOutput (SPI1_GPIO_PORT, SPI1_MOSI_PIN, GPIO_OUTPUT_PP);
  GPIOSetSpeed (SPI1_GPIO_PORT, SPI1_MOSI_PIN, GPIO_HIGH_SPEED);
  GPIOSetPUPD (SPI1_GPIO_PORT, SPI1_MOSI_PIN, GPIO_PULLDOWN);

  /*
   * Configure CR1:
   *   CPHA = 0, CPOL = 0
   *   Baudrate PCLK / 16
   *   Master configuration MSTR = 1
   *   MSB transmitted first LSBFIRST = 0
   *   Software Slave Management SSM = 1
   *   2-line unidirectional mode
   *     BIDIMODE = 0, BIDIOE = 0, RXONLY = 0
   *   8-bit data DFF = 0
   *   CRC disable CRCEN = 0
   */
  SPI1->CR1 = SPI_BAUDRATEPRESCALER_16 |
              SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI;

  /*
   * Configure CR2:
   *   SS output disable SSOE = 0
   *   SPI TI mode disabled FRF = 0
   */
  SPI1->CR2 = 0x0;

  /* Activate SPI mode */
  SPI1->I2SCFGR &= ~((uint32_t) SPI_I2SCFGR_I2SMOD);

  /* CRC polynomial set to default (0x7) */
  SPI1->CRCPR = 0x7;

  /* Enable controller */
  SPI_ENABLE (SPI1);
}

/* vi: set et ai sw=2 sts=2: */
