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

#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <parser.h>
#include <stdint.h>
#include <inttypes.h>

void
dump_os (oil_os_object_t * os)
{
  oil_driver_object_t * driver;
  oil_object_list_t * index;

  printf ("OS: %s\n", os->name);
  printf ("  STATUS: %s\n", (os->status == OS_STATUS_STANDARD) ?
          "STANDARD" : "EXTENDED");
  printf ("  STARTUPHOOK: %s\n", (os->startup_hook) ?
          "TRUE" : "FALSE");
  printf ("  ERRORHOOK: %s\n", (os->error_hook) ?
          "TRUE" : "FALSE");
  printf ("  SHUTDOWNHOOK: %s\n", (os->shutdown_hook) ?
          "TRUE" : "FALSE");
  printf ("  PRETASKHOOK: %s\n", (os->pretask_hook) ?
          "TRUE" : "FALSE");
  printf ("  POSTTASKHOOK: %s\n", (os->posttask_hook) ?
          "TRUE" : "FALSE");
  printf ("  USEGETSERVICEID: %s\n", (os->use_getserviceid) ?
          "TRUE" : "FALSE");
  printf ("  USEPARAMETERACCESS: %s\n", (os->use_parameteraccess) ?
          "TRUE" : "FALSE");
  printf ("  USERESSCHEDULER: %s\n", (os->use_resscheduler) ?
          "TRUE" : "FALSE");
  printf ("  DEBUGLEVEL: %d\n", os->debuglevel);
  printf ("  BOARD: %s\n", os->board);

  for_each (driver, oil_drivers, index) {
    printf ("  Driver: %s (%s)\n", driver->name, driver->file_name);
  }
}

void
dump_task (oil_task_object_t * task)
{
  char * sched = NULL;
  oil_object_list_t * index = NULL;
  oil_appmode_object_t * mode = NULL;
  oil_resource_object_t * res = NULL;
  oil_event_object_t * event = NULL;

  switch (task->schedule) {
    case TASK_SCHEDULE_NON :
      sched = "NON";
      break;
    case TASK_SCHEDULE_FULL :
      sched = "FULL";
      break;
    default :
      sched = "ERROR!";
  }

  printf ("TASK: %s\n", task->name);
  printf ("  ID = %d\n", task->id);
  printf ("  Priority = %d\n", task->priority);
  printf ("  Activation = %d\n", task->activation);
  printf ("  Schedule = %s\n", sched);
  printf ("  Stacksize = %d\n", task->stacksize);

  if (task->autostart) {
    printf ("  Auto start in application mode: ");
    for_each (mode, task->appmode, index) {
      printf ("%s ", mode->name);
    }
    printf ("\n");
  }

  printf ("  Resources: ");
  for_each (res, task->resource, index) {
    printf ("%s ", res->name);
  }
  printf ("\n");

  printf ("  Events: ");
  for_each (event, task->event, index) {
    printf ("%s(0x%" PRIu64 ") ", event->name, event->mask);
  }
  printf ("\n");
}

void
dump_event (oil_event_object_t * event)
{
  printf ("Event: %s\n", event->name);
  printf ("  ID = %d\n", event->id);
  printf ("  MASK = 0x%" PRIu64 "\n", event->mask);
}

void
dump_appmode (oil_appmode_object_t * appmode)
{
  printf ("Appmode: %s\n", appmode->name);
  printf ("  ID = %d\n", appmode->id);
  printf ("  DEFAULT = %s\n", appmode->default_appmode ?
          "TRUE" : "FALSE");
}

void
dump_param_list (oil_param_t * param)
{
  printf ("%d = %d\n", param->attribute, param->value->value_type);
}

void
dump_counter (oil_counter_object_t * counter)
{
  printf ("Counter: %s\n", counter->name);
  printf ("  ID = %d\n", counter->id);
  printf ("  MINCYCLE = %d\n", counter->min_cycle);
  printf ("  MAXALLOWEDVALUE = %d\n", counter->max_allowed_value);
  printf ("  TICKSPERBASE = %d\n", counter->ticks_per_base);
}

