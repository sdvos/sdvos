#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>

DeclareTask (task1);

TickType cur = 0;
TickType elapsed = 0;

TASK (task1)
{
  GetCounterValue (SYS_COUNTER, &cur);
  Delay (SYS_COUNTER, 1000);
  GetElapsedValue (SYS_COUNTER, &cur, &elapsed);
  sdvos_printf ("Time elapsed: %d\n", elapsed);

  GetCounterValue (SYS_COUNTER, &cur);
  Delay (SYS_COUNTER, 1000);
  GetElapsedValue (SYS_COUNTER, &cur, &elapsed);
  sdvos_printf ("Time elapsed: %d\n", elapsed);

  GetCounterValue (SYS_COUNTER, &cur);
  Delay (SYS_COUNTER, 1000);
  GetElapsedValue (SYS_COUNTER, &cur, &elapsed);
  sdvos_printf ("Time elapsed: %d\n", elapsed);

  GetCounterValue (SYS_COUNTER, &cur);
  Delay (SYS_COUNTER, 1000);
  GetElapsedValue (SYS_COUNTER, &cur, &elapsed);
  sdvos_printf ("Time elapsed: %d\n", elapsed);

  GetCounterValue (SYS_COUNTER, &cur);
  Delay (SYS_COUNTER, 1000);
  GetElapsedValue (SYS_COUNTER, &cur, &elapsed);
  sdvos_printf ("Time elapsed: %d\n", elapsed);

  TerminateTask ();

  return E_OK;
}

/* vi: set et ai sw=2 sts=2: */
