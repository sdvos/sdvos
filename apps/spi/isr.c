#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>
#include <drivers/nucleof4/gpio.h>
#include <drivers/can.h>

void
ErrorHook (StatusType e)
{
  DEBUG_PRINTF ("Error: (%d)\n", OSErrorGetServiceId ());
}

void
CanRecvCallback (CanMsgType * CanMsg)
{
  int i = 0;

  DEBUG_PRINTF ("Message: ID=0x%X (%s), LENGTH=%d\n",
                CanMsg->id, (CanMsg->ide) ? "ext" : "std",
                CanMsg->length);
  DEBUG_PRINTF ("         REMOTE=%d, FMI=0x%X, TIME=0x%X\n",
                CanMsg->remote, CanMsg->fmi, CanMsg->time);
  DEBUG_PRINTF ("Data: ");
  for (i = 0; i < CanMsg->length; i++) {
    DEBUG_PRINTF ("%2X ", CanMsg->data[i]);
  }
  DEBUG_PRINTF ("\n");
}

ISR (ext_vec_40)
{
  /* Acknowledge interrupt, clear pending. */
  EXTI->PR |= (0x1 << 13);

  /* Set event for task1 */
  SetEvent (task1, event1);
}

void
StartupHook ()
{
  /* PC13 is button (input, fast, pull-up) */
  GPIOEnable (C);
  GPIOSetMode (C, 13, GPIO_MODE_INPUT);
  GPIOSetSpeed (C, 13, GPIO_FAST_SPEED);
  GPIOSetPUPD (C, 13, GPIO_PULLUP);

  /* MCP2515 interrupt routed to PA 8 */
  GPIOEnable (A);
  GPIOSetMode (A, 8, GPIO_MODE_INPUT);
  GPIOSetSpeed (A, 8, GPIO_FAST_SPEED);
  GPIOSetPUPD (A, 8, GPIO_PULLUP);

  /* Enable SYSCFG. Needed for EXTI interrupt */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* EXTI15_10 vector */
  /* EXTICR4, EXIT13, PC[X] pin (0010) */
  SYSCFG->EXTICR[3] &= ~((uint32_t)(0xF << 4));
  SYSCFG->EXTICR[3] |= ((uint32_t)(0x2 << 4));

  /* Clear interrupt mask for line 13 (MR13) */
  EXTI->IMR |= (0x1 << 13);
  /* Enable falling trigger (button push event) */
  EXTI->FTSR |= (0x1 << 13);
  /* Clear potential pending interrupt */
  EXTI->PR |= (0x1 << 13);

  /* EXTI9_5 vector */
  /* EXTICR3, EXIT8, PA[X] pin (0000) */
  SYSCFG->EXTICR[2] &= ~((uint32_t) 0xF);

  /* Clear interrupt mask for line 8 (MR8) */
  EXTI->IMR |= (0x1 << 8);
  /* Enable falling trigger (button push event) */
  EXTI->FTSR |= (0x1 << 8);
  /* Clear potential pending interrupt */
  EXTI->PR |= (0x1 << 8);

  /* Register CAN receive callback function */
  CanRecvMsgIT (CanRecvCallback);
}

int
main (void)
{
  StartOS (OSDEFAULTAPPMODE);

  /* Should not reach here */
  while (1) {};

  return 0;
}

/* vi: set et ai sw=2 sts=2: */
