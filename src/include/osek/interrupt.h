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
 * @file   src/include/osek/interrupt.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Interrupt Management
 */
#ifndef _OSEK_INTERRUPT_H_
#define _OSEK_INTERRUPT_H_

#if defined __ARCH_AVR5__ || defined __ARCH_AVR6__
#include <arch/avr/interrupt.h>
#elif defined __ARCH_I386__
#include <arch/i386/interrupt.h>
#elif defined __ARCH_ARMV7M__
#include <arch/armv7m/interrupt.h>
#elif defined __ARCH_LINUX__
#include <arch/linux/interrupt.h>
#endif

/**
 * @brief
 * This service restores the state saved by
 * DisableAllInterrupts.
 *
 * The service may be called from an ISR category 1
 * and category 2 and from the task level, but not from
 * hook routines. This service is a counterpart of
 * DisableAllInterrupts service, which has to be called
 * before, and its aim is the completion of the critical
 * section of code. No API service calls are allowed within
 * this critical section. The implementation should adapt
 * this service to the target hardware providing a minimum
 * overhead. Usually, this service enables recognition of
 * interrupts by the central processing unit.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void EnableAllInterrupts (void);

/**
 * @brief
 * This service disables all interrupts for which the
 * hardware supports disabling. The state before is saved
 * for the EnableAllInterrupts call.
 *
 * The service may be called from an ISR category 1 and
 * category 2 and from the task level, but not from hook
 * routines. This service is intended to start a critical
 * section of the code. This section shall be finished by
 * calling the EnableAllInterrupts service. No API service
 * calls are allowed within this critical section.
 *
 * The implementation should adapt this service to the
 * target hardware providing a minimum overhead. Usually,
 * this service disables recognition of interrupts by the
 * central processing unit. Note that this service does not
 * support nesting. If nesting is needed for critical
 * sections e.g. for libraries SuspendOSInterrupts/
 * ResumeOSInterrupts or SuspendAllInterrupt/
 * ResumeAllInterrupts should be used.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void DisableAllInterrupts (void);

/**
 * @brief
 * This service restores the recognition status of all
 * interrupts saved by the SuspendAllInterrupts service.
 *
 * The service may be called from an ISR category 1 and
 * category 2, from alarm-callbacks and from the task level,
 * but not from all hook routines.
 *
 * This service is the counterpart of SuspendAllInterrupts
 * service, which has to have been called before, and its
 * aim is the completion of the critical section of code.
 * No API service calls beside SuspendAllInterrupts/
 * ResumeAllInterrupts pairs and SuspendOSInterrupts/
 * ResumeOSInterrupts pairs are allowed within this
 * critical section.
 *
 * The implementation should adapt this service to the
 * target hardware providing a minimum overhead.
 * SuspendAllInterrupts/ResumeAllInterrupts can be nested.
 * In case of nesting pairs of the calls SuspendAllInterrupts
 * and ResumeAllInterrupts the interrupt recognition status
 * saved by the first call of SuspendAllInterrupts is
 * restored by the last call of the ResumeAllInterrupts
 * service.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void ResumeAllInterrupts (void);

/**
 * @brief
 * This service saves the recognition status of all
 * interrupts and disables all interrupts for which the
 * hardware supports disabling.
 *
 * The service may be called from an ISR category 1 and
 * category 2, from alarm-callbacks and from the task level,
 * but not from all hook routines.
 *
 * This service is intended to protect a critical section
 * of code from interruptions of any kind. This section
 * shall be finished by calling the ResumeAllInterrupts
 * service. No API service calls beside SuspendAllInterrupts/
 * ResumeAllInterrupts pairs and SuspendOSInterrupts/
 * ResumeOSInterrupts pairs are allowed within this critical
 * section. The implementation should adapt this service to
 * the target hardware providing a minimum overhead.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void SuspendAllInterrupts (void);

/**
 * @brief
 * This service restores the recognition status of interrupts
 * saved by the SuspendOSInterrupts service.
 *
 * The service may be called from an ISR category 1 and
 * category 2 and from the task level, but not from hook
 * routines. This service is the counterpart of
 * SuspendOSInterrupts service, which has to have been
 * called before, and its aim is the completion of the
 * critical section of code. No API service calls beside
 * SuspendAllInterrupts/ResumeAllInterrupts pairs and
 * SuspendOSInterrupts/ResumeOSInterrupts pairs are allowed
 * within this critical section.
 *
 * The implementation should adapt this service to the target
 * hardware providing a minimum overhead.
 * SuspendOSInterrupts/ResumeOSInterrupts can be nested. In
 * case of nesting pairs of the calls SuspendOSInterrupts
 * and ResumeOSInterrupts the interrupt recognition status
 * saved by the first call of SuspendOSInterrupts is
 * restored by the last call of the ResumeOSInterrupts
 * service.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void ResumeOSInterrupts (void);

/**
 * @brief
 * This service saves the recognition status of interrupts
 * of category 2 and disables the recognition of these
 * interrupts.
 *
 * The service may be called from an ISR and from the task
 * level, but not from hook routines.
 *
 * This service is intended to protect a critical section
 * of code. This section shall be finished by calling the
 * ResumeOSInterrupts service. No API service calls beside
 * SuspendAllInterrupts/ResumeAllInterrupts pairs and
 * SuspendOSInterrupts/ResumeOSInterrupts pairs are allowed
 * within this critical section.
 *
 * The implementation should adapt this service to the
 * target hardware providing a minimum overhead. It is
 * intended only to disable interrupts of category 2.
 * However, if this is not possible in an efficient way
 * more interrupts may be disabled.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 */
void SuspendOSInterrupts (void);

#endif

/* vi: set et ai sw=2 sts=2: */
