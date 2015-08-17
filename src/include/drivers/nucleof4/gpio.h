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
 * @file   src/include/drivers/nucleof4/gpio.h
 * @author Ye Li (liye@sdvos.org)
 * @brief NUCLEO Board GPIO Driver Interface
 */
#ifndef _DRIVERS_NUCLEOF4_GPIO_H_
#define _DRIVERS_NUCLEOF4_GPIO_H_

#include <board.h>

/**
 * @def GPIOEnable(n)
 * @brief Enable GPIO Bus
 *
 * Enable a GPIO bus on the NUCLEO board.
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 */
#define GPIOEnable(n)  _GPIOEnable(n)

/**
 * @def _GPIOEnable(n)
 * @brief Internal Macro to enable argument expansion
 *
 * Enable a GPIO bus on the NUCLEO board.
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 */
#define _GPIOEnable(n)    do {                        \
  if (!(RCC->AHB1ENR & RCC_AHB1ENR_GPIO##n##EN)) {    \
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIO##n##EN;          \
  }                                                   \
} while (0)

/** GPIO Input Mode */
#define GPIO_MODE_INPUT    0x0
/** GPIO Output Mode */
#define GPIO_MODE_OUTPUT   0x1
/** GPIO Alternate Function Mode */
#define GPIO_MODE_AF       0x2
/** GPIO Analog Mode */
#define GPIO_MODE_ANALOG   0x3

/**
 * @def GPIOSetMode(n,p,m)
 * @brief Set GPIO Pin Mode
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] m
 *   GPIO mode
 */
#define GPIOSetMode(n,p,m)  _GPIOSetMode(n,p,m)

/**
 * @def _GPIOSetMode(n,p,m)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] m
 *   GPIO mode
 */
#define _GPIOSetMode(n,p,m)   do {                   \
  uint32_t mod =                                     \
    GPIO##n->MODER & ~((uint32_t)(0x3 << (p << 1))); \
  GPIO##n->MODER = (mod | (m << (p << 1)));          \
} while (0)

/** GPIO Output Push-Pull */
#define GPIO_OUTPUT_PP     0x0
/** GPIO Output Open-Drain */
#define GPIO_OUTPUT_OD     0x1

/**
 * @def GPIOSetOutput(n,p,o)
 * @brief Set GPIO Pin Output Type
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] o
 *   GPIO output type 
 */
#define GPIOSetOutput(n,p,o)  _GPIOSetOutput(n,p,o)

/**
 * @def _GPIOSetOutput(n,p,o)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] o
 *   GPIO output type 
 */
#define _GPIOSetOutput(n,p,o)   do {                  \
  uint32_t otype =                                   \
    GPIO##n->OTYPER & ~((uint32_t) (0x1 << p));      \
  GPIO##n->OTYPER = (otype | (o << p));              \
} while (0)

/** GPIO No Pull-up, Pull-down */
#define GPIO_NOPULL        0x0
/** GPIO Pull-up */
#define GPIO_PULLUP        0x1
/** GPIO Pull-down */
#define GPIO_PULLDOWN      0x2

/**
 * @def GPIOSetPUPD(n,p,u)
 * @brief Set GPIO Pull-up/Pull-down
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] u
 *   GPIO pull-up/pull-down
 */
#define GPIOSetPUPD(n,p,u)  _GPIOSetPUPD(n,p,u)

/**
 * @def _GPIOSetPUPD(n,p,u)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] u
 *   GPIO pull-up/pull-down
 */
#define _GPIOSetPUPD(n,p,u)    do {                  \
  uint32_t pupd =                                    \
    GPIO##n->PUPDR & ~((uint32_t)(0x3 << (p << 1))); \
  GPIO##n->PUPDR = (pupd | (u << (p << 1)));         \
} while (0)

/** GPIO low speed output */
#define GPIO_LOW_SPEED     0x0
/** GPIO medium speed output */
#define GPIO_MEDIUM_SPEED  0x1
/** GPIO fast speed output */
#define GPIO_FAST_SPEED    0x2
/** GPIO high speed output */
#define GPIO_HIGH_SPEED    0x3

/**
 * @def GPIOSetSpeed(n,p,s)
 * @brief Set GPIO Speed
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] s
 *   GPIO speed
 */
#define GPIOSetSpeed(n,p,s)  _GPIOSetSpeed(n,p,s)

/**
 * @def _GPIOSetSpeed(n,p,s)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] s
 *   GPIO speed
 */
