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

#ifndef _LIST_H_
#define _LIST_H_

#include <stdint.h>

struct oil_task_object;
struct oil_appmode_object;
struct oil_resource_object;
struct oil_event_object;
struct oil_param;
struct oil_sched_tbl_object;
struct oil_expiry_point_object;
struct oil_expiry_point_event_object;
struct oil_driver_object;

typedef struct oil_object_list {
  union _data {
    struct oil_task_object * task;
    struct oil_appmode_object * appmode;
    struct oil_resource_object * resource;
    struct oil_event_object * event;
    struct oil_param * param;
    struct oil_sched_tbl_object * sched_tbl;
    struct oil_expiry_point_object * ep;
    struct oil_expiry_point_event_object * task_event;
    struct oil_driver_object * driver;
    void * object;
  } data;
  struct oil_object_list * next;
} oil_object_list_t;

extern oil_object_list_t *
  object_list_add (oil_object_list_t ** list, void * object);

#define for_each(i,l,ind)                                  \
  for ((ind = (l))?((i)=(ind)->data.object):((i)=NULL);    \
       (i) != NULL;                                        \
       ((ind) = (ind)->next) ? ((i) = (ind)->data.object): \
       ((i) = NULL))

#endif

/* vi: set et ai sw=2 sts=2: */
