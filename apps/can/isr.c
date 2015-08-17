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

ISR (ext_vec_6)
{
  /* Acknowledge interrupt, clear pending. */
  EXTI->PR |= 0x1;

  /* Set event for task1 */
  SetEvent (task1, event1);
}

void
StartupHook ()
{
  /* Push button connected to PA0 */
  GPIOEnable (A);
  GPIOSetMode (A, 0, GPIO_MODE_INPUT);
  GPIOSetSpeed (A, 0, GPIO_FAST_SPEED);
  GPIOSetPUPD (A, 0, GPIO_PULLDOWN);

  /* Enable SYSCFG. Needed for EXTI interrupt */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* EXTI0 for PA0 */
  SYSCFG->EXTICR[0] &= ~((uint32_t) 0xF);

  /* Clear interrupt mask for line 0 (MR0) */
  EXTI->IMR |= 0x1;
  /* Enable rising trigger (button push event) */
  EXTI->RTSR |= 0x1;
  /* Clear potential pending interrupt */
  EXTI->PR |= 0x1;

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
