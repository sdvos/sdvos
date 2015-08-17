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
 * @file   src/osctl.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Operating System Execution Control
 */
#include <osek/osek.h>
#include <config/config.h>
#include <sdvos.h>

/** Application mode of OS */
AppModeType sdvos_appmode = OSDEFAULTAPPMODE;

AppModeType
Sys_GetActiveApplicationMode ()
{
  return sdvos_appmode;
}

/**
 * @brief Activate auto start tasks
 *
 * TaskAutoStart activates all auto start tasks according
 * to a given application mode.
 *
 * @param[in] mode
 *   Application mode
 */
static void
TaskAutoStart (AppModeType mode)
{
  int i = 0;
  TaskType tid = 0;

  /* Activate all auto start tasks for this appmode */
  for (i = 0; i < NUM_TASKS; i++) {
    if ((tid = auto_tasks[sdvos_appmode][i]) ==
        INVALID_TASK) break;
    Sys_ActivateTask (tid);
    DEBUG_PRINTFV ("Task %d auto started!\n", tid);
  }
}

#ifdef DEBUG_SDVOS_VERBOSE
/**
 * @brief Perform some integrity and sanity check
 */
static void
IntegrityCheck ()
{
  /* Check packing of TCB key fields */
  if (!TCB_KEY_FIELDS_PACKED ()) {
    DEBUG_PRINTFV ("TCB key fields not packed!\n");
    panic ();
  }
}
#endif

void
StartOS (AppModeType mode)
{
  /* Set application mode */
  sdvos_appmode = mode;

  /* 
   * Initialize the MCU we are using. This involves low
   * level initialization which usually at least include
   * a stack switch after disabling interrupts globally for
   * initialization. It is OK because StartOS will never
   * return and we already processed the parameter.
   * Procedures after this call will be carried out in
   * SDVOS kernel context.
   *
   * McuInit can either be a C function, assembly function
   * or a macro on different platforms.
   */
  McuInit ();

  /*
   * Board specific initialization. Might include clock
   * configuration, flash setup, etc.
   */
  BoardInit ();

  /* Initialize Interrupt Handling */
  InterruptInit ();

  /* Initialize all the drivers configured */
  DriversInit ();

  /* Activate auto start tasks */
  TaskAutoStart (sdvos_appmode);

  /* Initialize Alarm/Counter */
  AlarmInit ();

#ifdef USE_SCHEDTBL
  /*
   * Initialize schedule table. This is mostly schedule
   * table auto start. And it has to be done after task
   * and alarm auto start. [SWS_Os_00510]
   */
  ScheduleTableInit ();
#endif

  /*
   * StartupHook is called by the OS at the end of the
   * initialization and before the scheduler is running.
   */
  STARTUPHOOK ();

#ifdef DEBUG_SDVOS_VERBOSE
  IntegrityCheck ();
#endif

#ifdef PRINT_BANNER
  PrintBanner ();
#endif

  DEBUG_PRINTF ("Starting SDVOS in mode %d...\n", sdvos_appmode);

  /* Switch to the first user task */
  JumpNext ();

  /* Should never reach here */
  return;
}

void
Sys_ShutdownOS (StatusType error)
{
  /*
   * ShutdownHook is called by the OS when the OS service
   * ShutdownOS has been called.
   */
  SHUTDOWNHOOK (error);
  return;
}

/* vi: set et ai sw=2 sts=2: */
