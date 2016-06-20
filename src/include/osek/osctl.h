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
 * @file   src/include/osek/osctl.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  OSEK Operating System Execution Control
 */
#ifndef _OSEK_OSCTL_H_
#define _OSEK_OSCTL_H_

#include <osek/types.h>

/** Default application mode of OS */
#define OSDEFAULTAPPMODE    0x0

/**
 * @brief
 * This service returns the current application mode. It
 * may be used to write mode dependent code.
 *
 * Allowed for task, ISR and all hook routines.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @return
 *   Application mode
 */
AppModeType GetActiveApplicationMode (void);

/**
 * @brief
 * User can call this system service to start the operating
 * system in a specific mode.
 *
 * Only allowed outside of the operating system, therefore
 * implementation specific restrictions may apply.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] mode
 *   Application mode
 */
void StartOS (AppModeType mode);

/**
 * @brief
 * User can call this system service to abort the overall
 * system (e.g. emergency off). The OS also calls this
 * function internally, if it has reached an undefined
 * internal state and is no longer ready to run.
 *
 * If a ShutdownHook is configured, it is always called
 * before shutting down the OS. If ShutdownHook returns,
 * further behaviour of ShutdownOS is implemention specific.
 * In case of a system where OSEK OS and OSEKtime coexist,
 * <Error> might also be a value accepted by OSEKtime OS.
 * In this case, if enabled by and OSEKtime configuration
 * parameter, OSEKtime OS will be shut down after OSEK OS
 * shutdown.
 *
 * Allowed at task level, ISR, ErrorHOook and StartupHook.
 * Also called internally by the OS. If the OS calls this
 * function, it never uses E_OK as the passed parameter.
 *
 * Conformance:
 *   BCC1, BCC2, ECC1, ECC2
 *
 * @param[in] error
 *   Error occurred
 */
void ShutdownOS (StatusType error);

#endif

/* vi: set et ai sw=2 sts=2: */
