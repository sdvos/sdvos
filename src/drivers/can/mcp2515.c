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

/*
 * mcp_can.cpp
 * 2012 Copyright (c) Seeed Technology Inc.  All right reserved.
 *
 * Author:Loovee
 * 2012-4-24
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-
 * 1301  USA
 */

/**
 * @file   src/drivers/can/mcp2515.c
 * @author Ye Li (liye@sdvos.org)
 * @brief MCP2515 SPI CAN Controller Driver
 *
 * This driver is based on the Seeed CAN bus shield driver
 */
#include <sdvos.h>
#include <board.h>
#include <drivers/spi.h>
#include <drivers/can.h>
#include <drivers/nucleof4/gpio.h>
#include "mcp2515.h"

extern uint32_t APB1Clock;

#define spi_readwrite SpiTransfer
#define spi_read() spi_readwrite(0x00)

#define MCP2515_SS_GPIO_PORT    B
#define MCP2515_SS_GPIO_PIN     6

#define MCP2515_SELECT()                    \
  GPIODigitalWrite (MCP2515_SS_GPIO_PORT,   \
                    MCP2515_SS_GPIO_PIN, 0);\
  LoopDelay (10)

#define MCP2515_UNSELECT()                  \
  GPIODigitalWrite (MCP2515_SS_GPIO_PORT,   \
                    MCP2515_SS_GPIO_PIN, 1);\
  LoopDelay (10)

static void
mcp2515_reset (void)
{
  MCP2515_SELECT ();
  spi_readwrite (MCP_RESET);
  MCP2515_UNSELECT ();
  LoopDelay (10);
}

static void
mcp2515_setRegister (const uint8_t address, const uint8_t value)
{
  MCP2515_SELECT ();
  spi_readwrite (MCP_WRITE);
  spi_readwrite (address);
  spi_readwrite (value);
  MCP2515_UNSELECT ();
}


static uint8_t
mcp2515_readRegister (const uint8_t address)
{
  uint8_t ret;

  MCP2515_SELECT ();
  spi_readwrite (MCP_READ);
  spi_readwrite (address);
  ret = spi_read ();
  MCP2515_UNSELECT ();

  return ret;
}

static void
mcp2515_modifyRegister (const uint8_t address,
                        const uint8_t mask, const uint8_t data)
{
  MCP2515_SELECT ();
  spi_readwrite (MCP_BITMOD);
  spi_readwrite (address);
  spi_readwrite (mask);
  spi_readwrite (data);
  MCP2515_UNSELECT ();
}

static void
mcp2515_setRegisterS (const uint8_t address, const uint8_t values[],
                      const uint8_t n)
{
  uint8_t i;
  MCP2515_SELECT ();
  spi_readwrite (MCP_WRITE);
  spi_readwrite (address);

  for (i=0; i<n; i++) {
    spi_readwrite (values[i]);
  }
  MCP2515_UNSELECT ();
}

static uint8_t
mcp2515_setCANCTRL_Mode (const uint8_t newmode)
{
  uint8_t i;

  mcp2515_modifyRegister (MCP_CANCTRL, MODE_MASK, newmode);

  i = mcp2515_readRegister (MCP_CANCTRL);

  i &= MODE_MASK;

  if (i == newmode) {
    return MCP2515_OK;
  }

  return MCP2515_FAIL;
}

void
mcp2515_readRegisterS (const uint8_t address, uint8_t values[],
                       const uint8_t n)
{
  uint8_t i;

  MCP2515_SELECT ();

  spi_readwrite (MCP_READ);
  spi_readwrite (address);

  for (i = 0; i < n; i++) {
    values[i] = spi_read ();
  }

  MCP2515_UNSELECT ();
}

