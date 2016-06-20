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
 * @file   src/drivers/can/stm32f4xx_bxcan.c
 * @author Ye Li (liye@sdvos.org)
 * @brief Basic Extended Can Controller Driver for STM32F4xx
 */
#include <sdvos.h>
#include <board.h>
#include <drivers/nucleof4/gpio.h>
#include <drivers/can.h>

/*
 * CAN1 GPIO pins on STM32F4 Discovery
 *   PD0 - CAN1 RX (AF9)
 *   PD1 - CAN1 TX (AF9)
 */

/** bxCAN GPIO Port */
#define CAN_GPIO_PORT    D
/** bxCAN RX GPIO Pin */
#define CAN_RX_PIN       0
/** bxCAN TX GPIO Pin */
#define CAN_TX_PIN       1
/** bxCAN GPIO Alternate Function */
#define CAN_GPIO_AF      GPIO_AF9
/** bxCAN RX0 Interrupt Vector Number */
#define CAN_RX0_VECTOR   20

/* quanta 1+6+7 = 14, 14 * 3 = 42, 42000000 / 42 = 1000000 */
/* CAN Baudrate = 1Mbps (CAN clocked at 42 MHz) Prescale = 3 */
/* CAN Baudrate = 500kbps (CAN clocked at 42 MHz) Prescale = 6 */
/* CAN Baudrate = 250kbps (CAN clocked at 42 MHz) Prescale = 12 */

extern uint32_t APB1Clock;

/** CAN Baud Rate (500kbps default) */
#define CAN_BPS          500000

/** Baud Rate Prescaler */
#define BRP_VAL          \
  (APB1Clock / CAN_BPS / (1 + TS1_TQ + TS2_TQ))

/** Time Segment 1 Time Quanta (Prop + Phase1) */
#define TS1_TQ           6
/** Time Segment 2 Time Quanta (Phase2) */
#define TS2_TQ           7
/** Resynchronization Jump Width */
#define SJW_TQ           1

/* Number of filters assigned to CAN1 */
#define NUM_CAN1_FILTERS    0xE

/** Enable CAN1 */
#define CAN1_CLK_ENABLE()    \
  (RCC->APB1ENR |= (RCC_APB1ENR_CAN1EN))

/** CAN1 Reset */
#define CAN1_FORCE_RESET()    \
  (RCC->APB1RSTR |= (RCC_APB1RSTR_CAN1RST))

/** CAN1 Release Reset */
#define CAN1_RELEASE_RESET()    \
  (RCC->APB1RSTR &= ~(RCC_APB1RSTR_CAN1RST))

CanTxStatusType
CanSendMsg (uint32_t id, uint8_t ext,
            uint8_t len, uint8_t * buf)
{
  int8_t tx_mail_box = -1;
  uint8_t data[8];

  sdvos_memset (data, 0, 8);
  sdvos_memcpy (data, buf, len & 0xF);

  /* Find an empty mailbox */
  if (CAN1->TSR & CAN_TSR_TME0) {
    tx_mail_box = 0;
  } else if (CAN1->TSR & CAN_TSR_TME1) {
    tx_mail_box = 1;
  } else if (CAN1->TSR & CAN_TSR_TME2) {
    tx_mail_box = 2;
  } else {
    /* No mailbox left */
    return CAN_TX_STATUS_NOMAILBOX;
  }

  /* Clear mailbox */
  CAN1->sTxMailBox[tx_mail_box].TIR = 0x0;

  if (ext == CAN_EXTID) {
    CAN1->sTxMailBox[tx_mail_box].TIR |= ((id << 3) |
                                          CAN_TI0R_IDE);
  } else {
    CAN1->sTxMailBox[tx_mail_box].TIR |= (id << 21);
  }

  CAN1->sTxMailBox[tx_mail_box].TDTR &= 0xFEF0;
  /* Update DLC */
  CAN1->sTxMailBox[tx_mail_box].TDTR |= (len & 0xF);
  /* Clear TGT */
  CAN1->sTxMailBox[tx_mail_box].TDTR &= (~(uint32_t) CAN_TDT0R_TGT);

  /* Fill in data */
  CAN1->sTxMailBox[tx_mail_box].TDLR = data[3] << 24 |
    data[2] << 16 | data[1] << 8 | data[0];

  CAN1->sTxMailBox[tx_mail_box].TDHR = data[7] << 24 |
    data[6] << 16 | data[5] << 8 | data[4];

  /* Request Tx */
  CAN1->sTxMailBox[tx_mail_box].TIR |= CAN_TI0R_TXRQ;

  /*
   * Automatic retransmission is enabled by default (~NART).
   * As a result, we just return CAN_TX_STATUS_SUCCESS at
   * this point. If this is not the case, we should also
   * check RQCP_x, TXOK_x, and TME_x bits in CAN->TSR.
   * We should also check CAN->ESR for error code.
   */

  return CAN_TX_STATUS_SUCCESS;
}

