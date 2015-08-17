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

#include <list.h>
#include <oil_object.h>
#include <stdio.h>
#include <stdlib.h>

oil_object_list_t * oil_tasks;
oil_os_object_t * oil_os;

oil_object_list_t *
object_list_add (oil_object_list_t ** list, void * object)
{
  oil_object_list_t * last = NULL;
  oil_object_list_t * new_obj =
    (oil_object_list_t *) malloc (sizeof (oil_object_list_t));

  if (!new_obj) {
    fprintf (stderr, "malloc failed in object_list_add!\n");
    return NULL;
  }

  new_obj->data.object = object;
  new_obj->next = NULL;

  if (!(*list)) {
    *list = new_obj;
    return *list;
  }

  last = *list;
  while (last->next) {
    last = last->next;
  }

  last->next = new_obj;

  return *list;
}

/* vi: set et ai sw=2 sts=2: */