static uint8_t
mcp2515_configRate (const uint8_t canSpeed)
{
  uint8_t set, cfg1, cfg2, cfg3;

  set = 1;

  switch (canSpeed) {
    case (CAN_5KBPS):
    cfg1 = MCP_16MHz_5kBPS_CFG1;
    cfg2 = MCP_16MHz_5kBPS_CFG2;
    cfg3 = MCP_16MHz_5kBPS_CFG3;
    break;

    case (CAN_10KBPS):
    cfg1 = MCP_16MHz_10kBPS_CFG1;
    cfg2 = MCP_16MHz_10kBPS_CFG2;
    cfg3 = MCP_16MHz_10kBPS_CFG3;
    break;

    case (CAN_20KBPS):
    cfg1 = MCP_16MHz_20kBPS_CFG1;
    cfg2 = MCP_16MHz_20kBPS_CFG2;
    cfg3 = MCP_16MHz_20kBPS_CFG3;
    break;

    case (CAN_40KBPS):
    cfg1 = MCP_16MHz_40kBPS_CFG1;
    cfg2 = MCP_16MHz_40kBPS_CFG2;
    cfg3 = MCP_16MHz_40kBPS_CFG3;
    break;

    case (CAN_50KBPS):
    cfg1 = MCP_16MHz_50kBPS_CFG1;
    cfg2 = MCP_16MHz_50kBPS_CFG2;
    cfg3 = MCP_16MHz_50kBPS_CFG3;
    break;

    case (CAN_80KBPS):
    cfg1 = MCP_16MHz_80kBPS_CFG1;
    cfg2 = MCP_16MHz_80kBPS_CFG2;
    cfg3 = MCP_16MHz_80kBPS_CFG3;
    break;

    case (CAN_100KBPS):
    cfg1 = MCP_16MHz_100kBPS_CFG1;
    cfg2 = MCP_16MHz_100kBPS_CFG2;
    cfg3 = MCP_16MHz_100kBPS_CFG3;
    break;

    case (CAN_125KBPS):
    cfg1 = MCP_16MHz_125kBPS_CFG1;
    cfg2 = MCP_16MHz_125kBPS_CFG2;
    cfg3 = MCP_16MHz_125kBPS_CFG3;
    break;

    case (CAN_200KBPS):
    cfg1 = MCP_16MHz_200kBPS_CFG1;
    cfg2 = MCP_16MHz_200kBPS_CFG2;
    cfg3 = MCP_16MHz_200kBPS_CFG3;
    break;

    case (CAN_250KBPS):
    cfg1 = MCP_16MHz_250kBPS_CFG1;
    cfg2 = MCP_16MHz_250kBPS_CFG2;
    cfg3 = MCP_16MHz_250kBPS_CFG3;
    break;

    case (CAN_500KBPS):
    cfg1 = MCP_16MHz_500kBPS_CFG1;
    cfg2 = MCP_16MHz_500kBPS_CFG2;
    cfg3 = MCP_16MHz_500kBPS_CFG3;
    break;

    default:
    set = 0;
    break;
  }

  if (set) {
    mcp2515_setRegister (MCP_CNF1, cfg1);
    mcp2515_setRegister (MCP_CNF2, cfg2);
    mcp2515_setRegister (MCP_CNF3, cfg3);
    return MCP2515_OK;
  } else {
    return MCP2515_FAIL;
  }
}

static void
mcp2515_write_id (const uint8_t mcp_addr, const uint8_t ext,
                  const uint32_t id)
{
  uint16_t canid;
  uint8_t tbufdata[4];

  canid = (uint16_t)(id & 0x0FFFF);

  if (ext == 1) {
    tbufdata[MCP_EID0] = (uint8_t) (canid & 0xFF);
    tbufdata[MCP_EID8] = (uint8_t) (canid / 256);
    canid = (uint16_t) (id / 0x10000L);
    tbufdata[MCP_SIDL] = (uint8_t) (canid & 0x03);
    tbufdata[MCP_SIDL] += (uint8_t) ((canid & 0x1C) * 8);
    tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
    tbufdata[MCP_SIDH] = (uint8_t) (canid / 32);
  } else {
    tbufdata[MCP_SIDH] = (uint8_t) (canid / 8);
    tbufdata[MCP_SIDL] = (uint8_t) ((canid & 0x07) << 5);
    tbufdata[MCP_EID0] = 0;
    tbufdata[MCP_EID8] = 0;
  }

  mcp2515_setRegisterS (mcp_addr, tbufdata, 4);
}

static void
mcp2515_initCANBuffers (void)
{
  uint8_t i, a1, a2, a3;
  uint8_t ext = 0;

  uint32_t ulMask = 0x00, ulFilt = 0x00;

  mcp2515_write_id(MCP_RXM0SIDH, ext, ulMask);
  mcp2515_write_id(MCP_RXM1SIDH, ext, ulMask);
  /* Anyway, set all filters to 0 */
  /* RXB0: extended */
  mcp2515_write_id(MCP_RXF0SIDH, ext, ulFilt);
  /* AND standard */
  mcp2515_write_id(MCP_RXF1SIDH, ext, ulFilt);
  /* RXB1: extended */
  mcp2515_write_id(MCP_RXF2SIDH, ext, ulFilt);
  /* AND standard */
  mcp2515_write_id(MCP_RXF3SIDH, ext, ulFilt);
  mcp2515_write_id(MCP_RXF4SIDH, ext, ulFilt);
  mcp2515_write_id(MCP_RXF5SIDH, ext, ulFilt);

  /* Clear, deactivate the three  */
  /* transmit buffers             */
  /* TXBnCTRL -> TXBnD7           */
  a1 = MCP_TXB0CTRL;
  a2 = MCP_TXB1CTRL;
  a3 = MCP_TXB2CTRL;

  for (i = 0; i < 14; i++) {
    mcp2515_setRegister (a1, 0);
    mcp2515_setRegister (a2, 0);
    mcp2515_setRegister (a3, 0);
    a1++;
    a2++;
    a3++;
  }
  mcp2515_setRegister (MCP_RXB0CTRL, 0);
  mcp2515_setRegister (MCP_RXB1CTRL, 0);
}