void
stm32f4xx_bxcan_init (void)
{
  DEBUG_PRINTFV ("Initializing bxCAN...\n");

  /* Enable CAN1 clock */
  CAN1_CLK_ENABLE ();

  CAN1_FORCE_RESET ();
  CAN1_RELEASE_RESET ();

  /* Enable GPIO Port */
  GPIOEnable (CAN_GPIO_PORT);

  /* Initialize CAN RX pin */
  GPIOAlternateFunction (CAN_GPIO_PORT, CAN_RX_PIN,
                         CAN_GPIO_AF);
  GPIOSetMode (CAN_GPIO_PORT, CAN_RX_PIN, GPIO_MODE_AF);
  GPIOSetOutput (CAN_GPIO_PORT, CAN_RX_PIN, GPIO_OUTPUT_PP);
  GPIOSetSpeed (CAN_GPIO_PORT, CAN_RX_PIN, GPIO_FAST_SPEED);
  GPIOSetPUPD (CAN_GPIO_PORT, CAN_RX_PIN, GPIO_PULLUP);

  /* Initialize CAN TX pin */
  GPIOAlternateFunction (CAN_GPIO_PORT, CAN_TX_PIN,
                         CAN_GPIO_AF);
  GPIOSetMode (CAN_GPIO_PORT, CAN_TX_PIN, GPIO_MODE_AF);
  GPIOSetOutput (CAN_GPIO_PORT, CAN_TX_PIN, GPIO_OUTPUT_PP);
  GPIOSetSpeed (CAN_GPIO_PORT, CAN_TX_PIN, GPIO_FAST_SPEED);
  GPIOSetPUPD (CAN_GPIO_PORT, CAN_TX_PIN, GPIO_PULLUP);

  /* Exit sleep mode */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_SLEEP);

  /* Initialization request */
  CAN1->MCR |= CAN_MCR_INRQ;

  /* Wait for acknowledgement */
  while ((CAN1->MSR & CAN_MSR_INAK) != CAN_MSR_INAK);

  DEBUG_PRINTFV ("bxCAN in init mode\n");

  /* Disable time triggered communication mode */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_TTCM);
  /* Disable Tx FIFO Priority. Use ID priority. */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_TXFP);
  /* Allow retransmission */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_NART);
  /* Disable Rx FIFO lock. Old messages will be discarded. */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_RFLM);
  /* Disable automatic wakeup */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_AWUM);
  /* Disable automatic bus-off management */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_ABOM);

  /* Interrupt Configuration */
  CAN1->IER = 0;
  /* FIFO message pending interrupt enable */
  CAN1->IER |= CAN_IER_FMPIE0;

  /* Filter configuration */
  /* Assign 14 filters to CAN1 and enter filter init mode */
  CAN1->FMR &= (~(uint32_t) CAN_FMR_CAN2SB);
  CAN1->FMR |= ((NUM_CAN1_FILTERS << 8) | CAN_FMR_FINIT);
  /* All filters in mask mode by default */
  CAN1->FM1R &= 0xF0000000;
  /* All filters in single 32-bit scale */
  CAN1->FS1R |= 0x0FFFFFFF;
  /* All filters assigned to FIFO0 */
  CAN1->FFA1R &= 0xF0000000;
  /* Enable filter 0. Only 1 filter is used by default. */
  CAN1->FA1R &= 0xF0000000;
  CAN1->FA1R |= 0x1;
  /* Configure filter bank 0. Allow all messages. */
  CAN1->sFilterRegister[0].FR1 = 0x0;
  CAN1->sFilterRegister[0].FR2 = 0x0;
  /* Exit filter init mode */
  CAN1->FMR &= (~(uint32_t) CAN_FMR_FINIT);

  /* Disable silent and loopback mode, set prescaler */
  CAN1->BTR = (((TS1_TQ & 0xF) - 1) << 16) |
              (((TS2_TQ & 0x7) - 1) << 20) |
              (((SJW_TQ & 0x3) - 1) << 24) |
              (BRP_VAL - 1);

  /* Leave initialization mode */
  CAN1->MCR &= (~(uint32_t) CAN_MCR_INRQ);

  /* Wait for acknowledgement */
  while ((CAN1->MSR & CAN_MSR_INAK) == CAN_MSR_INAK);

  /*
   * NVIC RX interrupt initialization. This will only be
   * triggered if the user did not add ISR in the OIL
   * configuration file for the CAN RX IRQ. A user should
   * always add the ISR entry and specify the ISR category.
   * By default, CAN RX ISR will be set to ISR2.
   */
  if ((NVIC->IP[CAN_RX0_VECTOR] != ISR1_PRI) &&
      (NVIC->IP[CAN_RX0_VECTOR] != ISR2_PRI)) {
    /* Interrupt not initialized */
    DEBUG_PRINTF ("IRQ not initialized!\n");
    NVIC_SetPriority (CAN_RX0_VECTOR, ISR2_PRI);
    NVIC_EnableIRQ (CAN_RX0_VECTOR);
  }

  DEBUG_PRINTF ("bxCan initialized\n");
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

