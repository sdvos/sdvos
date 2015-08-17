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
 * @file   src/debug.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Debug Utilities.
 */
#include <osek/osek.h>
#include <config/config.h>
#include <sdvos.h>

#if defined(USEGETSERVICEID) && (USEGETSERVICEID == 0x1)
struct OSErrorService_t OSErrorService;
#endif

#if defined(HAS_ERRORHOOK) && (HAS_ERRORHOOK == 0x1)
FlagType InErrorHook = FALSE;
#endif

#ifdef DEBUG_SDVOS

void
dump_prio_queue ()
{
  int i = 0;
  TCB * index = NULL;
  prio_queue_t * pq = NULL;

#ifdef MULTI_TASK_PER_PRIO
  for (i = MAX_PRIO; i >= 0; i--) {
    DEBUG_PRINTF ("Priority[%d]:\n", i);
    pq = &prio_queue[i];
    index = pq->head;
    while (index) {
      DEBUG_PRINTF ("  tid:%d prio:%d orig_p:%d state:%d ",
                    index->tid, index->priority, index->orig_prio,
                    index->state);
#ifdef MULTI_ACTIVATION
      DEBUG_PRINTF ("act:%d max_act:%d ",
                    index->act, index->max_act);
#endif
      DEBUG_PRINTF ("start:0x%X stack:0x%X\n",
                    index->start, index->sp);
      index = index->next;
    }
  }
#else
  for (i = MAX_PRIO; i >= 0; i--) {
    DEBUG_PRINTF ("Priority[%d]:\n", i);
    pq = &prio_queue[i];
    index = *pq;
    if (index) {
      DEBUG_PRINTF ("  tid:%d prio:%d orig_p:%d state:%d ",
                    index->tid, index->priority, index->orig_prio,
                    index->state);
      DEBUG_PRINTF ("start:0x%X stack:0x%X\n",
                    index->start, index->sp);
    }
  }
#endif
}

#ifdef PRINT_BANNER
void
PrintBanner ()
{
  DEBUG_PRINTF ("     _______ _   ______  ____\n");
  DEBUG_PRINTF ("    / __/ _ \\ | / / __ \\/ __/\n");
  DEBUG_PRINTF ("   _\\ \\/ // / |/ / /_/ /\\ \\  \n");
  DEBUG_PRINTF ("  /___/____/|___/\\____/___/  \n");
  DEBUG_PRINTF ("\n");
  DEBUG_PRINTF ("       Welcome to SDVOS!\n");
  DEBUG_PRINTF ("         www.sdvos.com\n");
  DEBUG_PRINTF ("\n");
}
#endif

#endif

/* vi: set et ai sw=2 sts=2: */
