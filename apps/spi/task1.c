#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>
#include <board.h>
#include <drivers/nucleof4/gpio.h>
#include <drivers/can.h>

uint8_t buf[8] = {'S', 'D', 'V', 'O', 'S', 'C', 'A', 'N'};

DeclareEvent (event1);

TASK (task1)
{
  while (1) {
    WaitEvent (event1);

    sdvos_printf ("Sending message on CAN bus...\n");

    if (CanSendMsg (0x0, CAN_STID, 8, buf) !=
        CAN_TX_STATUS_SUCCESS) {
      sdvos_printf ("CanSendMsg failed!\n");
    }

    ClearEvent (event1);
  }

  TerminateTask ();

  return E_OK;
}

/* vi: set et ai sw=2 sts=2: */