static uint8_t
mcp2515_getNextFreeTXBuf (uint8_t *txbuf_n)
{
  uint8_t res, i, ctrlval;
  uint8_t ctrlregs[MCP_N_TXBUFFERS] =
    {MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL};

  res = MCP_ALLTXBUSY;
  *txbuf_n = 0x00;

  /* check all 3 TX-Buffers       */
  for (i = 0; i < MCP_N_TXBUFFERS; i++) {
    ctrlval = mcp2515_readRegister (ctrlregs[i]);
    if ((ctrlval & MCP_TXB_TXREQ_M) == 0) {
      *txbuf_n = ctrlregs[i]+1;
      res = MCP2515_OK;
      return res;
    }
  }

  return res;
}

static void
mcp2515_write_canMsg (const uint8_t buffer_sidh_addr,
                      uint32_t m_nID, uint8_t m_nExtFlg,
                      uint8_t m_nDlc, uint8_t * m_nDta,
                      uint8_t m_nRtr)
{
  uint8_t mcp_addr;
  mcp_addr = buffer_sidh_addr;
  mcp2515_setRegisterS (mcp_addr + 5, m_nDta, m_nDlc);
  if (m_nRtr == 1) {
    m_nDlc |= MCP_RTR_MASK;
  }

  /* write the RTR and DLC */
  mcp2515_setRegister ((mcp_addr + 4), m_nDlc);
  /* write CAN id */
  mcp2515_write_id (mcp_addr, m_nExtFlg, m_nID);

}

static void
mcp2515_start_transmit (const uint8_t mcp_addr)
{
  mcp2515_modifyRegister (mcp_addr - 1 , MCP_TXB_TXREQ_M,
                          MCP_TXB_TXREQ_M);
}

CanTxStatusType
CanSendMsg (uint32_t id, uint8_t ext,
            uint8_t len, uint8_t * buf)
{
  uint8_t res, res1, txbuf_n;
  uint16_t uiTimeOut = 0;

  do {
    res = mcp2515_getNextFreeTXBuf (&txbuf_n);
    uiTimeOut++;
  } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

  if (uiTimeOut == TIMEOUTVALUE) {
    return CAN_TX_STATUS_NOMAILBOX;
  }

  uiTimeOut = 0;
  mcp2515_write_canMsg (txbuf_n, id, ext, len, buf, 0);
  mcp2515_start_transmit (txbuf_n);

  do {
    uiTimeOut++;
    res1= mcp2515_readRegister (txbuf_n);
    res1 = res1 & 0x08;
  } while(res1 && (uiTimeOut < TIMEOUTVALUE));

  if (uiTimeOut == TIMEOUTVALUE) {
    return 2;
  }

  return CAN_TX_STATUS_SUCCESS;
}

