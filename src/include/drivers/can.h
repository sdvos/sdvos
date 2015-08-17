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
 * @file   src/include/drivers/can.h
 * @author Ye Li (liye@sdvos.org)
 * @brief CAN Controller Driver Interface
 */
#ifndef _DRIVERS_CAN_H_
#define _DRIVERS_CAN_H_

#include <osek/types.h>

/** CAN Standard ID */
#define CAN_STID     0x0
/** CAN Extended ID */
#define CAN_EXTID    0x1

/** CAN Transmit return status */
typedef enum _CanTxStatusType {
  CAN_TX_STATUS_SUCCESS = 0x0,    /**< Success */
  CAN_TX_STATUS_NOMAILBOX = 0x1   /**< No mailbox left */
} CanTxStatusType;

/**
 * @brief Send a message on CAN Bus
 *
 * Send a message on CAN Bus. CAN1 controller is used by
 * default.
 *
 * @param[in] id
 *   CAN message ID
 * @param[in] ext
 *   CAN_STID -- Standard ID (11 bit)
 *   CAN_EXTID -- Extended ID (29 bit)
 * @param[in] len
 *   Message length (8 bytes maximum)
 * @param[in] buf
 *   Pointer to the data buffer
 *
 * @retval CAN_TX_STATUS_SUCCESS
 *   Transmit request is initiated
 * @retval CAN_TX_STATUS_NOMAILBOX
 *   Transmit failed. No empty mailbox left
 */
CanTxStatusType CanSendMsg (uint32_t id, uint8_t ext,
                            uint8_t len, uint8_t * buf);

/**
 * @brief Can Message Type
 * 
 * Can message structure.
 */
typedef struct _CanMsgType {
  uint32_t id;        /**< Message ID */
  uint32_t remote:1;  /**< Remote message 1-remote */
  uint32_t ide:1;     /**< ID Type 0-std 1-ext */
  uint32_t length:4;  /**< Data length */
  uint32_t fmi:8;     /**< Filter match index */
  uint32_t time:16;   /**< Message time */
  uint8_t data[8];    /**< Message payload */
} CanMsgType;

/** Receive callback function type */
typedef void (* RecvCallbackFunc) (CanMsgType *);

/**
 * @brief Register message receive callback
 *
 * Register a message receive callback function.
 *
 * @param[in] CallbackFunc
 *   Callback function to be registered
 *
 * @return
 *   Previous callback function registered.
 *   Will return NULL if no callback was registered.
 */
RecvCallbackFunc CanRecvMsgIT (RecvCallbackFunc CallbackFunc);

/**
 * @brief Receive message from CAN bus
 *
 * Try to read message from CAN bus. Notice that since RX
 * interrupt is enabled by default, this function should
 * only be used when interrupt is disabled. CanRecvMsgIT
 * should be used by application instead.
 *
 * @param[in] CanMsg
 *   Reference to CAN message structure. This structure will
 *   be filled if message exists.
 *
 * @return
 *   Number of pending messages left in the FIFO
 *   Returns -1 when no message is available
 */
int8_t CanRecvMsg (CanMsgType * CanMsg);

#endif

/* vi: set et ai sw=2 sts=2: */