void
dump_resource (oil_resource_object_t * resource)
{
  char * prop = NULL;

  switch (resource->property.type) {
    case RESOURCE_TYPE_STANDARD :
      prop = "STANDARD";
      break;
    case RESOURCE_TYPE_INTERNAL :
      prop = "INTERNAL";
      break;
    case RESOURCE_TYPE_LINKED :
      prop = "LINKED";
      break;
    default :
      prop = "UNKNOWN";
  }

  printf ("Resource: %s\n", resource->name);
  printf ("  ID = %d\n", resource->id);
  printf ("  PROPERTY = %s\n", prop);
  printf ("  PRIORITY = %d\n", resource->priority);

  if (resource->property.linked_resource) {
    printf ("  LINKED RESOURCE = %s\n",
            resource->property.linked_resource->name);
  }
}

void
dump_alarm (oil_alarm_object_t * alarm)
{
  char * action_string = NULL;
  oil_appmode_object_t * mode = NULL;
  oil_object_list_t * index = NULL;

  printf ("Alarm: %s\n", alarm->name);
  printf ("  ID = %d\n", alarm->id);
  printf ("  COUNTER = %s\n", alarm->counter->name);

  switch (alarm->action.type) {
    case ACTION_TYPE_SETEVENT :
      action_string = "SETEVENT";
      break;
    case ACTION_TYPE_ACTIVATETASK :
      action_string = "ACTIVATETASK";
      break;
    case ACTION_TYPE_ALARMCALLBACK :
      action_string = "ALARMCALLBACK";
      break;
    default :
      action_string = "UNKNOWN";
  }

  printf ("  ACTION = %s\n", action_string);
  if (alarm->action.event) {
    printf ("    EVENT %s (0x%" PRIu64 ")\n", alarm->action.event->name,
            alarm->action.event->mask);
  }

  if (alarm->action.task) {
    printf ("    TASK %s\n", alarm->action.task->name);
  }

  if (alarm->action.alarm_callback_name) {
    printf ("    ALARMCALLBACKNAME = %s\n",
            alarm->action.alarm_callback_name);
  }

  if (alarm->autostart) {
    printf ("  Auto start in application mode: ");
    for_each (mode, alarm->appmode, index) {
      printf ("%s ", mode->name);
    }
    printf ("\n");
    printf ("  ALARM_TIME = %d\n", alarm->alarm_time);
    printf ("  CYCLE_TIME = %d\n", alarm->cycle_time);
  }
}

void
dump_isr (oil_isr_object_t * isr)
{
  oil_resource_object_t * res = NULL;
  oil_object_list_t * index = NULL;

  printf ("ISR: %s\n", isr->name);
  printf ("  CATEGORY = %d\n", isr->category);
  printf ("  VECTOR = %d\n", isr->vector);
  printf ("  Resources: ");
  for_each (res, isr->resource, index) {
    printf ("%s ", res->name);
  }
  printf ("\n");
}

void
dump_exp (oil_expiry_point_object_t * exp)
{
  oil_task_object_t * task = NULL;
  oil_expiry_point_event_object_t * task_event = NULL;
  oil_object_list_t * index = NULL;

  printf ("EXPIRYPOINT: %s\n", exp->name);
  printf ("  OFFSET = %d\n", exp->offset);
  printf ("  MAXLENGTHEN = %d\n", exp->max_lengthen);
  printf ("  MAXSHORTEN = %d\n", exp->max_shorten);
  printf ("  NUMTASKS = %d\n", exp->num_tasks);
  printf ("  NUMTASKEVENTS = %d\n", exp->num_task_events);

  printf ("  TASKS: ");
  for_each (task, exp->tasks, index) {
    printf ("%s ", task->name);
  }

  printf ("\n  Events: ");
  for_each (task_event, exp->task_events, index) {
    printf ("(%s, 0x%" PRIu64 ") ", task_event->task->name,
            task_event->event->mask);
  }
  printf ("\n");
}

