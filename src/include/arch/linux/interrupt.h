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
 * @file   src/include/arch/linux/interrupt.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  Linux Interrupt Management
 */
#ifndef _LINUX_INTERRUPT_H_
#define _LINUX_INTERRUPT_H_

#include <arch/linux/task.h>
#include <sdvos.h>
#include <signal.h>
#include <stdio.h>

/**
 * @def CONSTRUCTOR_ATTR
 * @brief Constructor attribute for functions
 *
 * This is only used in Linux for constructor functions.
 */
#define CONSTRUCTOR_ATTR     __attribute__((constructor))

/**
 * @def ISR_CAT1
 * @brief Definition macro for Category 1 ISR
 *
 * Macro used for the definition of Category 1 ISR. System
 * services allowed in Category 1 ISR are:
 *
 * EnableAllInterrupts, DisableAllInterrupts
 * ResumeAllInterrupts, SuspendAllInterrupts
 * ResumeOSInterrupts, SuspendOSInterrupts
 *
 * @param[in] vector
 *   Interrupt vector (POSIX signal number)
 */
#define ISR_CAT1(vector)                       \
  void vector##_init (void) CONSTRUCTOR_ATTR;  \
  void vector##_handler_impl (int signo);      \
  void vector##_handler (int signo);           \
  void vector##_init (void) {                  \
    struct sigaction act;                      \
    act.sa_handler = vector##_handler;         \
    sigemptyset (&act.sa_mask);                \
    act.sa_flags = 0;                          \
    if (sigaction (vector, &act, NULL) < 0) {  \
      panic ();                                \
    }                                          \
  }                                            \
  void vector##_handler (int signo) {          \
    sigset_t set, oset;                        \
    sigfillset (&set);                         \
    sigprocmask (SIG_SETMASK, &set, &oset);    \
    uatomic_inc (&NestedISRs);                 \
    vector##_handler_impl (signo);             \
    uatomic_dec (&NestedISRs);                 \
    sigprocmask (SIG_SETMASK, &oset, NULL);    \
  }                                            \
  void vector##_handler_impl (int signo)

/**
 * @def ISR
 * @brief Definition macro for Category 2 ISR
 *
 * Macro used for the definition of Category 2 ISR. System
 * services allowed in Category 2 ISR are:
 *
 * ActivateTask, GetTaskID, GetTaskState, GetResource
 * ReleaseResource, SetEvent, GetEvent, GetAlarmBase
 * GetAlarm, SetRelAlarm, SetAbsAlarm, CancelAlarm
 * GetActiveApplicationMode, ShutdownOS
 *
 * @param[in] vector
 *   Interrupt vector (POSIX signal number)
 */
#define ISR(vector)        \
  void vector##_init (void) CONSTRUCTOR_ATTR;  \
  void vector##_handler_impl (int signo);      \
  void vector##_handler (int signo);           \
  void vector##_init (void) {                  \
    struct sigaction act;                      \
    act.sa_handler = vector##_handler;         \
    sigemptyset (&act.sa_mask);                \
    act.sa_flags = 0;                          \
    if (sigaction (vector, &act, NULL) < 0) {  \
      panic ();                                \
    }                                          \
  }                                            \
  void vector##_handler (int signo) {          \
    sigset_t set, oset;                        \
    sigfillset (&set);                         \
    sigprocmask (SIG_SETMASK, &set, &oset);    \
    uatomic_inc (&NestedISRs);                 \
    vector##_handler_impl (signo);             \
    uatomic_dec (&NestedISRs);                 \
    CheckPreemption (PREEMPT_ISR);             \
    sigprocmask (SIG_SETMASK, &oset, NULL);    \
  }                                            \
  void vector##_handler_impl (int signo)

/**
 * @def ArchEnableAllInterrupts
 * @brief Internal macro used by EnableAllInterrupts().
 */
#define ArchEnableAllInterrupts() do {       \
  sigset_t sigset;                           \
  sigemptyset (&sigset);                     \
  sigprocmask (SIG_SETMASK, &sigset, NULL);  \
} while (0)

/**
 * @def ArchDisableAllInterrupts
 * @brief Internal macro used by DisableAllInterrupts().
 */
#define ArchDisableAllInterrupts() do {      \
  sigset_t sigset;                           \
  sigfillset (&sigset);                      \
  sigprocmask (SIG_SETMASK, &sigset, NULL);  \
} while (0)

/*
 * Difficult to make Resume/SuspendAllInterrupts efficient
 * in AVR5. Let's just make it Enable/DisableAllInterrupts
 * for now. The important thing is, nested calls should be
 * supported.
 */

/**
 * @def ArchResumeAllInterrupts
 * @brief Internal macro used by ResumeAllInterrupts().
 */
#define ArchResumeAllInterrupts  ArchEnableAllInterrupts

/**
 * @def ArchSuspendAllInterrupts
 * @brief Internal macro used by SuspendAllInterrupts().
 */
#define ArchSuspendAllInterrupts  ArchDisableAllInterrupts

/*
 * Same as above. But if necessary, a stack structure can
 * be used to save PCICR and EIMSK registers. This assumes
 * OS interrupts only uses external interrupts.
 */

/**
 * @def ArchResumeOSInterrupts
 * @brief Internal macro used by ResumeOSInterrupts().
 */
#define ArchResumeOSInterrupts  ArchEnableAllInterrupts

/**
 * @def ArchSuspendOSInterrupts
 * @brief Internal macro used by SuspendOSInterrupts().
 */
#define ArchSuspendOSInterrupts  ArchDisableAllInterrupts

#endif

/* vi: set et ai sw=2 sts=2: */