#define _GPIOSetSpeed(n,p,s)    do {                   \
  uint32_t speed =                                     \
    GPIO##n->OSPEEDR & ~((uint32_t)(0x3 << (p << 1))); \
  GPIO##n->OSPEEDR = (speed | (s << (p << 1)));        \
} while (0)

/**
 * @def GPIODigitalWrite(n,p,v)
 * @brief Write 1 or 0 to digital IO pin
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] v
 *   Value to be written (0 or 1)
 */
#define GPIODigitalWrite(n,p,v)  _GPIODigitalWrite(n,p,v)

/**
 * @def _GPIODigitalWrite(n,p,v)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] v
 *   Value to be written (0 or 1)
 */
#define _GPIODigitalWrite(n,p,v)    do {             \
  uint32_t odr =                                     \
    GPIO##n->ODR & ~((uint32_t)(0x1 << p));          \
  GPIO##n->ODR = (odr | (v << p));                   \
} while (0)

/**
 * @def GPIODigitalToggle(n,p)
 * @brief Toggle an digital IO pin
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 */
#define GPIODigitalToggle(n,p)  _GPIODigitalToggle(n,p)

/**
 * @def _GPIODigitalToggle(n,p)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 */
#define _GPIODigitalToggle(n,p)    do {              \
  uint32_t odr =                                     \
    GPIO##n->ODR & ((uint32_t)(0x1 << p));           \
  (odr) ? (GPIO##n->ODR &= ~((uint32_t)(0x1 << p))) :\
          (GPIO##n->ODR |= ((uint32_t)(0x1 << p)));  \
} while (0)

/**
 * @def GPIODigitalRead(n,p)
 * @brief Read value from digital IO pin
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 */
#define GPIODigitalRead(n,p)  _GPIODigitalRead(n,p)

/**
 * @def _GPIODigitalRead(n,p)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 */
#define _GPIODigitalRead(n,p)                        \
  ((GPIO##n->IDR & ((uint32_t) (0x1 << p))) ? 1 : 0)

/** GPIO Alternate Function 0 */
#define GPIO_AF0        0x0
/** GPIO Alternate Function 1 */
#define GPIO_AF1        0x1
/** GPIO Alternate Function 2 */
#define GPIO_AF2        0x2
/** GPIO Alternate Function 3 */
#define GPIO_AF3        0x3
/** GPIO Alternate Function 4 */
#define GPIO_AF4        0x4
/** GPIO Alternate Function 5 */
#define GPIO_AF5        0x5
/** GPIO Alternate Function 6 */
#define GPIO_AF6        0x6
/** GPIO Alternate Function 7 */
#define GPIO_AF7        0x7
/** GPIO Alternate Function 8 */
#define GPIO_AF8        0x8
/** GPIO Alternate Function 9 */
#define GPIO_AF9        0x9
/** GPIO Alternate Function 10 */
#define GPIO_AF10       0x10
/** GPIO Alternate Function 11 */
#define GPIO_AF11       0x11
/** GPIO Alternate Function 12 */
#define GPIO_AF12       0x12
/** GPIO Alternate Function 13 */
#define GPIO_AF13       0x13
/** GPIO Alternate Function 14 */
#define GPIO_AF14       0x14
/** GPIO Alternate Function 15 */
#define GPIO_AF15       0x15

/**
 * @def GPIOAlternateFunction(n,p,a)
 * @brief Configure GPIO Alternate Function
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] a
 *   Alternate Function (GPIO_AF0 to 15)
 */
#define GPIOAlternateFunction(n,p,a)  _GPIOAlternateFunction(n,p,a)

/**
 * @def _GPIOAlternateFunction(n,p,a)
 * @brief Internal Macro to enable argument expansion
 *
 * @param[in] n
 *   GPIO bus (A/B/C/...)
 * @param[in] p
 *   GPIO pin number
 * @param[in] a
 *   Alternate Function (GPIO_AF0 to 15)
 */
#define _GPIOAlternateFunction(n,p,a)    do {        \
  uint8_t i = (p <= 7) ? 0 : 1;                      \
  uint32_t afr =                                     \
    GPIO##n->AFR[i] &                                \
    ~((uint32_t)(0xF << ((p % 8) << 2)));            \
  GPIO##n->AFR[i] = (afr | (a << ((p % 8) << 2)));   \
} while (0)

#endif

/* vi: set et ai sw=2 sts=2: */