void
dump_sched_tbl (oil_sched_tbl_object_t * sched_tbl)
{
  oil_appmode_object_t * mode = NULL;
  oil_expiry_point_object_t * exp = NULL;
  oil_object_list_t * index = NULL;

  printf ("SCHEDULE TABLE: %s\n", sched_tbl->name);
  printf ("  ID = %d\n", sched_tbl->id);
  printf ("  COUNTER = %s\n", sched_tbl->counter->name);
  printf ("  DURATION = %d\n", sched_tbl->duration);
  printf ("  DELAY = %d\n", sched_tbl->delay);
  printf ("  REPEATING = %s\n", (sched_tbl->repeating) ?
          "TRUE" : "FALSE");
  printf ("  SYNCSTRATEGY = ");
  switch (sched_tbl->sync_strategy) {
    case SCHEDTBL_SYNC_NONE :
      printf ("NONE\n");
      break;
    case SCHEDTBL_SYNC_IMPLICIT :
      printf ("IMPLICIT\n");
      break;
    case SCHEDTBL_SYNC_EXPLICIT :
      printf ("EXPLICIT\n");
      break;
    default :
      printf ("ERROR\n");
      break;
  }
  printf ("  PRECISION = %d\n", sched_tbl->precision);

  if (sched_tbl->autostart) {
    printf ("  Auto start in application mode: ");
    for_each (mode, sched_tbl->appmode, index) {
      printf ("%s ", mode->name);
    }
    printf ("\n");
    printf ("  STARTVALUE = %d\n", sched_tbl->start_value);
    printf ("  STARTMODE = ");
    switch (sched_tbl->start_mode) {
      case SCHEDTBL_STARTMODE_ABSOLUTE :
        printf ("ABSOLUTE\n");
        break;
      case SCHEDTBL_STARTMODE_RELATIVE :
        printf ("RELATIVE\n");
        break;
      case SCHEDTBL_STARTMODE_SYNCHRON :
        printf ("SYNCHRON\n");
        break;
      case SCHEDTBL_STARTMODE_NONE :
        printf ("NONE\n");
        break;
      default :
        printf ("ERROR\n");
        break;
    }
  }

  printf ("  NUMEXPS = %d\n", sched_tbl->num_exps);
  for_each (exp, sched_tbl->exps, index) {
    dump_exp (exp);
  }
}

void
dump_all_objects ()
{
  oil_task_object_t * task = NULL;
  oil_event_object_t * event = NULL;
  oil_appmode_object_t * appmode = NULL;
  oil_counter_object_t * counter = NULL;
  oil_resource_object_t * resource = NULL;
  oil_alarm_object_t * alarm = NULL;
  oil_isr_object_t * isr = NULL;
  oil_sched_tbl_object_t * sched_tbl = NULL;
  oil_object_list_t * index = NULL;

  dump_os (oil_os);

  for_each (task, oil_tasks, index) {
    dump_task (task);
  }

  for_each (event, oil_events, index) {
    dump_event (event);
  }

  for_each (appmode, oil_appmodes, index) {
    dump_appmode (appmode);
  }

  for_each (counter, oil_counters, index) {
    dump_counter (counter);
  }

  for_each (resource, oil_resources, index) {
    dump_resource (resource);
  }

  for_each (alarm, oil_alarms, index) {
    dump_alarm (alarm);
  }

  for_each (isr, oil_isrs, index) {
    dump_isr (isr);
  }

  for_each (sched_tbl, oil_sched_tbls, index) {
    dump_sched_tbl (sched_tbl);
  }

  printf ("\n");
}

void
show_stats ()
{
  printf ("-------------------------------------------\n");
  printf ("SDVOS Configuration and Resource Statistics\n");
  printf ("-------------------------------------------\n");
  printf ("OIL Version: %s\n", oil_version);
  printf ("CPU Name: %s\n", cpu_name);
  printf ("OS Name: %s\n", oil_os->name);
  printf ("Board: %s\n", oil_os->board);
  printf ("-------------------------------------------\n");
  printf ("Object Type       | Count                  \n");
  printf ("-------------------------------------------\n");
  printf ("Tasks             | %d\n", num_tasks);
  printf ("Events            | %d\n", num_events);
  printf ("Resources         | %d\n", num_resources);
  printf ("App Modes         | %d\n", num_appmodes);
  printf ("Counters          | %d\n", num_counters);
  printf ("Alarms            | %d\n", num_alarms);
  printf ("ISR 1             | %d\n", num_isr1s);
  printf ("ISR 2             | %d\n", num_isr2s);
  if (with_sched_tbl)
  printf ("Schedule Tables   | %d\n", num_sched_tbls);
  printf ("Drivers           | %d\n", num_drivers);
  printf ("-------------------------------------------\n");
}

/* vi: set et ai sw=2 sts=2: */
