/*
 *                   SDVOS System Generator
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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <oil_object.h>

extern void dump_task (oil_task_object_t *);
extern void dump_os (oil_os_object_t * os);
extern void dump_event (oil_event_object_t * event);
extern void dump_appmode (oil_appmode_object_t * appmode);
extern void dump_param_list (oil_param_t * param);
extern void dump_counter (oil_counter_object_t * counter);
extern void dump_resource (oil_resource_object_t * resource);
extern void dump_alarm (oil_alarm_object_t * alarm);
extern void dump_isr (oil_isr_object_t * isr);
extern void dump_exp (oil_expiry_point_object_t * exp);
extern void dump_sched_tbl (oil_sched_tbl_object_t * sched_tbl);
extern void dump_all_objects (void);
extern void show_stats (void);

#endif

/* vi: set et ai sw=2 sts=2: */
