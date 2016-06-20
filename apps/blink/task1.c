#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>
#include <board.h>
#include <drivers/nucleof4/gpio.h>

/*
 * LED mode:
 *   0 - Off
 *   1 - 1 second interval
 *   2 - 0.5 second interval
 *   3 - On
 */
extern int LEDMode;

TASK (task1)
{
  if (LEDMode == 1) {
    GPIODigitalToggle (A, 5);
  }

  TerminateTask ();

  return E_OK;
}

TASK (task2)
{
  if (LEDMode == 2) {
    GPIODigitalToggle (A, 5);
  }

  TerminateTask ();

  return E_OK;
}

/* vi: set et ai sw=2 sts=2: */
