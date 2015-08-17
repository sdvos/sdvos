#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>
#include <drivers/nucleof4/gpio.h>

/* LED mode variable */
int LEDMode = 0;

void
ErrorHook (StatusType e)
{
  DEBUG_PRINTF ("Error: (%d)\n", OSErrorGetServiceId ());
}

void
StartupHook ()
{
  /* PA5 is LED1 (output, push-pull, fast, pull-up) */
  GPIOEnable (A);
  GPIOSetMode (A, 5, GPIO_MODE_OUTPUT);
  GPIOSetOutput (A, 5, GPIO_OUTPUT_PP);
  GPIOSetSpeed (A, 5, GPIO_FAST_SPEED);
  GPIOSetPUPD (A, 5, GPIO_PULLUP);

  /* PC13 is button (input, fast, pull-up) */
  GPIOEnable (C);
  GPIOSetMode (C, 13, GPIO_MODE_INPUT);
  GPIOSetSpeed (C, 13, GPIO_FAST_SPEED);
  GPIOSetPUPD (C, 13, GPIO_PULLUP);

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
}

ISR_CAT1 (ext_vec_40)
{
  /* Acknowledge interrupt, clear pending. */
  EXTI->PR |= (0x1 << 13);

  /*
   * Set LED mode:
   *   0 - Off
   *   1 - 1 second interval
   *   2 - 0.5 second interval
   *   3 - On
   */
  if (LEDMode < 3) {
    if ((++LEDMode) == 3) {
      GPIODigitalWrite (A, 5, 1);
    }
  } else {
    LEDMode = 0;
    GPIODigitalWrite (A, 5, 0);
  }
  sdvos_printf ("LED Mode %d\n", LEDMode);
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
