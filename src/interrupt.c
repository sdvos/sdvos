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
 * @file   src/interrupt.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Interrupt Management
 */
#include <osek/interrupt.h>
#include <sdvos.h>

uatomic_t NestedISRs = ATOMIC_INIT (0);

/** Counter used for DisableAllInterrupts call count */
static IntCntType DisableAllCnt = 0;
/** Counter used for SuspendAllInterrupts call count */
static IntCntType SuspendAllCnt = 0;
/** Counter used for SuspendOSInterrupts call count */
static IntCntType SuspendOSCnt = 0;

/*
 * DisableAllCnt can only be 0 or 1. DisableAllInterrupts
 * disables all interrupts before increasing the counter.
 */
void
Sys_EnableAllInterrupts ()
{
  /* In case no DisableAllInterrupts is called */
  /* Disable interrupts to avoid contention */
  ArchDisableAllInterrupts ();
  if (DisableAllCnt) DisableAllCnt--;
  ArchEnableAllInterrupts ();
}

void
Sys_DisableAllInterrupts ()
{
  ArchDisableAllInterrupts ();
  DisableAllCnt++;
  /* Enable/DisableAllInterrupts cannot be nested */
  if (DisableAllCnt != 1) {
    DEBUG_PRINTFV ("Nested EnableAllInterrupts!\n");
    panic ();
  }
}

/*
 * Suspend/ResumeAllInterrupts pair need to be protected
 * by disabling all interrupts. This is not necessary for
 * some architectures (e.g. ARM Cortex M3) where these
 * services are treated as system calls which disables all
 * interrupts automatically upon entering. However, to be
 * generic, we assume such facility does not exist.
 */
void
Sys_ResumeAllInterrupts ()
{
  /* Disable all interrupts to avoid contention */
  ArchSuspendAllInterrupts ();

  if (SuspendAllCnt) SuspendAllCnt--;

  /* Nested SuspendAllInterrupts? */
  if (SuspendAllCnt) {
    /* Do nothing */
    /* All interrupt remain suspended */
  } else {
    /* In SuspendOSInterrupts CS? */
    if (SuspendOSCnt) {
      ArchSuspendOSInterrupts ();
    } else {
      ArchResumeOSInterrupts ();
    }
  }
}

void
Sys_SuspendAllInterrupts ()
{
  ArchSuspendAllInterrupts ();
  SuspendAllCnt++;
}

/*
 * Suspend/ResumeOSInterrupts pair is similar to
 * Suspend/ResumeAllInterrupts pair.
 */
void
Sys_ResumeOSInterrupts ()
{
  /* First suspend all interrupts to avoid contention */
  ArchSuspendAllInterrupts ();

  if (SuspendOSCnt) SuspendOSCnt--;

  /* In SuspendAllInterrupts CS? */
  if (SuspendAllCnt) {
    /* Do nothing */
    /* All interrupt remain suspended */
  } else {
    /* Nested SuspendOSInterrupts? */
    if (SuspendOSCnt) {
      ArchSuspendOSInterrupts ();
    } else {
      ArchResumeOSInterrupts ();
    }
  }
}

void
Sys_SuspendOSInterrupts ()
{
  /* First suspend all interrupts to avoid contention */
  ArchSuspendAllInterrupts ();

  SuspendOSCnt++;
  /* Are we in SuspendAllInterrupts CS? */
  if (SuspendAllCnt) {
    /* Do nothing */
  } else {
    ArchSuspendOSInterrupts ();
  }
}

/* vi: set et ai sw=2 sts=2: */