void
mcp2515_init (void)
{
  uint8_t res = 0;

  DEBUG_PRINTFV ("Initializing MCP2515...\n");

  /* Enable SS pin */
  GPIOEnable (MCP2515_SS_GPIO_PORT);
  GPIOSetMode (MCP2515_SS_GPIO_PORT, MCP2515_SS_GPIO_PIN,
               GPIO_MODE_OUTPUT);
  GPIOSetOutput (MCP2515_SS_GPIO_PORT, MCP2515_SS_GPIO_PIN,
                 GPIO_OUTPUT_OD);
  GPIOSetSpeed (MCP2515_SS_GPIO_PORT, MCP2515_SS_GPIO_PIN,
                 GPIO_FAST_SPEED);
  GPIOSetPUPD (MCP2515_SS_GPIO_PORT, MCP2515_SS_GPIO_PIN,
               GPIO_PULLUP);

  MCP2515_UNSELECT ();
  mcp2515_reset ();

  /* Enter setting mode */
  res = mcp2515_setCANCTRL_Mode (MODE_CONFIG);

  if (res > 0) {
    DEBUG_PRINTF ("Enter init mode failed\n");
    return;
  }

  DEBUG_PRINTF ("MCP2515 in init mode\n");

  /* Configure CAN baudrate */
  if (mcp2515_configRate (CAN_500KBPS)) {
    DEBUG_PRINTF ("CAN baudrate init failed\n");
    return;
  }

  /* Initialize buffers */
  mcp2515_initCANBuffers ();

  /* Interrupt mode */
  mcp2515_setRegister (MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);

  mcp2515_modifyRegister (MCP_RXB0CTRL,
                          MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
                          MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK );

  mcp2515_modifyRegister (MCP_RXB1CTRL, MCP_RXB_RX_MASK,
                          MCP_RXB_RX_STDEXT);

  /* Leaving setting mode */
  res = mcp2515_setCANCTRL_Mode (MODE_NORMAL);

  if (res) {
    DEBUG_PRINTF ("Leaving init mode failed\n");
    return;
  }

  DEBUG_PRINTF ("MCP2515 initialized\n");
}

/* Internal message structure */
static CanMsgType IntCanMsg;
/* User registered receive callback function */
static RecvCallbackFunc UsrCallbackFun = NULL;

RecvCallbackFunc
CanRecvMsgIT (RecvCallbackFunc CallbackFunc)
{
  RecvCallbackFunc prev_callback = UsrCallbackFun;

  UsrCallbackFun = CallbackFunc;

  return prev_callback;
}

static void
mcp2515_read_id (const uint8_t mcp_addr, uint8_t * ext, uint32_t * id )
{
  uint8_t tbufdata[4];

  *ext = 0;
  *id = 0;

  mcp2515_readRegisterS (mcp_addr, tbufdata, 4);

  *id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

  if ((tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M) {
    /* Extended id */
    *id = (*id << 2) + (tbufdata[MCP_SIDL] & 0x03);
    *id <<= 16;
    *id = *id +(tbufdata[MCP_EID8] << 8) + tbufdata[MCP_EID0];
    *ext = 1;
  }
}

static void
mcp2515_read_canMsg (const uint8_t buffer_sidh_addr, CanMsgType * CanMsg)
{
  uint8_t mcp_addr, ctrl, ext;

  mcp_addr = buffer_sidh_addr;

  mcp2515_read_id (mcp_addr, &ext, &(CanMsg->id));
  CanMsg->ide = ext;

  ctrl = mcp2515_readRegister (mcp_addr - 1);
  CanMsg->length = mcp2515_readRegister (mcp_addr + 4) & MCP_DLC_MASK;

  (ctrl & 0x80) ? (CanMsg->remote = 1) :
                  (CanMsg->remote = 0);

  mcp2515_readRegisterS (mcp_addr + 5, &(CanMsg->data[0]),
                         CanMsg->length);
}

static uint8_t
mcp2515_readStatus (void)
{
  uint8_t status;

  MCP2515_SELECT ();
  spi_readwrite (MCP_READ_STATUS);
  status = spi_read ();
  MCP2515_UNSELECT ();

  return status;
}

int8_t
CanRecvMsg (CanMsgType * CanMsg)
{
  uint8_t stat, res;

  stat = mcp2515_readStatus ();

  if (stat & MCP_STAT_RX0IF) {
    /* Msg available in Buffer 0 */
    mcp2515_read_canMsg (MCP_RXBUF_0, CanMsg);
    mcp2515_modifyRegister (MCP_CANINTF, MCP_RX0IF, 0);
    res = 0;
  } else if (stat & MCP_STAT_RX1IF) {
    /* Msg available in Buffer 1 */
    mcp2515_read_canMsg (MCP_RXBUF_1, CanMsg);
    mcp2515_modifyRegister (MCP_CANINTF, MCP_RX1IF, 0);
    res = 0;
  } else {
    res = -1;
  }

  return res;
}

/*
 * MCP2515 interrupt is connected to pin 2 (PA 10).
 * We connect it to PA 8 instead to avoid sharing the
 * EXTI15_10 line. PA 8 is Arduino D7.
 */
ISR (ext_vec_23)
{
  /* Acknowledge interrupt, clear pending. */
  EXTI->PR |= (0x1 << 8);

  if (CanRecvMsg (&IntCanMsg) >= 0) {
    if (UsrCallbackFun) {
      /* Invoke user callback function */
      (*UsrCallbackFun) (&IntCanMsg);
    }
  }
}

/* vi: set et ai sw=2 sts=2: */
