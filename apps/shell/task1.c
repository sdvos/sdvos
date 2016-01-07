#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <sdvos_printf.h>

DeclareTask (task1);

TASK (task1)
{
  sdvos_printf ("Hello from task1\n");

  TerminateTask ();

  return E_OK;
}

/* vi: set et ai sw=2 sts=2: */
