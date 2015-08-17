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
 * @file   src/include/drivers/spi.h
 * @author Ye Li (liye@sdvos.org)
 * @brief SPI Controller Driver Interface
 */
#ifndef _DRIVERS_SPI_H_
#define _DRIVERS_SPI_H_

#include <osek/types.h>

/**
 * @brief Initiate an SPI read/write transaction
 *
 * SpiTransfer sends an 8-bit data to the SPI client
 * selected and waits for an 8-bit response transfer.
 * Client select is done by software outside of this
 * function.
 *
 * @param[in] data
 *   8-bit data to be transfered to the client device
 *
 * @return
 *   The 8-bit response data from the client device
 */
uint8_t SpiTransfer (uint8_t data);

#endif

/* vi: set et ai sw=2 sts=2: */
