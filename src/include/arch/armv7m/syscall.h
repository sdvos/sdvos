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
 * @file   src/include/arch/armv7m/syscall.h
 * @author Ye Li (liye@sdvos.org)
 * @brief  ARMv7-M System Service Numbers
 */
#ifndef _ARMV7M_SYSCALL_H_
#define _ARMV7M_SYSCALL_H_

#if defined (SCHEDTBL_SYNC)
#define NUM_SYSCALLS          0x26
#elif defined (USE_SCHEDTBL)
#define NUM_SYSCALLS          0x23
#else
#define NUM_SYSCALLS          0x1E
#endif

#define SVC_MAX_NO_PREEMPT    0x06

/* The first six cause preemption at the end of service */
#define SVC_NO_ACTIVATETASK                      0x01
#define SVC_NO_SCHEDULE                          0x02
#define SVC_NO_RELEASERESOURCE                   0x03
#define SVC_NO_SETEVENT                          0x04
#define SVC_NO_WAITEVENT                         0x05
#define SVC_NO_INCREMENTCOUNTER                  0x06
/* The following two trigger context switch (not preemption) */
#define SVC_NO_TERMINATETASK                     0x07
#define SVC_NO_CHAINTASK                         0x08
/* The rest are "normal" system services */
#define SVC_NO_GETTASKID                         0x09
#define SVC_NO_GETTASKSTATE                      0x0A
#define SVC_NO_DISABLEALLINTERRUPTS              0x0B
#define SVC_NO_ENABLEALLINTERRUPTS               0x0C
#define SVC_NO_SUSPENDALLINTERRUPTS              0x0D
#define SVC_NO_RESUMEALLINTERRUPTS               0x0E
#define SVC_NO_SUSPENDOSINTERRUPTS               0x0F
#define SVC_NO_RESUMEOSINTERRUPT                 0x10
#define SVC_NO_GETRESOURCE                       0x11
#define SVC_NO_CLEAREVENT                        0x12
#define SVC_NO_GETEVENT                          0x13
#define SVC_NO_GETALARMBASE                      0x14
#define SVC_NO_GETALARM                          0x15
#define SVC_NO_SETRELALARM                       0x16
#define SVC_NO_SETABSALARM                       0x17
#define SVC_NO_CANCELALARM                       0x18
#define SVC_NO_GETACTIVEAPPLICATIONMODE          0x19
#define SVC_NO_STARTOS                           0x1A
#define SVC_NO_SHUTDOWNOS                        0x1B
#define SVC_NO_GETCOUNTERVALUE                   0x1C
#define SVC_NO_GETELAPSEDVALUE                   0x1D

#define SVC_NO_STARTSCHEDULETABLEREL             0x1E
#define SVC_NO_STARTSCHEDULETABLEABS             0x1F
#define SVC_NO_STOPSCHEDULETABLE                 0x20
#define SVC_NO_NEXTSCHEDULETABLE                 0x21
#define SVC_NO_GETSCHEDULETABLESTATUS            0x22
#define SVC_NO_STARTSCHEDULETABLESYNCHRON        0x23
#define SVC_NO_SYNCSCHEDULETABLE                 0x24
#define SVC_NO_SETSCHEDULETABLEASYNC             0x25

#endif

/* vi: set et ai sw=2 sts=2: */