int8_t
CanRecvMsg (CanMsgType * CanMsg)
{
  int i = 0;

  if (! CanMsg) goto release_fifo;

  if (CAN1->RF0R & CAN_RF0R_FMP0) {
    /* Pending message exists */
    /* Fill in CanMsg */
    if (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_IDE) {
      /* Extended ID */
      CanMsg->id = (CAN1->sFIFOMailBox[0].RIR >> 3) & 0x1FFFFFFF;
      CanMsg->ide = 1;
    } else {
      /* Standard ID */
      CanMsg->id = (CAN1->sFIFOMailBox[0].RIR >> 21) & 0x7FF;
      CanMsg->ide = 0;
    }

    CanMsg->remote = (CAN1->sFIFOMailBox[0].RIR & CAN_RI0R_RTR) >> 1;
    CanMsg->length = CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_DLC;
    CanMsg->time = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_TIME) >> 16;
    CanMsg->fmi = (CAN1->sFIFOMailBox[0].RDTR & CAN_RDT0R_FMI) >> 8;

    sdvos_memset (CanMsg->data, 0, 8);

    /* Copy message payload */
    for (i = 0; i < 4 && i < CanMsg->length; i++) {
      CanMsg->data[i] = (CAN1->sFIFOMailBox[0].RDLR >> (i << 3)) & 0xFF;
    }

    for (i = 4; i < 8 && i < CanMsg->length; i++) {
      CanMsg->data[i] =
        (CAN1->sFIFOMailBox[0].RDHR >> ((i - 4) << 3)) & 0xFF;
    }
  } else {
    /* No pending messages */
    return -1;
  }

release_fifo:
  /* Release FIFO */
  CAN1->RF0R |= CAN_RF0R_RFOM0;
  return (CAN1->RF0R & CAN_RF0R_FMP0);
}

/* CAN1 RX0 Vector */
ISR (ext_vec_20)
{
  if (CanRecvMsg (&IntCanMsg) >= 0) {
    if (UsrCallbackFun) {
      /* Invoke user callback function */
      (*UsrCallbackFun) (&IntCanMsg);
    }
  }
}

/* vi: set et ai sw=2 sts=2: */
