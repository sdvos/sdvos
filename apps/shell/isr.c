#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>

void
ALARM0_CALLBACK ()
{
  sdvos_printf ("Callback\n");
}

void
ErrorHook (StatusType e)
{
  DEBUG_PRINTF ("Error: (%d)\n", OSErrorGetServiceId ());
}

void
StartupHook ()
{
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
