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

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <list.h>
#include <oil_object.h>
#include <debug.h>
#include <string.h>
#include <parser.h>
#include <getopt.h>
#include <dirent.h>
#include <sys/stat.h>
#include "parser_bison.h"

uint32_t num_tasks = 0;
uint32_t num_events = 0;
uint32_t num_resources = 0;
uint32_t num_appmodes = 0;
uint32_t num_counters = 0;
uint32_t num_alarms = 0;
uint32_t num_sched_tbls = 0;
uint32_t num_autostart_sched_tbls = 0;
uint32_t num_expiry_points = 0;
uint32_t num_isrs = 0;
uint32_t num_isr1s = 0;
uint32_t num_isr2s = 0;
uint32_t num_drivers = 0;
uint32_t max_prio = 0;
uint32_t max_activation = 0;
uint64_t max_mask = 0;
uint64_t max_tick = 0;
uint32_t max_irq = 0;
uint64_t masks = 0;
bool rflag = FALSE, tflag = FALSE, dflag = FALSE;
bool bflag = FALSE, mflag = FALSE;
bool mult_task_per_prio = FALSE, mult_activation = FALSE;
bool with_sched_tbl_sync = FALSE, with_sched_tbl = FALSE;
bool mult_schedtbl_per_cntr = FALSE;
char * include_path = NULL;
char * include_path_list[MAX_INCLUDE_PATH];
char * sdvos_root = NULL;
oil_object_list_t * oil_tasks = NULL;
oil_os_object_t * oil_os = NULL;
oil_object_list_t * oil_appmodes = NULL;
oil_object_list_t * oil_events = NULL;
oil_object_list_t * oil_resources = NULL;
oil_object_list_t * oil_counters = NULL;
oil_object_list_t * oil_alarms = NULL;
oil_object_list_t * oil_isrs = NULL;
oil_object_list_t * oil_sched_tbls = NULL;
oil_object_list_t * oil_expiry_points = NULL;
oil_object_list_t * oil_drivers = NULL;
generic_oil_object_t cur_oil_object = {0, NULL};
char * oil_version = NULL;
char * cpu_name = NULL;
const char * version_string = "0.1a";
extern void yyerror (const char *s, ...);
extern void sderror (const char *s, int line, ...);

#define CHK_RANGE2(v,min,max)   (((v > max) || (v < min)) ? \
                                (FALSE) : (TRUE))
#define CHK_RANGE(v,l)   CHK_RANGE2 (v, 0, l)

oil_task_object_t *
get_task_object (const char * name)
{
  oil_task_object_t * task = NULL;
  oil_object_list_t * index = NULL;

  for_each (task, oil_tasks, index) {
    if (strncmp (name, task->name, strlen (name)) == 0) {
      return task;
    }
  }

  /* Create new task object */
  task = new_oil_object (oil_task_object_t);
  memset (task, 0, sizeof (oil_task_object_t));
  task->name = (char *) name;
  task->priority = -1;
  object_list_add (&oil_tasks, task);
  num_tasks++;

  return task;
}

oil_event_object_t *
get_event_object (const char * name)
{
  oil_event_object_t * event = NULL;
  oil_object_list_t * index = NULL;

  for_each (event, oil_events, index) {
    if (strncmp (name, event->name, strlen (name)) == 0) {
      return event;
    }
  }

  /* Create new event object */
  event = new_oil_object (oil_event_object_t);
  memset (event, 0, sizeof (oil_event_object_t));
  event->name = (char *) name;
  event->mask = -1;
  object_list_add (&oil_events, event);
  num_events++;

  return event;
}

oil_resource_object_t *
get_resource_object (const char * name)
{
  oil_resource_object_t * resource = NULL;
  oil_object_list_t * index = NULL;

  for_each (resource, oil_resources, index) {
    if (strncmp (name, resource->name, strlen (name)) == 0) {
      return resource;
    }
  }

  /* Create new resource object */
  resource = new_oil_object (oil_resource_object_t);
  memset (resource, 0, sizeof (oil_resource_object_t));
  resource->name = (char *) name;
  resource->property.type = -1;
  object_list_add (&oil_resources, resource);
  num_resources++;

  return resource;
}

oil_appmode_object_t *
get_appmode_object (const char * name)
{
  oil_appmode_object_t * appmode = NULL;
  oil_object_list_t * index = NULL;

  for_each (appmode, oil_appmodes, index) {
    if (strncmp (name, appmode->name, strlen (name)) == 0) {
      return appmode;
    }
  }

  /* Create new appmode object */
  appmode = new_oil_object (oil_appmode_object_t);
  memset (appmode, 0, sizeof (oil_appmode_object_t));
  appmode->name = (char *) name;
  appmode->default_appmode = FALSE;
  object_list_add (&oil_appmodes, appmode);
  num_appmodes++;

  return appmode;
}

oil_counter_object_t *
get_counter_object (const char * name)
{
  oil_counter_object_t * counter = NULL;
  oil_object_list_t * index = NULL;

  for_each (counter, oil_counters, index) {
    if (strncmp (name, counter->name, strlen (name)) == 0) {
      return counter;
    }
  }

  /* Create new counter object */
  counter = new_oil_object (oil_counter_object_t);
  memset (counter, 0, sizeof (oil_counter_object_t));
  counter->name = (char *) name;
  object_list_add (&oil_counters, counter);
  num_counters++;

  return counter;
}

oil_alarm_object_t *
get_alarm_object (const char * name)
{
  oil_alarm_object_t * alarm = NULL;
  oil_object_list_t * index = NULL;

  for_each (alarm, oil_alarms, index) {
    if (strncmp (name, alarm->name, strlen (name)) == 0) {
      return alarm;
    }
  }

  /* Create new alarm object */
  alarm = new_oil_object (oil_alarm_object_t);
  memset (alarm, 0, sizeof (oil_alarm_object_t));
  alarm->name = (char *) name;
  alarm->alarm_time = -1;
  alarm->cycle_time = -1;
  alarm->action.type = -1;
  alarm->autostart = FALSE;
  object_list_add (&oil_alarms, alarm);
  num_alarms++;

  return alarm;
}

oil_isr_object_t *
get_isr_object (const char * name)
{
  oil_isr_object_t * isr = NULL;
  oil_object_list_t * index = NULL;

  for_each (isr, oil_isrs, index) {
    if (strncmp (name, isr->name, strlen (name)) == 0) {
      return isr;
    }
  }

  /* Create new ISR object */
  isr = new_oil_object (oil_isr_object_t);
  memset (isr, 0, sizeof (oil_isr_object_t));
  isr->name = (char *) name;
  isr->vector = -1;
  object_list_add (&oil_isrs, isr);
  num_isrs++;

  return isr;
}

oil_sched_tbl_object_t *
get_sched_tbl_object (const char * name)
{
  oil_sched_tbl_object_t * sched_tbl = NULL;
  oil_object_list_t * index = NULL;

  for_each (sched_tbl, oil_sched_tbls, index) {
    if (strncmp (name, sched_tbl->name, strlen (name)) == 0) {
      return sched_tbl;
    }
  }

  /* Create new schedule table object */
  sched_tbl = new_oil_object (oil_sched_tbl_object_t);
  memset (sched_tbl, 0, sizeof (oil_sched_tbl_object_t));
  sched_tbl->name = (char *) name;
  sched_tbl->repeating = FALSE;
  sched_tbl->duration = -1;
  sched_tbl->precision = -1;
  sched_tbl->sync_strategy = SCHEDTBL_SYNC_NONE;
  sched_tbl->start_mode = SCHEDTBL_STARTMODE_NONE;
  sched_tbl->start_value = -1;
  sched_tbl->autostart = FALSE;
  object_list_add (&oil_sched_tbls, sched_tbl);
  num_sched_tbls++;

  return sched_tbl;
}

oil_expiry_point_object_t *
get_expiry_point_object (const char * name)
{
  oil_expiry_point_object_t * exp = NULL;
  oil_object_list_t * index = NULL;

  for_each (exp, oil_expiry_points, index) {
    if (strncmp (name, exp->name, strlen (name)) == 0) {
      return exp;
    }
  }

  /* Create new expiry point object */
  exp = new_oil_object (oil_expiry_point_object_t);
  memset (exp, 0, sizeof (oil_expiry_point_object_t));
  exp->name = (char *) name;
  exp->offset = -1;
  exp->max_lengthen = -1;
  exp->max_shorten = -1;
  object_list_add (&oil_expiry_points, exp);
  num_expiry_points++;

  return exp;
}

oil_driver_object_t *
get_driver_object (const char * name)
{
  oil_driver_object_t * driver = NULL;
  oil_object_list_t * index = NULL;

  for_each (driver, oil_drivers, index) {
    if (strncmp (name, driver->name, strlen (name)) == 0) {
      return driver;
    }
  }

  /* Create new driver object */
  driver = new_oil_object (oil_driver_object_t);
  memset (driver, 0, sizeof (oil_driver_object_t));
  driver->name = (char *) name;
  object_list_add (&oil_drivers, driver);
  num_drivers++;

  return driver;
}

int
object_add_param (generic_oil_object_t * object,
                  int attribute, oil_attribute_value_t * value)
{
  switch (object->object_type) {
    case OS :
    {
      oil_os_object_t * os = object->object;

      switch (attribute) {
        case USR_ATTR_NAME :
          if (value->value_type != VALUE_TYPE_STRING) goto os_err;
          os->name = value->v.s;
          break;
        case ATTR_STATUS :
          if (value->value_type != VALUE_TYPE_OS_STATUS) goto os_err;
          (value->v.s4b == STANDARD) ?
            (os->status = OS_STATUS_STANDARD) :
            (os->status = OS_STATUS_EXTENDED);
          break;
        case ATTR_STARTUPHOOK :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->startup_hook = value->v.b;
          break;
        case ATTR_ERRORHOOK :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->error_hook = value->v.b;
          break;
        case ATTR_SHUTDOWNHOOK :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->shutdown_hook = value->v.b;
          break;
        case ATTR_PRETASKHOOK :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->pretask_hook = value->v.b;
          break;
        case ATTR_POSTTASKHOOK :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->posttask_hook = value->v.b;
          break;
        case ATTR_USEGETSERVICEID :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->use_getserviceid = value->v.b;
          break;
        case ATTR_USEPARAMETERACCESS :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->use_parameteraccess = value->v.b;
          break;
        case ATTR_USERESSCHEDULER :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->use_resscheduler = value->v.b;
          break;
        case ATTR_DEBUGLEVEL :
          if (value->value_type != VALUE_TYPE_INT) goto os_err;
          os->debuglevel = value->v.s4b;
          break;
        case ATTR_BOARD :
          if (value->value_type != VALUE_TYPE_STRING) goto os_err;
          os->board = value->v.s;
          break;
        case ATTR_DRIVER :
          if (value->value_type != VALUE_TYPE_STRING_CONST) goto os_err;
          get_driver_object (value->v.s);
          break;
        case ATTR_SHELL :
          if (value->value_type != VALUE_TYPE_BOOL) goto os_err;
          os->shell = value->v.b;
          break;
        default :
          sderror ("Unknown attribute in OS object!", value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
os_err:
      sderror ("Invalid attribute value in OS object!", value->lineno);
      return ERR_ATTRIBUTE;
    }

    case TASK :
    {
      oil_task_object_t * task = object->object;

      switch (attribute) {
        case USR_ATTR_NAME :
          if (value->value_type != VALUE_TYPE_STRING) goto task_err;
          task->name = value->v.s;
          break;
        case ATTR_PRIORITY :
          if (value->value_type != VALUE_TYPE_INT) goto task_err;
          if (!CHK_RANGE (value->v.s8b, MAX_PRIORITY)) {
            sderror ("Task priority out of range!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          task->priority = value->v.s4b;
          break;
        case ATTR_ACTIVATION :
          if (value->value_type != VALUE_TYPE_INT) goto task_err;
          if (!CHK_RANGE (value->v.s8b, MAX_ACTIVATION)) {
            sderror ("Task activation out of range!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          task->activation = value->v.s4b;
          break;
        case ATTR_STACKSIZE :
          if (value->value_type != VALUE_TYPE_INT) goto task_err;
          if (!CHK_RANGE (value->v.s8b, MAX_STACKSIZE)) {
            sderror ("Task stack size out of range!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          task->stacksize = value->v.s4b;
          break;
        case ATTR_SCHEDULE :
          if (value->value_type != VALUE_TYPE_OS_SCHEDULE) goto task_err;
          (value->v.s4b == NON) ?
            (task->schedule = TASK_SCHEDULE_NON) :
            (task->schedule = TASK_SCHEDULE_FULL);
          break;
        case EVENT :
          if (value->value_type != VALUE_TYPE_STRING) goto task_err;
          object_list_add (&(task->event),
                           get_event_object (value->v.s));
          break;
        case RESOURCE :
          if (value->value_type != VALUE_TYPE_STRING) goto task_err;
          object_list_add (&(task->resource),
                           get_resource_object (value->v.s));
          break;
        case ATTR_AUTOSTART :
        {
          oil_param_t * param = NULL;
          oil_object_list_t * index = NULL;

          if ((value->value_type != VALUE_TYPE_BOOL) &&
              (value->value_type != VALUE_TYPE_BOOL_LIST)) goto task_err;
          task->autostart = value->v.b;

          if (task->autostart) {
            for_each (param, value->list, index) {
              if (param->attribute == APPMODE) {
                if (param->value->value_type != VALUE_TYPE_STRING) {
                  sderror ("Invalid attribute value in AUTOSTART",
                           value->lineno);
                  return ERR_ATTRIBUTE;
                }
                object_list_add (&(task->appmode),
                                 get_appmode_object (param->value->v.s));
              } else {
                printf ("Invalid attribute in AUTOSTART ignored!\n");
              }
            }
          }

          break;
        }
        default :
          sderror ("Unknown attribute in Task object!", value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
task_err:
      sderror ("Invalid attribute value in Task object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case EVENT :
    {
      oil_event_object_t * event = object->object;

      switch (attribute) {
        case ATTR_MASK :
          if ((value->value_type != VALUE_TYPE_INT) &&
              (value->value_type != VALUE_TYPE_AUTO)) goto event_err;
          if (value->value_type == VALUE_TYPE_AUTO) {
            event->mask = -1;
            break;
          }
          if (!CHK_RANGE (value->v.s8b, MAX_MASK)) {
            sderror ("Invalid event mask!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          event->mask = value->v.s8b;
          break;
        default :
          sderror ("Unknown attribute in Event object!", value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
event_err:
      sderror ("Invalid attribute value in Event object!", value->lineno);
      return ERR_ATTRIBUTE;
    }

    case COUNTER :
    {
      oil_counter_object_t * counter = object->object;

      switch (attribute) {
        case ATTR_MINCYCLE :
          if (value->value_type != VALUE_TYPE_INT) goto counter_err;
          if (!CHK_RANGE (value->v.s8b, MAX_MINCYCLE)) {
            sderror ("Counter MINCYCLE out of range!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          counter->min_cycle = value->v.s4b;
          break;
        case ATTR_MAXALLOWEDVALUE :
          if (value->value_type != VALUE_TYPE_INT) goto counter_err;
          if (!CHK_RANGE (value->v.s8b, MAX_MAXALLOWEDVALUE)) {
            sderror ("Counter MAXALLOWEDVALUE out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          counter->max_allowed_value = value->v.s4b;
          break;
        case ATTR_TICKSPERBASE :
          if (value->value_type != VALUE_TYPE_INT) goto counter_err;
          if (!CHK_RANGE (value->v.s8b, MAX_TICKSPERBASE)) {
            sderror ("Counter TICKSPERBASE out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          counter->ticks_per_base = value->v.s4b;
          break;
        default :
          sderror ("Unknown attribute in Counter object!",
                   value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
counter_err:
      sderror ("Invalid attribute value in Counter object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case RESOURCE :
    {
      oil_resource_object_t * resource = object->object;

      switch (attribute) {
        case ATTR_RESOURCEPROPERTY :
          if ((value->value_type != VALUE_TYPE_RES_RESPROP) &&
              (value->value_type != VALUE_TYPE_OS_STATUS))
            goto resource_err;
          switch (value->v.s4b) {
            case STANDARD :
              resource->property.type = RESOURCE_TYPE_STANDARD;
              break;
            case INTERNAL :
              resource->property.type = RESOURCE_TYPE_INTERNAL;
              break;
            case LINKED :
            {
              oil_param_t * param = NULL;
              oil_object_list_t * index = NULL;
              resource->property.type = RESOURCE_TYPE_LINKED;
              for_each (param, value->list, index) {
                switch (param->attribute) {
                  case ATTR_LINKEDRESOURCE :
                    if (param->value->value_type != VALUE_TYPE_STRING) {
                      sderror ("Invalid LINKEDRESOURCE", value->lineno);
                      return ERR_ATTRIBUTE;
                    }
                    if (resource->property.linked_resource) {
                      printf ("Warning: Multiple linked resources"
                              " specified!\n");
                    }
                    resource->property.linked_resource =
                      get_resource_object (param->value->v.s);
                    break;
                  default :
                    sderror ("Unknown attribute in Resource object!",
                             value->lineno);
                    return ERR_ATTRIBUTE;
                }
              }
              break;
            }
            default :
              goto resource_err;
          }
          break;
        default :
          sderror ("Unknown attribute in Resource object!",
                   value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
resource_err:
      sderror ("Invalid attribute value in Resource object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case ALARM :
    {
      oil_alarm_object_t * alarm = object->object;

      switch (attribute) {
        case COUNTER :
          if (value->value_type != VALUE_TYPE_STRING) goto alarm_err;
          alarm->counter = get_counter_object (value->v.s);
          break;
        case ATTR_AUTOSTART :
        {
          oil_param_t * param = NULL;
          oil_object_list_t * index = NULL;

          if ((value->value_type != VALUE_TYPE_BOOL) &&
              (value->value_type != VALUE_TYPE_BOOL_LIST))
            goto alarm_err;
          alarm->autostart = value->v.b;

          if (alarm->autostart) {
            for_each (param, value->list, index) {
              switch (param->attribute) {
                case ATTR_ALARMTIME :
                  if (param->value->value_type != VALUE_TYPE_INT)
                    goto alarm_err;
                  if (!CHK_RANGE (param->value->v.s8b, MAX_ALARMTIME)) {
                    sderror ("ALARMTIME out of range!", value->lineno);
                    return ERR_ATTRIBUTE;
                  }
                  alarm->alarm_time = param->value->v.s4b;
                  break;
                case ATTR_CYCLETIME :
                  if (param->value->value_type != VALUE_TYPE_INT)
                    goto alarm_err;
                  if (!CHK_RANGE (param->value->v.s8b, MAX_CYCLETIME)) {
                    sderror ("CYCLETIME out of range!", value->lineno);
                    return ERR_ATTRIBUTE;
                  }
                  alarm->cycle_time = param->value->v.s4b;
                  break;
                case APPMODE :
                  if (param->value->value_type != VALUE_TYPE_STRING)
                    goto alarm_err;
                  object_list_add (&(alarm->appmode),
                                   get_appmode_object (param->value->v.s));
                  break;
                default :
                  sderror ("Unknow attribute in Alarm AUTOSTART!",
                           value->lineno);
                  return ERR_ATTRIBUTE;
              }
            }
          }

          break;
        }
        case ATTR_ACTION :
        {
          oil_param_t * param = NULL;
          oil_object_list_t * index = NULL;

          if (value->value_type != VALUE_TYPE_ACTION_LIST)
            goto alarm_err;

          switch (value->v.s4b) {
            case ALARMCALLBACK :
              alarm->action.type = ACTION_TYPE_ALARMCALLBACK;
              break;
            case ACTIVATETASK :
              alarm->action.type = ACTION_TYPE_ACTIVATETASK;
              break;
            case SETEVENT :
              alarm->action.type = ACTION_TYPE_SETEVENT;
              break;
            default :
              sderror ("Unknown action type!", value->lineno);
              return ERR_ATTRIBUTE;
          }

          for_each (param, value->list, index) {
            switch (param->attribute) {
              case TASK :
                if (param->value->value_type != VALUE_TYPE_STRING)
                  goto alarm_err;
                if (alarm->action.task) {
                  printf ("Warning: Multiple Tasks specified!\n");
                }
                alarm->action.task = get_task_object (param->value->v.s);
                break;
              case EVENT :
                if (param->value->value_type != VALUE_TYPE_STRING)
                  goto alarm_err;
                if (alarm->action.event) {
                  printf ("Warning: Multiple Events specified!\n");
                }
                alarm->action.event =
                  get_event_object (param->value->v.s);
                break;
              case ATTR_ALARMCALLBACKNAME :
                if (param->value->value_type != VALUE_TYPE_STRING_CONST)
                  goto alarm_err;
                if (alarm->action.alarm_callback_name) {
                  printf ("Warning: Multiple Callbacks specified!\n");
                }
                alarm->action.alarm_callback_name = param->value->v.s;
                break;
              default :
                sderror ("Unknown attribute in Alarm ACTION!",
                         value->lineno);
                return ERR_ATTRIBUTE;
            }
          }

          break;
        }
        default :
          sderror ("Unknown attribute in Alarm object!", value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
alarm_err:
      sderror ("Invalid attribute value in Alarm object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case ISR :
    {
      oil_isr_object_t * isr = object->object;

      switch (attribute) {
        case ATTR_CATEGORY :
          if (value->value_type != VALUE_TYPE_INT) goto isr_err;
          if ((value->v.s8b != 1) && (value->v.s8b != 2)) {
            sderror ("Invalid ISR category!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          isr->category = value->v.s4b;
          break;
        case ATTR_VECTOR :
          if (value->value_type != VALUE_TYPE_INT) goto isr_err;
          if (!CHK_RANGE (value->v.s8b, MAX_VECTOR)) {
            sderror ("Invalid vector!", value->lineno);
            return ERR_ATTRIBUTE;
          }
          isr->vector = value->v.s4b;
          break;
        case RESOURCE :
          if (value->value_type != VALUE_TYPE_STRING) goto isr_err;
          object_list_add (&(isr->resource),
                           get_resource_object (value->v.s));
          break;
        case MESSAGE :
          sderror ("Message currently not supported!", value->lineno);
          return ERR_ATTRIBUTE;
          break;
        default :
          sderror ("Unknown attribute in ISR object!", value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
isr_err:
      sderror ("Invalid attribute value in ISR object!", value->lineno);
      return ERR_ATTRIBUTE;
    }

    case APPMODE :
    {
      oil_appmode_object_t * appmode = object->object;

      switch (attribute) {
        case ATTR_DEFAULT :
          if (value->value_type != VALUE_TYPE_BOOL) goto appmode_err;
          appmode->default_appmode = value->v.b;
          break;
        default :
          sderror ("Unknown attribute in Appmode object!",
                   value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
appmode_err:
      sderror ("Invalid attribute value in Appmode object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case EXPIRYPOINT :
    {
      oil_expiry_point_object_t * exp = object->object;

      switch (attribute) {
        case ATTR_OFFSET :
          if (value->value_type != VALUE_TYPE_INT) goto exp_err;
          if (!CHK_RANGE (value->v.s8b, MAX_MAXALLOWEDVALUE)) {
            sderror ("Expiry Point offset out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          exp->offset = value->v.s4b;
          break;
        case ATTR_MAXLENGTHEN :
          if (value->value_type != VALUE_TYPE_INT) goto exp_err;
          if (!CHK_RANGE (value->v.s8b, MAX_MAXALLOWEDVALUE)) {
            sderror ("Expiry Point max_lengthen out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          exp->max_lengthen = value->v.s4b;
          break;
        case ATTR_MAXSHORTEN :
          if (value->value_type != VALUE_TYPE_INT) goto exp_err;
          if (!CHK_RANGE (value->v.s8b, MAX_MAXALLOWEDVALUE)) {
            sderror ("Expiry Point max_shorten out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          exp->max_shorten = value->v.s4b;
          break;
        case ATTR_SCHEDTBLACTION :
        {
          oil_param_t * param = NULL;
          oil_object_list_t * index = NULL;
          static oil_expiry_point_event_object_t * task_event = NULL;
          int type = 0;

          if (value->value_type != VALUE_TYPE_ACTION_LIST)
            goto exp_err;

          switch (value->v.s4b) {
            case ACTIVATETASK :
              type = ACTION_TYPE_ACTIVATETASK;
              break;
            case SETEVENT :
              type = ACTION_TYPE_SETEVENT;
              break;
            default :
              sderror ("Unknown schedule table action type!",
                       value->lineno);
              return ERR_ATTRIBUTE;
          }

          for_each (param, value->list, index) {
            switch (param->attribute) {
              case TASK :
                if (param->value->value_type != VALUE_TYPE_STRING)
                  goto exp_err;
                if (type == ACTION_TYPE_ACTIVATETASK) {
                  /* Action is ACTIVATETASK */
                  object_list_add (&exp->tasks,
                                   get_task_object (param->value->v.s));
                  exp->num_tasks++;
                } else {
                  /* Action is SETEVENT */
                  if (!task_event) {
                    task_event =
                       new_oil_object (oil_expiry_point_event_object_t);
                    task_event->task = NULL;
                    task_event->event = NULL;
                  } else {
                    if (task_event->task) {
                      printf ("Warning: Multiple Tasks specified!\n");
                    }
                  }
                  task_event->task = get_task_object (param->value->v.s);
                }
                break;
              case EVENT :
                if (param->value->value_type != VALUE_TYPE_STRING)
                  goto exp_err;
                if (type != ACTION_TYPE_SETEVENT) {
                  sderror ("Unknown attribute in SCHEDTBLACTION!",
                           param->value->lineno);
                  return ERR_ATTRIBUTE;
                }
                if (!task_event) {
                  task_event =
                     new_oil_object (oil_expiry_point_event_object_t);
                  task_event->task = NULL;
                  task_event->event = NULL;
                } else {
                  if (task_event->event) {
                    printf ("Warning: Multiple Events specified!\n");
                  }
                }
                task_event->event = get_event_object (param->value->v.s);
                break;
              default :
                sderror ("Unknown attribute in SCHEDTBLACTION!",
                         param->value->lineno);
                return ERR_ATTRIBUTE;
            }
          }

          if (type == ACTION_TYPE_SETEVENT) {
            if (!(task_event) ||
                !(task_event->task) || !(task_event->event)) {
              sderror ("SETEVENT action incomplete!", value->lineno);
              return ERR_ATTRIBUTE;
            } else {
              object_list_add (&exp->task_events, task_event);
              exp->num_task_events++;
            }
          }

          task_event = NULL;

          break;
        }
        default :
          sderror ("Unknown attribute in EXPIRYPOINT object!",
                   value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
exp_err:
      sderror ("Invalid attribute value EXPIRYPOINT object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    case SCHEDULETABLE :
    {
      oil_sched_tbl_object_t * sched_tbl = object->object;

      switch (attribute) {
        case COUNTER :
          if (value->value_type != VALUE_TYPE_STRING) goto sched_tbl_err;
          sched_tbl->counter = get_counter_object (value->v.s);
          break;
        case ATTR_DURATION :
          if (value->value_type != VALUE_TYPE_INT) goto sched_tbl_err;
          if (!CHK_RANGE (value->v.s8b, MAX_DURATION)) {
            sderror ("Schedule Table DURATION out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          sched_tbl->duration = value->v.s4b;
          break;
        case ATTR_REPEATING :
          if (value->value_type != VALUE_TYPE_BOOL) goto sched_tbl_err;
          sched_tbl->repeating = value->v.b;
          break;
        case ATTR_SYNCSTRATEGY :
          if (value->value_type != VALUE_TYPE_STRATEGY)
            goto sched_tbl_err;
          switch (value->v.s4b) {
            case EXPLICIT :
              sched_tbl->sync_strategy = SCHEDTBL_SYNC_EXPLICIT;
              break;
            case IMPLICIT :
              sched_tbl->sync_strategy = SCHEDTBL_SYNC_IMPLICIT;
              break;
            case NONE :
              sched_tbl->sync_strategy = SCHEDTBL_SYNC_NONE;
              break;
            default :
              sderror ("Unknown synchronization strategy!",
                       value->lineno);
              return ERR_ATTRIBUTE;
          }
          break;
        case ATTR_PRECISION :
          if (value->value_type != VALUE_TYPE_INT) goto sched_tbl_err;
          if (!CHK_RANGE (value->v.s8b, MAX_DURATION)) {
            sderror ("Schedule Table PRECISION out of range!",
                     value->lineno);
            return ERR_ATTRIBUTE;
          }
          sched_tbl->precision = value->v.s4b;
          break;
        case EXPIRYPOINT :
          if (value->value_type != VALUE_TYPE_STRING)
            goto sched_tbl_err;
          object_list_add (&(sched_tbl->exps),
                           get_expiry_point_object (value->v.s));
          sched_tbl->num_exps++;
          break;
        case ATTR_AUTOSTART :
        {
          oil_param_t * param = NULL;
          oil_object_list_t * index = NULL;

          if ((value->value_type != VALUE_TYPE_BOOL) &&
              (value->value_type != VALUE_TYPE_BOOL_LIST))
            goto sched_tbl_err;
          sched_tbl->autostart = value->v.b;

          if (sched_tbl->autostart) {
            for_each (param, value->list, index) {
              switch (param->attribute) {
                case ATTR_STARTMODE :
                  if (param->value->value_type != VALUE_TYPE_START_MODE)
                    goto sched_tbl_err;
                  switch (param->value->v.s4b) {
                    case ABSOLUTE :
                      sched_tbl->start_mode =
                        SCHEDTBL_STARTMODE_ABSOLUTE;
                      break;
                    case RELATIVE :
                      sched_tbl->start_mode =
                        SCHEDTBL_STARTMODE_RELATIVE;
                      break;
                    case SYNCHRON :
                      sched_tbl->start_mode =
                        SCHEDTBL_STARTMODE_SYNCHRON;
                      break;
                    default :
                      sderror ("Unknown schedule table start mode!",
                               param->value->lineno);
                      return ERR_ATTRIBUTE;
                  }
                  break;
                case ATTR_STARTVALUE :
                  if (param->value->value_type != VALUE_TYPE_INT)
                    goto sched_tbl_err;
                  if (!CHK_RANGE (param->value->v.s8b,
                      MAX_MAXALLOWEDVALUE)) {
                    sderror ("STARTVALUE out of range!",
                             param->value->lineno);
                    return ERR_ATTRIBUTE;
                  }
                  sched_tbl->start_value = param->value->v.s4b;
                  break;
                case APPMODE :
                  if (param->value->value_type != VALUE_TYPE_STRING)
                    goto sched_tbl_err;
                  object_list_add (&(sched_tbl->appmode),
                                   get_appmode_object (param->value->v.s));
                  break;
                default :
                  sderror ("Unknow attribute in Schedule Table AUTOSTART!",
                           param->value->lineno);
                  return ERR_ATTRIBUTE;
              }
            }
          }

          break;
        }
        default :
          sderror ("Unknown attribute in SCHEDULETABLE object!",
                   value->lineno);
          return ERR_ATTRIBUTE;
      }

      break;
sched_tbl_err:
      sderror ("Invalid attribute value in SCHEDULETABLE object!",
               value->lineno);
      return ERR_ATTRIBUTE;
    }

    default :
      sderror ("Unknown object!", value->lineno);
      return ERR_OBJECT;
  }

  return 0;
}

typedef struct {
  uint32_t prio;
  uint32_t id;
} prios_t;

static int
cmpfunc1 (const void * a, const void * b)
{
   prios_t * pa, * pb;
   pa = (prios_t *) a;
   pb = (prios_t *) b;

   return pa->prio - pb->prio;
}

static int
cmpfunc2 (const void * a, const void * b)
{
   prios_t * pa, * pb;
   pa = (prios_t *) a;
   pb = (prios_t *) b;

   return pa->id - pb->id;
}

static int
cmpfunc3 (const void * a, const void * b)
{
  oil_expiry_point_object_t * epa, * epb;
  epa = (*(oil_object_list_t **) a)->data.ep;
  epb = (*(oil_object_list_t **) b)->data.ep;

  return epa->offset - epb->offset;
}

static void
update_oil_objects ()
{
  oil_task_object_t * task = NULL;
  oil_event_object_t * event = NULL;
  oil_appmode_object_t * appmode = NULL;
  oil_counter_object_t * counter = NULL;
  oil_resource_object_t * resource = NULL, * resource2 = NULL;
  oil_alarm_object_t * alarm = NULL;
  oil_isr_object_t * isr = NULL;
  oil_sched_tbl_object_t * sched_tbl = NULL;
  oil_expiry_point_object_t * exp = NULL, * prev_exp = NULL;
  oil_driver_object_t * driver = NULL;
  oil_object_list_t ** tmp_list = NULL;
  oil_object_list_t * index = NULL, * index2 = NULL, * index3 = NULL;
  uint32_t id = 0, i = 0, cur_p = 0, prev_p = 0, max_off = 0;
  prios_t * prios = malloc (sizeof (prios_t) * num_tasks);
  bool appmode_default_set = FALSE;
  uint32_t * cnter_cnt;

  /* Update OS object */
  if (oil_os->board == NULL) {
    fprintf (stderr, "Board is not specified!\n");
    exit (1);
  }

  /* Update task objects */
  id = 1;
  max_activation = 0;
  for_each (task, oil_tasks, index) {
    int icount = 0;
    if (!(task->stacksize)) {
      fprintf (stderr, "%s stack size not specified!\n", task->name);
      exit (1);
    }
    task->id = id;
    for_each (resource, task->resource, index2) {
      if (resource->property.type == RESOURCE_TYPE_INTERNAL)
        icount++;
    }
    if (icount > 1) {
      fprintf (stderr, "%s has more than 1 internal resources!\n",
               task->name);
      exit (1);
    }
    icount = 0;
    if (task->priority == -1) {
      fprintf (stdout, "Warning: %s priority not specified."
               " Default to lowest task priority (1).\n", task->name);
      task->priority = 1;
    }
    if (task->activation == 0) {
      fprintf (stdout, "Warning: %s activation not specified."
               " Default to 1.\n", task->name);
      task->activation = 1;
    }
    /* Multiple activation should be enabled */
    if (task->activation > 1) {
      mult_activation = TRUE;
      mult_task_per_prio = TRUE;
    }
    if (max_activation < task->activation)
      max_activation = task->activation;
    prios[id - 1].id = id;
    prios[id - 1].prio = task->priority;
    id++;
  }

  /* Re-assign priorities */
  qsort (prios, num_tasks, sizeof (prios_t), cmpfunc1);

  cur_p = 1;
  for (i = 0; i < num_tasks; i++) {
    if (!prev_p) {
      prev_p = prios[i].prio;
      prios[i].prio = cur_p;
    } else if (prios[i].prio == prev_p) {
      /* Same priority as previous task */
      prios[i].prio = cur_p;
      mult_task_per_prio = TRUE;
    } else {
      prev_p = prios[i].prio;
      prios[i].prio = (++cur_p);
    }
  }

  max_prio = cur_p;

  qsort (prios, num_tasks, sizeof (prios_t), cmpfunc2);

  i = 0;
  for_each (task, oil_tasks, index) {
    task->priority = prios[i++].prio;
  }

  /* Update event objects */
  id = 0;
  max_mask = 1;
  for_each (event, oil_events, index) {
    event->id = id++;
    if (event->mask != -1) {
      if (event->mask & masks) {
        fprintf (stderr, "%s mask conflict!\n", event->name);
        exit (1);
      } else {
        masks |= event->mask;
      }
    } else {
      /* AUTO mask */
      for (i = 0; i < 64; i++) {
        if (!((masks >> i) & 0x1ULL)) {
          event->mask = (0x1ULL << i);
          masks |= event->mask;
          break;
        }
      }
    }
    if (event->mask == -1) {
      fprintf (stderr, "%s mask cannot be allocated!\n", event->name);
      exit (1);
    }
    if (max_mask < event->mask) max_mask = event->mask;
  }

  /* Update appmode objects */
  id = 1;
  i = 0;
  for_each (appmode, oil_appmodes, index) {
    if (appmode->default_appmode) {
      appmode_default_set = TRUE;
      if (i > 0) {
        fprintf (stdout, "Warning: Multiple default appmodes!\n");
        fprintf (stdout, "%s set to non-default\n", appmode->name);
        appmode->default_appmode = FALSE;
      }
      i++;
    }
  }
  if (!appmode_default_set) {
    fprintf (stdout, "Warning: Default appmode not specified!\n");
    fprintf (stdout, "%s set to default\n",
             oil_appmodes->data.appmode->name);
    oil_appmodes->data.appmode->default_appmode = TRUE;
  }
  for_each (appmode, oil_appmodes, index) {
    if (appmode->default_appmode)
      appmode->id = 0;
    else
      appmode->id = id++;
  }

  /* Update counter objects */
  id = 0;
  max_tick = 0;
  /*
   * We should always have a SYS_COUNTER. If the user has not
   * specificied it in OIL, we create it here.
   */
  counter = get_counter_object ("SYS_COUNTER");
  if (!counter->min_cycle) counter->min_cycle = 1;
  if (!counter->max_allowed_value) counter->max_allowed_value = 0xFFFF;
  if (!counter->ticks_per_base) counter->ticks_per_base = 1;

  for_each (counter, oil_counters, index) {
    counter->id = id++;
    if (counter->min_cycle == 0) {
      fprintf (stderr, "%s min_cycle not specified!\n", counter->name);
      exit (1);
    }
    if (counter->max_allowed_value == 0) {
      fprintf (stderr, "%s max_allowed_value not specified!\n",
               counter->name);
      exit (1);
    }
    if (counter->ticks_per_base == 0) {
      fprintf (stderr, "%s ticks_per_base not specified!\n",
               counter->name);
      exit (1);
    }
    /* We need to make sure max_allowed_value can fit into atomic_t */
    if (strncmp (oil_os->board, "ARDUINO_UNO", 11) == 0) {
      if (counter->max_allowed_value > 0xFFFF) {
        fprintf (stderr, "Counter %s max_allowed_value illegal!\n",
                 counter->name);
        fprintf (stderr, "Board max_allowed_value is 0xFFFF\n");
        exit (1);
      }
    } else {
      if (counter->max_allowed_value > 0xFFFFFFFF) {
        fprintf (stderr, "Counter %s max_allowed_value illegal!\n",
                 counter->name);
        fprintf (stderr, "Board max_allowed_value is 0xFFFFFFFF\n");
        exit (1);
      }
    }
    if (max_tick < counter->max_allowed_value)
      max_tick = counter->max_allowed_value;
  }

  /* Update resource objects */
  if (oil_os->use_resscheduler)
    id = 1;
  else
    id = 0;
  for_each (resource, oil_resources, index) {
    if (resource->property.type != RESOURCE_TYPE_INTERNAL)
      resource->id = id++;
    else
      /* Internal resource is not generated as object */
      resource->id = -1;
    if (resource->property.type == -1) {
      fprintf (stderr, "%s property not specified!\n", resource->name);
      exit (1);
    }
    /*
     * Ceiling priority of resource should be the highest priority
     * of all the tasks that access the resource.
     */
    for_each (task, oil_tasks, index2) {
      for_each (resource2, task->resource, index3) {
        if (strncmp (resource->name, resource2->name,
                     strlen (resource->name)) == 0) {
          if (resource->priority < task->priority)
            resource->priority = task->priority;
        }
      }
    }
  }

  /* Handle linked resources */
  for_each (resource, oil_resources, index) {
    oil_resource_object_t * res = NULL;
    uint32_t rprio = 0;
    res = resource;

    if (res->property.type == RESOURCE_TYPE_LINKED) {
      rprio = res->priority;
      do {
        if (res->property.linked_resource->property.type ==
            RESOURCE_TYPE_INTERNAL) {
          fprintf (stderr, "Cannot link %s to internal resource %s!\n",
                   res->name, res->property.linked_resource->name);
          exit (1);
        }
        res = res->property.linked_resource;
        if (rprio < res->priority) {
          rprio = res->priority;
        }
      } while (res->property.type == RESOURCE_TYPE_LINKED);

      /* Now rprio has the highest prio of all linked resources */
      res = resource;
      res->priority = rprio;
      /* Set ceiling priority of all linked resources to rprio */
      do {
        res = res->property.linked_resource;
        res->priority = rprio;
      } while (res->property.type == RESOURCE_TYPE_LINKED);
      /* Reset rprio to 0 */
      rprio = 0;
    }
  }

  /* We should not have resource with priority 0 */
  for_each (resource, oil_resources, index) {
    if (resource->priority == 0) {
      fprintf (stdout, "Warning: %s is not associated with any task!\n",
               resource->name);
    }
  }

  /* Update alarm objects */
  id = 0;
  for_each (alarm, oil_alarms, index) {
    alarm->id = id++;
    if (!alarm->counter) {
      fprintf (stderr, "%s counter not specified!\n", alarm->name);
      exit (1);
    }
    if (alarm->action.type == -1) {
      fprintf (stderr, "%s action not specified!\n", alarm->name);
      exit (1);
    }
    switch (alarm->action.type) {
      case ACTION_TYPE_ACTIVATETASK :
        if (!(alarm->action.task)) {
          fprintf (stderr, "%s task not specified!\n",
                   alarm->name);
          exit (1);
        }
        break;
      case ACTION_TYPE_SETEVENT :
        if (!(alarm->action.event)) {
          fprintf (stderr, "%s event not specified!\n",
                   alarm->name);
          exit (1);
        }
        break;
      case ACTION_TYPE_ALARMCALLBACK :
        if (!(alarm->action.alarm_callback_name)) {
          fprintf (stderr, "%s callback not specified!\n",
                   alarm->name);
          exit (1);
        }
        break;
      default :
        fprintf (stderr, "%s action unknown!\n", alarm->name);
        exit (1);
    }
    if (alarm->autostart && alarm->alarm_time == -1) {
      fprintf (stderr, "%s alarm_time not specified!\n", alarm->name);
      exit (1);
    }
    if (alarm->autostart && alarm->cycle_time == -1) {
      fprintf (stderr, "%s cycle_time not specified!\n", alarm->name);
      exit (1);
    }
  }

  /* Update isr objects */
  id = 0;
  for_each (isr, oil_isrs, index) {
    if ((isr->category != 1) && (isr->category != 2)) {
      fprintf (stderr, "%s category invalid!\n", isr->name);
      exit (1);
    }
    if (isr->vector == -1) {
      fprintf (stderr, "%s vector not specified!\n", isr->name);
      exit (1);
    }
    if (isr->category == 1) num_isr1s++;
    if (isr->category == 2) num_isr2s++;
    if (max_irq < isr->vector) max_irq = isr->vector;
  }

  /* Update SCHEDULETABLE object */
  id = 0;
  cnter_cnt = malloc (sizeof (uint32_t) * num_counters);
  memset (cnter_cnt, 0, sizeof (uint32_t) * num_counters);
  for_each (sched_tbl, oil_sched_tbls, index) {
    sched_tbl->id = id++;
    with_sched_tbl = TRUE;

    if (!sched_tbl->counter) {
      fprintf (stderr, "%s counter not specified!\n", sched_tbl->name);
      exit (1);
    } else {
      if ((++cnter_cnt[sched_tbl->counter->id]) > 1)
        mult_schedtbl_per_cntr = TRUE;
    }

    if (sched_tbl->duration == -1) {
      fprintf (stderr, "%s duration not specified!\n",
               sched_tbl->name);
      exit (1);
    }

    if (sched_tbl->autostart) {
      if (sched_tbl->start_mode == SCHEDTBL_STARTMODE_NONE) {
        fprintf (stderr, "%s is set to auto start but without"
                 " start mode!\n", sched_tbl->name);
        exit (1);
      }

      if ((sched_tbl->start_mode != SCHEDTBL_STARTMODE_SYNCHRON) &&
          (sched_tbl->start_value == -1)) {
        fprintf (stderr, "%s is set to auto start but without"
                 " start value!\n", sched_tbl->name);
        exit (1);
      }

      if (sched_tbl->start_value == -1) sched_tbl->start_value = 0;

      if (sched_tbl->start_mode == SCHEDTBL_STARTMODE_SYNCHRON &&
          sched_tbl->sync_strategy != SCHEDTBL_SYNC_EXPLICIT) {
        fprintf (stderr, "%s is set to auto start synchronously"
                 " without EXPLICIT synchronization strategy!\n",
                 sched_tbl->name);
        exit (1);
      }

      for_each (appmode, sched_tbl->appmode, index2) {
        num_autostart_sched_tbls++;
      }
    }

    if (sched_tbl->sync_strategy != SCHEDTBL_SYNC_NONE) {
      with_sched_tbl_sync = TRUE;
    }

    if (sched_tbl->sync_strategy == SCHEDTBL_SYNC_EXPLICIT) {
      if (sched_tbl->precision == -1) {
        fprintf (stderr, "%s is explicitly synchronized but"
                 " without a precision!\n", sched_tbl->name);
        exit (1);
      }

      if (sched_tbl->precision > sched_tbl->duration) {
        fprintf (stderr, "%s precision out of range!\n",
                 sched_tbl->name);
        exit (1);
      }
    }

    if (sched_tbl->num_exps == 0) {
      fprintf (stderr, "%s does not have expiry point!\n",
               sched_tbl->name);
      exit (1);
    }

    /* Create a temporary object list to sort EPs */
    tmp_list = malloc (sizeof (oil_object_list_t *) *
                       sched_tbl->num_exps);
    if (!tmp_list) {
      fprintf (stderr, "Cannot create temporary object list!\n");
      exit (1);
    }
    i = 0;

    for_each (exp, sched_tbl->exps, index2) {
      if (exp->offset == -1) {
        fprintf (stderr, "%s does not have offset!\n", exp->name);
        exit (1);
      }
      if (exp->tasks == NULL &&
          exp->task_events == NULL) {
        fprintf (stderr, "%s does not have actions!\n",
                 exp->name);
        exit (1);
      }
      if (sched_tbl->sync_strategy == SCHEDTBL_SYNC_EXPLICIT) {
        if ((exp->max_lengthen == -1) ||
            (exp->max_shorten == -1)) {
          fprintf (stderr, "%s does not have a max_lengthen"
                   " or max_shorten value!\n", exp->name);
          exit (1);
        }
      }
      tmp_list[i++] = index2;
    }

    /* Sort EPs for sched_tbl */
    qsort (tmp_list, sched_tbl->num_exps,
           sizeof (oil_object_list_t *), cmpfunc3);

    max_off = tmp_list[sched_tbl->num_exps - 1]->data.ep->offset;

    sched_tbl->exps = tmp_list[0];
    tmp_list[sched_tbl->num_exps - 1]->next = NULL;
    for (i = 0; i < sched_tbl->num_exps - 1; i++) {
      tmp_list[i]->next = tmp_list[i + 1];
    }

    /* Now, sched_tbl's EPs are sorted by their offsets */
    /* Check expiry point consistency */
    prev_exp = NULL;
    for_each (exp, sched_tbl->exps, index2) {
      if ((exp->max_lengthen != -1) &&
          (exp->max_lengthen >= sched_tbl->duration)) {
        fprintf (stderr, "%s has illegal lengthen value!\n",
                 exp->name);
        exit (1);
      }

      if (index2->next) {
        if ((exp->offset + sched_tbl->counter->min_cycle) >
            index2->next->data.ep->offset) {
          fprintf (stderr, "%s has illegal offset!\n", exp->name);
          exit (1);
        }
      } else {
        /* exp is last EP */
        if (exp->offset > sched_tbl->duration) {
          fprintf (stderr, "%s has mismatch duration!\n",
                   sched_tbl->name);
          exit (1);
        }
      }

      if (prev_exp == NULL) {
        /* exp is first EP */
        if (exp->offset == 0) {
          if ((exp->max_shorten != -1) &&
              (exp->max_shorten != 0)) {
            fprintf (stderr, "%s has illegal shorten value!\n",
                     exp->name);
            exit (1);
          }
        } else {
          if ((exp->offset < sched_tbl->counter->min_cycle) ||
              (exp->offset > sched_tbl->counter->max_allowed_value)) {
            fprintf (stderr, "%s has illegal offset!\n", exp->name);
            exit (1);
          }

          if ((exp->max_shorten != -1) && (exp->max_shorten >
              (exp->offset - sched_tbl->counter->min_cycle))) {
            fprintf (stderr, "%s has illegal shorten value!\n",
                     exp->name);
            exit (1);
          }
        }
      } else {
        if (((exp->offset - prev_exp->offset) <
            sched_tbl->counter->min_cycle) ||
            ((exp->offset - prev_exp->offset) >
            sched_tbl->counter->max_allowed_value)) {
          fprintf (stderr, "%s has illegal offset!\n", exp->name);
          exit (1);
        }

        if ((exp->max_shorten != -1) &&
            ((exp->offset - prev_exp->offset) <
             (exp->max_shorten + sched_tbl->counter->min_cycle))) {
          fprintf (stderr, "%s has illegal shorten value!\n",
                   exp->name);
          exit (1);
        }
      }

      prev_exp = exp;
    }

    sched_tbl->delay = sched_tbl->duration - max_off;
  }

  /* Update driver objects */
  if (num_drivers) {
    id = 0;
    /*
     * We need to reverse the driver list. The list is ordered
     * based on dependencies. When the parser created the list,
     * it is the opposite to the order in the configuration
     * file.
     */
    tmp_list = malloc (sizeof (oil_object_list_t *) * num_drivers);

    i = 0;
    for_each (driver, oil_drivers, index) {
      tmp_list[i++] = index;
    }

    oil_drivers = tmp_list[num_drivers - 1];

    for (i = num_drivers - 1; i > 0; i--) {
      tmp_list[i]->next = tmp_list[i - 1];
    }

    tmp_list[0]->next = NULL;

    for_each (driver, oil_drivers, index) {
      int l = strlen (driver->name);
      for (i = 0; i < l; i++) {
        driver->name[i] = driver->name[i + 1];
      }
      driver->name[l - 2] = '\0';
      l = strlen (driver->name);
      for (i = l; i > 0; i--) {
        if (driver->name[i] == '/') break;
      }
      driver->file_name = malloc (l - i);
      if (!driver->file_name) {
        fprintf (stderr, "malloc failed!\n");
        exit (1);
      }
      memcpy (driver->file_name, &(driver->name[i + 1]), l - i);
      driver->id = id++;
    }
  }

  return;
}

static char *
get_type_string (uint64_t value)
{
  if (value <= 0xFFULL) return "uint8_t";
  if (value <= 0xFFFFULL) return "uint16_t";
  if (value <= 0xFFFFFFFFULL) return "uint32_t";
  if (value <= 0xFFFFFFFFFFFFFFFFULL) return "uint64_t";
  return NULL;
}

#define RESET_COLOR  "\e[m"
#define GREEN_COLOR  "\e[32m"
#define RED_COLOR    "\e[31m"

static void
generate_code ()
{
  int i = 0;
  oil_task_object_t * task = NULL;
  oil_event_object_t * event = NULL;
  oil_appmode_object_t * appmode = NULL;
  oil_counter_object_t * counter = NULL;
  oil_resource_object_t * resource = NULL;
  oil_alarm_object_t * alarm = NULL;
  oil_isr_object_t * isr = NULL;
  oil_sched_tbl_object_t * sched_tbl = NULL;
  oil_expiry_point_object_t * exp = NULL;
  oil_expiry_point_event_object_t * task_event = NULL;
  oil_driver_object_t * driver = NULL;
  oil_object_list_t * index = NULL, * index2 = NULL;

  /* Tmp file for config.mk */
  FILE * tmp_cfgmk = NULL;
  /* Tmp file for config/config.h */
  FILE * tmp_cfgh = NULL;
  /* Tmp file for config/config.c */
  FILE * tmp_cfgc = NULL;
  /* Tmp file for apps/config.mk */
  FILE * tmp_appsmk = NULL;

  if (tflag) {
    /* For test/verify, code generated to STDOUT */
    tmp_cfgmk = stdout;
    tmp_cfgh = stdout;
    tmp_cfgc = stdout;
    tmp_appsmk = stdout;
  } else {
    if (!(tmp_cfgmk = tmpfile ()) ||
        !(tmp_cfgh = tmpfile ()) ||
        !(tmp_cfgc = tmpfile ()) ||
        !(tmp_appsmk = tmpfile ())) {
      fprintf (stderr, "Cannot create tmp file for code generation!\n");
      exit (1);
    }
  }
  
  /* Generating config.mk */
#define PRT_CFGMK(fmt, ...) fprintf (tmp_cfgmk, fmt, ##__VA_ARGS__)
  PRT_CFGMK ("# This file is generated by system generator\n");
  PRT_CFGMK ("# Set target board first\n");
  PRT_CFGMK ("BOARD = %s\n", oil_os->board);
  if (oil_os->debuglevel > 0)
    PRT_CFGMK ("CFG += -DDEBUG_SDVOS\n");
  if (oil_os->debuglevel > 1)
    PRT_CFGMK ("CFG += -DDEBUG_SDVOS_VERBOSE\n");
  PRT_CFGMK ("#CFG += -DPRINT_BANNER\n");
  PRT_CFGMK ("#MULTI_ACTIVATION can only be defined with MULTI_TASK_PER_PRIO\n");
  if (mult_activation) {
    PRT_CFGMK ("CFG += -DMULTI_ACTIVATION\n");
    PRT_CFGMK ("CFG += -DMULTI_TASK_PER_PRIO\n");
  } else if (mult_task_per_prio) {
    PRT_CFGMK ("CFG += -DMULTI_TASK_PER_PRIO\n");
  }
  if (oil_os->status == OS_STATUS_STANDARD) {
    PRT_CFGMK ("CFG += -DOSEK_STANDARD\n");
  } else if (oil_os->status == OS_STATUS_EXTENDED) {
    PRT_CFGMK ("CFG += -DOSEK_EXTENDED\n");
  }
  if (with_sched_tbl)
    PRT_CFGMK ("\nCFG += -DUSE_SCHEDTBL\n");
  if (mult_schedtbl_per_cntr) {
    PRT_CFGMK ("# Allow multiple schedule table processing for a single counter\n");
    PRT_CFGMK ("CFG += -DMULTI_SCHEDTBL_PER_CNTR\n");
  }
  if (with_sched_tbl_sync)
    PRT_CFGMK ("CFG += -DSCHEDTBL_SYNC\n");
  PRT_CFGMK ("\n");
  PRT_CFGMK ("# Selected objects to be compiled\n");

  PRT_CFGMK ("OBJ += task.o\n");
  PRT_CFGMK ("OBJ += osctl.o\n");
  PRT_CFGMK ("OBJ += interrupt.o\n");
  PRT_CFGMK ("OBJ += resource.o\n");
  PRT_CFGMK ("OBJ += event.o\n");
  PRT_CFGMK ("OBJ += alarm.o\n");
  PRT_CFGMK ("OBJ += debug.o\n");
  PRT_CFGMK ("OBJ += config/config.o\n");
  PRT_CFGMK ("OBJ += printf.o\n");

  if (oil_os->shell) {
    /* Shell configured */
    PRT_CFGMK ("OBJ += shell.o\n");
  }

  if (with_sched_tbl) {
    PRT_CFGMK ("# AUTOSAR objects\n");
    PRT_CFGMK ("OBJ += schedtbl.o\n");
  }
  PRT_CFGMK ("OBJ += drivers/driver.o\n");
  PRT_CFGMK ("\n");
  PRT_CFGMK ("# Driver Modules\n");
  for_each (driver, oil_drivers, index) {
   PRT_CFGMK ("OBJ += drivers/%s.o\n", driver->name);
  }
  PRT_CFGMK ("\n");
  fprintf (stdout, "Generating config.mk                   ");
  fprintf (stdout, "[" GREEN_COLOR "OK" RESET_COLOR "]\n");
#undef PRT_CFGMK

  /* Generating config.h */
#define PRT_CFGH(fmt, ...) fprintf (tmp_cfgh, fmt, ##__VA_ARGS__)
  PRT_CFGH ("/**********************************************/\n");
  PRT_CFGH ("/* This file is generated by system generator */\n");
  PRT_CFGH ("/* Do not modify manually!                    */\n");
  PRT_CFGH ("/**********************************************/\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("#ifndef _CONFIG_CONFIG_H_\n");
  PRT_CFGH ("#define _CONFIG_CONFIG_H_\n");
  PRT_CFGH ("\n");
  /* Tasks */
  PRT_CFGH ("/* Tasks */\n");
  for_each (task, oil_tasks, index) {
    PRT_CFGH ("#define %s\t0x%XU\n", task->name, task->id);
  }
  /* Need to add idle task */
  PRT_CFGH ("#define NUM_TASKS\t0x%XU\n", num_tasks + 1);
  PRT_CFGH ("#define MAX_PRIO\t0x%XU\n", max_prio);
  PRT_CFGH ("\n");
  /* AppModes */
  PRT_CFGH ("/* AppModes */\n");
  for_each (appmode, oil_appmodes, index) {
    PRT_CFGH ("#define %s\t0x%XU\n", appmode->name, appmode->id);
  }
  PRT_CFGH ("\n");
  /* Counters */
  PRT_CFGH ("/* Counters */\n");
  for_each (counter, oil_counters, index) {
    PRT_CFGH ("#define %s\t0x%XU\n", counter->name, counter->id);
    PRT_CFGH ("/* Counter related constants */\n");
    PRT_CFGH ("#define OSMAXALLOWEDVALUE_%s\t0x%X\n",
              counter->name, counter->max_allowed_value);
    PRT_CFGH ("#define OSTICKSPERBASE_%s\t0x%X\n",
              counter->name, counter->ticks_per_base);
    PRT_CFGH ("#define OSMINCYCLE_%s\t0x%X\n",
              counter->name, counter->min_cycle);
    if (strncmp (counter->name, "SYS_COUNTER",
                 strlen ("SYS_COUNTER")) == 0) {
      /* SYS_COUNTER */
      PRT_CFGH ("/* System counter constants */\n");
      PRT_CFGH ("#define OSMAXALLOWEDVALUE\t0x%X\n",
                counter->max_allowed_value);
      PRT_CFGH ("#define OSTICKSPERBASE\t0x%X\n",
                counter->ticks_per_base);
      PRT_CFGH ("#define OSMINCYCLE\t0x%X\n",
                counter->min_cycle);
    }
  }
  PRT_CFGH ("\n");
  PRT_CFGH ("#define NUM_COUNTERS\t0x%XU\n", num_counters);
  PRT_CFGH ("\n");
  /* Alarms */
  PRT_CFGH ("/* Alarms */\n");
  for_each (alarm, oil_alarms, index) {
    PRT_CFGH ("#define %s\t0x%XU\n", alarm->name, alarm->id);
  }
  PRT_CFGH ("#define NUM_ALARMS\t0x%XU\n", num_alarms);
  PRT_CFGH ("\n");
  /* Resources */
  PRT_CFGH ("/* Resources */\n");
  if (oil_os->use_resscheduler)
    PRT_CFGH ("#define RES_SCHEDULER\t0x0U\n");
  for_each (resource, oil_resources, index) {
    if (resource->property.type != RESOURCE_TYPE_INTERNAL)
      PRT_CFGH ("#define %s\t0x%XU\n", resource->name, resource->id);
  }
  PRT_CFGH ("#define NUM_RESOURCES\t0x%XU\n",
            oil_os->use_resscheduler ? num_resources + 1 :
                                       num_resources);
  PRT_CFGH ("\n");
  /* Events */
  PRT_CFGH ("/* Events */\n");
  for_each (event, oil_events, index) {
    PRT_CFGH ("#define %s\t0x%" PRIu64 "U\n", event->name, event->mask);
  }
  PRT_CFGH ("#define NUM_EVENTS\t0x%XU\n", num_events);
  PRT_CFGH ("\n");
  /* ISRs */
  PRT_CFGH ("/* ISRs */\n");
  PRT_CFGH ("#define NUM_ISR1\t0x%XU\n", num_isr1s);
  PRT_CFGH ("#define NUM_ISR2\t0x%XU\n", num_isr2s);
  PRT_CFGH ("\n");
  /* Schedule Tables */
  PRT_CFGH ("/* Schedule Tables */\n");
  for_each (sched_tbl, oil_sched_tbls, index) {
    PRT_CFGH ("#define %s\t0x%XU\n", sched_tbl->name, sched_tbl->id);
  }
  PRT_CFGH ("#define NUM_SCHED_TBLS\t0x%XU\n", num_sched_tbls);
  PRT_CFGH ("#define NUM_AUTOSTART_SCHED_TBLS\t0x%XU\n", num_autostart_sched_tbls);
  PRT_CFGH ("\n");
  /* Drivers */
  PRT_CFGH ("/* Drivers */\n");
  PRT_CFGH ("#define NUM_DRIVERS\t0x%XU\n", num_drivers);
  PRT_CFGH ("\n");

  PRT_CFGH ("/* Stack allocation for kernel and tasks */\n");
  PRT_CFGH ("/* %s */\n", oil_os->board);
  PRT_CFGH ("#define SRAM_END          (ARCH_SRAM_END)\n");
  PRT_CFGH ("#define KERN_STACK        SRAM_END\n");
  // TODO Kernel Stack Size?
  PRT_CFGH ("#define KERN_STACK_END    (KERN_STACK - KERN_STK_SIZE)\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("#define TASK_STACK_START  (KERN_STACK_END)\n");
  for_each (task, oil_tasks, index) {
    PRT_CFGH ("#define TASK_STK_SIZE_%d\t0x%XU\n",
              task->id, task->stacksize);
    PRT_CFGH ("#define TASK_STACK_%d\t\\\n", task->id);
    if (task->id == 1) {
      PRT_CFGH ("  (TASK_STACK_START)\n");
    } else {
      PRT_CFGH ("  (TASK_STACK_START");
      for (i = 1; i < task->id; i++) {
        PRT_CFGH (" - TASK_STK_SIZE_%d", i);
      }
      PRT_CFGH (")\n");
    }
  }

  PRT_CFGH ("#define TASK_STACK_END\t\\\n");
  PRT_CFGH ("  (TASK_STACK_START");
  for (i = 1; i <= num_tasks; i++) {
    PRT_CFGH (" - TASK_STK_SIZE_%d", i);
  }
  PRT_CFGH (")\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("#define IDLE_STACK\tTASK_STACK_END\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("/* Duration of a tick of the systemn counter in nanoseconds */\n");
  // TODO OSTICKDURATION should be configured?
  PRT_CFGH ("#define OSTICKDURATION    1000000UL\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("/* Used by Cortex M3 SYSTICK when calibration not available */\n");
  PRT_CFGH ("#define SYSTICK_RELOAD_VALUE  0x0U\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("/* Hook routine configuration */\n");
  PRT_CFGH ("#define HAS_STARTUPHOOK\t0x%XU\n",
            (oil_os->startup_hook) ? 1 : 0);
  PRT_CFGH ("#define HAS_ERRORHOOK\t0x%XU\n",
            (oil_os->error_hook) ? 1 : 0);
  PRT_CFGH ("#define HAS_SHUTDOWNHOOK\t0x%XU\n",
            (oil_os->shutdown_hook) ? 1 : 0);
  PRT_CFGH ("#define HAS_PRETASKHOOK\t0x%XU\n",
            (oil_os->pretask_hook) ? 1 : 0);
  PRT_CFGH ("#define HAS_POSTTASKHOOK\t0x%XU\n",
            (oil_os->posttask_hook) ? 1 : 0);
  PRT_CFGH ("#define USEGETSERVICEID\t0x%XU\n",
            (oil_os->use_getserviceid) ? 1 : 0);
  PRT_CFGH ("#define USEPARAMETERACCESS\t0x%XU\n",
            (oil_os->use_parameteraccess) ? 1 : 0);
  PRT_CFGH ("\n");
  PRT_CFGH ("#define USERESSCHEDULER\t0x%XU\n",
            (oil_os->use_resscheduler) ? 1 : 0);
  PRT_CFGH ("\n");
  PRT_CFGH ("#ifndef __ASSEMBLER__\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("#ifdef __ARCH_I386__\n");
  PRT_CFGH ("#include <arch/i386/types.h>\n");
  PRT_CFGH ("#elif defined __ARCH_AVR5__\n");
  PRT_CFGH ("#include <arch/avr5/types.h>\n");
  PRT_CFGH ("#elif defined __ARCH_ARMV7M__\n");
  PRT_CFGH ("#include <arch/armv7m/types.h>\n");
  PRT_CFGH ("#elif defined __ARCH_LINUX__\n");
  PRT_CFGH ("#include <arch/linux/types.h>\n");
  PRT_CFGH ("#else\n");
  PRT_CFGH ("/* Use i386 simulation as default */\n");
  PRT_CFGH ("#include <arch/i386/types.h>\n");
  PRT_CFGH ("#endif\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("/*\n");
  PRT_CFGH (" * Platform dependant type definition and types\n");
  PRT_CFGH (" * inferred from OIL.\n");
  PRT_CFGH (" */\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("/* Type for number of activations for tasks */\n");
  PRT_CFGH ("typedef %s ActivationNumType;\n",
            get_type_string (max_activation));
  PRT_CFGH ("/* Type for priority */\n");
  PRT_CFGH ("typedef %s PrioType;\n",
            get_type_string (max_prio));
  PRT_CFGH ("/* Type for event mask (8-bit minimum) */\n");
  PRT_CFGH ("typedef %s event_mask_t;\n",
            get_type_string (max_mask));
#if 0
  PRT_CFGH ("/* Type for tick. This has to match the max allowed value. */\n");
  PRT_CFGH ("typedef %s tick_t;\n",
            get_type_string (max_tick));
#endif
  PRT_CFGH ("/* Type for alarm ID. Limits max alarm supported. */\n");
  PRT_CFGH ("typedef %s alarm_t;\n",
            get_type_string (num_alarms ? num_alarms - 1 : 0));
  PRT_CFGH ("/* Type for IRQ numbers */\n");
  PRT_CFGH ("typedef %s IRQType;\n",
            get_type_string (max_irq));
  PRT_CFGH ("/* Type for IRQ priorities */\n");
  PRT_CFGH ("typedef uint8_t IRQPrioType;\n");
  PRT_CFGH ("/* Type for counters */\n");
  PRT_CFGH ("typedef %s CounterType;\n",
            get_type_string (num_counters ? num_counters - 1 : 0));
  PRT_CFGH ("/* Type for resources */\n");
  PRT_CFGH ("typedef %s ResourceType;\n",
            get_type_string (oil_os->use_resscheduler ?
                             num_resources : num_resources - 1));
  PRT_CFGH ("/* Type for schedule tables */\n");
  PRT_CFGH ("typedef %s sched_tbl_t;\n",
            get_type_string (num_sched_tbls ? num_sched_tbls - 1 : 0));
  PRT_CFGH ("\n");
  PRT_CFGH ("#endif /* __ASSEMBLER__ */\n");
  PRT_CFGH ("\n");
  PRT_CFGH ("#endif\n");
  fprintf (stdout, "Generating config/config.h             ");
  fprintf (stdout, "[" GREEN_COLOR "OK" RESET_COLOR "]\n");
#undef PRT_CFGH

  /* Generating config.c */
#define PRT_CFGC(fmt, ...) fprintf (tmp_cfgc, fmt, ##__VA_ARGS__)
  PRT_CFGC ("/**********************************************/\n");
  PRT_CFGC ("/* This file is generated by system generator */\n");
  PRT_CFGC ("/* Do not modify manually!                    */\n");
  PRT_CFGC ("/**********************************************/\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("#include <osek/osek.h>\n");
  PRT_CFGC ("#include <task.h>\n");
  PRT_CFGC ("\n");
  for_each (task, oil_tasks, index) {
    PRT_CFGC ("extern StatusType Func%s (void);\n", task->name);
  }
  PRT_CFGC ("\n");
  PRT_CFGC ("TCB tasks[] = {\n");
  PRT_CFGC ("  {IDLE_STACK, IDLE_STACK, {{0}},\n");
  PRT_CFGC ("   TASK_PREEMPTABLE | TASK_EXTENDED,\n");
  PRT_CFGC ("   0, 0, 0, INVALID_PRIO, NULL, SUSPENDED, 0, 0,\n");
  PRT_CFGC ("   (IDLE_STACK - IDLE_STK_SIZE),\n");
  PRT_CFGC ("   (code_addr_t) IdleTask");
  if (mult_activation) PRT_CFGC (", 0, 1");
  if (mult_task_per_prio) PRT_CFGC (", (TCB *) 0");
  PRT_CFGC ("},\n");
  for_each (task, oil_tasks, index) {
    PRT_CFGC ("  {TASK_STACK_%d, TASK_STACK_%d, {{0}},\n",
              task->id, task->id);
    if (task->schedule == TASK_SCHEDULE_FULL)
      PRT_CFGC ("   TASK_PREEMPTABLE");
    if (task->event) {
      if (task->schedule == TASK_SCHEDULE_FULL)
        PRT_CFGC (" | TASK_EXTENDED");
      else
        PRT_CFGC ("   TASK_EXTENDED");
    }
    if ((task->schedule == TASK_SCHEDULE_FULL) || task->event)
      PRT_CFGC (",\n");
    PRT_CFGC ("   %d, %d, %d, ", task->id,
              task->priority, task->priority);
    if (task->schedule == TASK_SCHEDULE_NON) {
      PRT_CFGC ("MAX_PRIO, ");
    } else if (task->resource == NULL) {
      PRT_CFGC ("INVALID_PRIO, ");
    } else {
      for_each (resource, task->resource, index2) {
        if (resource->property.type == RESOURCE_TYPE_INTERNAL) {
          PRT_CFGC ("%d, ", resource->priority);
          break;
        }
      }
    }
    PRT_CFGC ("NULL, SUSPENDED, 0, 0,\n");
    PRT_CFGC ("   (TASK_STACK_%d - TASK_STK_SIZE_%d),\n",
              task->id, task->id);
    PRT_CFGC ("   (code_addr_t) Func%s", task->name);
    if (mult_activation) PRT_CFGC (", %d, %d", 0, task->activation);
    if (mult_task_per_prio) PRT_CFGC (", (TCB *) 0");
    PRT_CFGC ("},\n");
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("TaskType auto_tasks[][NUM_TASKS] = {\n");
  for (i = 0; i < num_appmodes; i++) {
    bool flag = FALSE;
    PRT_CFGC ("  {0, ");
    for_each (task, oil_tasks, index) {
      if (!task->autostart) {
        PRT_CFGC ("INVALID_TASK, ");
      } else {
        for_each (appmode, task->appmode, index2) {
          if (appmode->id == i) {
            PRT_CFGC ("%d, ", task->id);
            flag = TRUE;
            break;
          }
        }
        if (!flag) PRT_CFGC ("INVALID_TASK, ");
        flag = FALSE;
      }
    }
    PRT_CFGC ("},\n");
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("prio_queue_t prio_queue[MAX_PRIO + 1] = {");
  for (i = 0; i < (max_prio + 1); i++) {
    if (mult_task_per_prio)
      PRT_CFGC ("{0, 0}, ");
    else
      PRT_CFGC ("0, ");
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("TCB * cur_task = (TCB *) 0;\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("Resource resources[] = {\n");
  if (oil_os->status == OS_STATUS_EXTENDED) {
    PRT_CFGC ("  {MAX_PRIO, FALSE, NULL},  /* RES_SCHEDULER */\n");
  } else {
    PRT_CFGC ("  {MAX_PRIO, NULL},  /* RES_SCHEDULER */\n");
  }
  for_each (resource, oil_resources, index) {
    if (resource->property.type != RESOURCE_TYPE_INTERNAL) {
      if (oil_os->status == OS_STATUS_EXTENDED) {
        PRT_CFGC ("  {%d, FALSE, NULL},\n", resource->priority);
      } else {
        PRT_CFGC ("  {%d, NULL},\n", resource->priority);
      }
    }
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("Counter counters[] = {\n");
  for_each (counter, oil_counters, index) {
    PRT_CFGC ("  {0, {0x%X, %d, %d}, 0, NULL},\n",
              counter->max_allowed_value,
              counter->ticks_per_base, counter->min_cycle);
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  for_each (alarm, oil_alarms, index) {
    char * tmp_str = NULL;
    if (alarm->action.type == ACTION_TYPE_ALARMCALLBACK) {
      tmp_str = malloc (strlen (alarm->action.alarm_callback_name) + 1);
      if (!tmp_str) {
        fprintf (stderr, "malloc failed!\n");
        exit (1);
      }
      memcpy (tmp_str, (alarm->action.alarm_callback_name + 1),
              strlen (alarm->action.alarm_callback_name) - 2);
      tmp_str[strlen (alarm->action.alarm_callback_name) - 2] = '\0';
      PRT_CFGC ("extern void %s (void);\n", tmp_str);
      free (tmp_str);
    }
  }
  PRT_CFGC ("\n");
  PRT_CFGC ("AlarmQueueType alarms[] = {\n");
  for_each (alarm, oil_alarms, index) {
    PRT_CFGC ("  {%d, &counters[%s], 0, %d, %d, ",
              alarm->id, alarm->counter->name,
              alarm->cycle_time, alarm->alarm_time);
    switch (alarm->action.type) {
      case ACTION_TYPE_ACTIVATETASK :
      {
        PRT_CFGC ("{%d, 0, NULL, 1}, ", alarm->action.task->id);
        break;
      }
      case ACTION_TYPE_SETEVENT :
      {
        PRT_CFGC ("{0, 0x%" PRIu64 ", NULL, 0}, ", alarm->action.event->mask);
        break;
      }
      case ACTION_TYPE_ALARMCALLBACK :
      {
        char * tmp_str = NULL;
        tmp_str =
          malloc (strlen (alarm->action.alarm_callback_name) + 1);
        if (!tmp_str) {
          fprintf (stderr, "malloc failed!\n");
          exit (1);
        }
        memcpy (tmp_str, (alarm->action.alarm_callback_name + 1),
                strlen (alarm->action.alarm_callback_name) - 2);
        tmp_str[strlen (alarm->action.alarm_callback_name) - 2] = '\0';
        PRT_CFGC ("{0, 0, %s, 2}, ", tmp_str);
        free (tmp_str);
        break;
      }
      default :
        fprintf (stderr, "Unknow action type for %s!\n", alarm->name);
        exit (1);
    }
    PRT_CFGC ("NULL, NULL},\n");
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("AlarmType auto_alarms[][NUM_ALARMS] = {\n");
  for (i = 0; i < num_appmodes; i++) {
    bool flag = FALSE;
    PRT_CFGC ("  {");
    for_each (alarm, oil_alarms, index) {
      if (!alarm->autostart) {
        PRT_CFGC ("INVALID_ALARM, ");
      } else {
        for_each (appmode, alarm->appmode, index2) {
          if (appmode->id == i) {
            PRT_CFGC ("%d, ", alarm->id);
            flag = TRUE;
            break;
          }
        }
        if (!flag) PRT_CFGC ("INVALID_ALARM, ");
        flag = FALSE;
      }
    }
    PRT_CFGC ("},\n");
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  PRT_CFGC ("IRQType isr1_list[] = {");
  for_each (isr, oil_isrs, index) {
    if (isr->category == 1)
      PRT_CFGC ("%d, ", isr->vector);
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("IRQType isr2_list[] = {");
  for_each (isr, oil_isrs, index) {
    if (isr->category == 2)
      PRT_CFGC ("%d, ", isr->vector);
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  if (with_sched_tbl) {
    for_each (exp, oil_expiry_points, index) {
      if (exp->num_tasks > 0) {
        PRT_CFGC ("TaskType tlist_%s[] = {", exp->name);
        for_each (task, exp->tasks, index2) {
          PRT_CFGC ("%d, ", task->id);
        }
        PRT_CFGC ("};\n");
      }
      if (exp->num_task_events > 0) {
        PRT_CFGC ("ExpiryPointEventList elist_%s[] = {", exp->name);
        for_each (task_event, exp->task_events, index2) {
          PRT_CFGC ("{%d, 0x%" PRIu64 "}, ", task_event->task->id, task_event->event->mask);
        }
        PRT_CFGC ("};\n");
      }
    }
    PRT_CFGC ("\n");
    for_each (sched_tbl, oil_sched_tbls, index) {
      PRT_CFGC ("ExpiryPointType eps%d[] = {\n", sched_tbl->id);
      for_each (exp, sched_tbl->exps, index2) {
        PRT_CFGC ("  {%d, ", exp->offset);
        if (exp->num_tasks > 0)
          PRT_CFGC ("tlist_%s, ", exp->name);
        else
          PRT_CFGC ("NULL, ");
        PRT_CFGC ("%d, ", exp->num_tasks);
        if (exp->num_task_events > 0)
          PRT_CFGC ("elist_%s, ", exp->name);
        else
          PRT_CFGC ("NULL, ");
        PRT_CFGC ("%d, ", exp->num_task_events);
        if (with_sched_tbl_sync)
          PRT_CFGC ("%d, %d", exp->max_shorten, exp->max_lengthen);
        PRT_CFGC ("},\n");
      }
      PRT_CFGC ("};\n");
      PRT_CFGC ("\n");
    }
    PRT_CFGC ("ScheduleTableStructType schedtbls[] = {\n");
    for_each (sched_tbl, oil_sched_tbls, index) {
      PRT_CFGC ("  {%d, &counters[%s], SCHEDULETABLE_STOPPED, ", sched_tbl->id,
                sched_tbl->counter->name);
      switch (sched_tbl->sync_strategy) {
        case SCHEDTBL_SYNC_NONE :
          PRT_CFGC ("0");
          break;
        case SCHEDTBL_SYNC_EXPLICIT :
          PRT_CFGC ("SCHEDULETABLE_SYNC_EXPLICIT");
          break;
        case SCHEDTBL_SYNC_IMPLICIT :
          PRT_CFGC ("SCHEDULETABLE_SYNC_IMPLICIT");
          break;
        default :
          /* Should not reach here */
          PRT_CFGC ("0");
          break;
      }
      if (sched_tbl->repeating)
        PRT_CFGC ("|SCHEDULETABLE_REPEATING, ");
      else
        PRT_CFGC (", ");
      PRT_CFGC ("0, %d, eps%d, INVALID_EP, %d, ", sched_tbl->delay, sched_tbl->id,
                sched_tbl->num_exps);
      if (with_sched_tbl_sync)
        PRT_CFGC ("%d, %d, 0, ", sched_tbl->precision, sched_tbl->duration);
      if (mult_schedtbl_per_cntr)
        PRT_CFGC ("NULL, NULL, ");
      PRT_CFGC ("NULL, NULL");
      PRT_CFGC ("},\n");
    }
    PRT_CFGC ("};\n");
    PRT_CFGC ("\n");
    PRT_CFGC ("ScheduleTableAutoStartType auto_sched_tbls[] = {\n");
    if (num_autostart_sched_tbls > 0) {
      for_each (sched_tbl, oil_sched_tbls, index) {
        if (sched_tbl->autostart) {
          for_each (appmode, sched_tbl->appmode, index2) {
            PRT_CFGC ("  {%s, ", sched_tbl->name);
            switch (sched_tbl->start_mode) {
              case SCHEDTBL_STARTMODE_ABSOLUTE :
                PRT_CFGC ("SCHEDTBL_STARTMODE_ABSOLUTE, ");
                break;
              case SCHEDTBL_STARTMODE_RELATIVE :
                PRT_CFGC ("SCHEDTBL_STARTMODE_RELATIVE, ");
                break;
              case SCHEDTBL_STARTMODE_SYNCHRON :
                PRT_CFGC ("SCHEDTBL_STARTMODE_SYNCHRON, ");
                break;
              default :
                PRT_CFGC ("-1, ");
                break;
            }
            PRT_CFGC ("%d, %s},\n", sched_tbl->start_value, appmode->name);
          }
        }
      }
    } else {
      PRT_CFGC ("  {INVALID_SCHEDULETABLE, 0, 0, 0}");
    }
    PRT_CFGC ("};\n");
    PRT_CFGC ("\n");
  }
  for_each (driver, oil_drivers, index) {
    PRT_CFGC ("extern void %s_init (void);\n", driver->file_name);
  }
  PRT_CFGC ("\n");
  PRT_CFGC ("void (* drivers[]) (void) = {\n");
  for_each (driver, oil_drivers, index) {
    PRT_CFGC ("  %s_init,\n", driver->file_name);
  }
  PRT_CFGC ("};\n");
  PRT_CFGC ("\n");
  fprintf (stdout, "Generating config/config.c             ");
  fprintf (stdout, "[" GREEN_COLOR "OK" RESET_COLOR "]\n");
#undef PRT_CFGC

  /* Generating apps/config.mk */
  build_apps_config_obj_list ();
#define PRT_APPSMK(fmt, ...) fprintf (tmp_appsmk, fmt, ##__VA_ARGS__)
  PRT_APPSMK ("# This file is generated by system generator\n");
  for (i = 0; i < MAX_APPS_OBJ; i++) {
    if (apps_obj_list[i]) {
      PRT_APPSMK ("OBJ += %s\n", apps_obj_list[i]);
    }
  }
  PRT_APPSMK ("\n");
  fprintf (stdout, "Generating apps/config.mk              ");
  fprintf (stdout, "[" GREEN_COLOR "OK" RESET_COLOR "]\n");
#undef PRT_APPSMK

  /*
   * File generation is now complete. Let's see whether
   * file copy is needed.
   */
  if (!tflag) {
    /* Rewind all files for copying */
    rewind (tmp_cfgmk);
    rewind (tmp_cfgh);
    rewind (tmp_cfgc);
    rewind (tmp_appsmk);

    /* No -t option. Files should be copied. */
    copy_config_files (tmp_cfgmk, tmp_cfgh, tmp_cfgc, tmp_appsmk);
  }
}

static void
print_help (const char * cmd)
{
  printf ("Usage: %s [Options] CONFIG_FILE\n", cmd);
  printf ("Options:\n");
  printf ("\t-i <INCLUDE_PATH> \tConfig file include path\n");
  printf ("\t\t\t\tMultiple paths separated by \":\"\n");
  printf ("\t-s <SDVOS_BASE> \tPath to SDVOS source\n");
  printf ("\t-r \t\t\tGenerating resource statistics\n");
  printf ("\t-t \t\t\tTest/Verify\n");
  printf ("\t-d \t\t\tDump all objects\n");
  printf ("\t-b \t\t\tBackup old configuration files\n");
  printf ("\t-m \t\t\tBuild system image\n");
  printf ("\t-h \t\t\tPrint this help message\n");
  printf ("\t-v \t\t\tVersion\n");
}

static bool
check_sdvos_root (const char * path)
{
  struct stat st;
  DIR * dir = NULL;
  struct dirent * ent = NULL;
  int i = 0, matches = 0;
#define NUM_FEATURES    4
  const char * feature_entries[NUM_FEATURES] = {
    "Makefile",
    "board",
    "include",
    "arch"
  };

  /* SDVOS root has to be a directory */
  if (stat (path, &st) < 0) {
    fprintf (stderr, "stat on %s failed!\n", path);
    exit (1);
  }

  if (!S_ISDIR(st.st_mode)) {
    fprintf (stderr, "%s is not a directory!\n", path);
    return FALSE;
  }

  if (!(dir = opendir (path))) {
    fprintf (stderr, "Cannot open %s!\n", path);
    return FALSE;
  }

  /* Path is probably correct if we can match the four feature entries */
  while ((ent = readdir (dir))) {
    for (i = 0; i < NUM_FEATURES; i++) {
      if (strncmp (feature_entries[i], ent->d_name,
                   strlen (feature_entries[i])) == 0) {
        matches++;
        continue;
      }
    }
  }

  if (matches != 4) {
    fprintf (stderr, "%s does not seem to be the SDVOS root!\n", path);
    return FALSE;
  }

  return TRUE;
}

static int
process_include_path (const char * ipath)
{
  int i = 0, j = 0, k = 0;

  /* Need to append '/' if necessary */
  include_path_list[k] = malloc (strlen (ipath) + 2);
  if (!include_path_list[k]) {
    fprintf (stderr, "malloc failed in process_include_path!\n");
    return -1;
  }

  for (i = 0; i < (strlen (ipath) + 1); i++) {
    if (ipath[i] == ':') {
      if (include_path_list[k][j - 1] != '/') {
        include_path_list[k][j] = '/';
        include_path_list[k][j + 1] = '\0';
      } else {
        include_path_list[k][j] = '\0';
      }
      include_path_list[++k] = malloc (strlen (ipath) + 2);
      if (!include_path_list[k]) {
        fprintf (stderr, "malloc failed in process_include_path!\n");
        return -1;
      }
      j = 0;
    } else if (ipath[i] == '\0') {
      if (include_path_list[k][j - 1] != '/') {
        include_path_list[k][j] = '/';
        include_path_list[k][j + 1] = '\0';
      } else {
        include_path_list[k][j] = '\0';
      }
    } else {
      include_path_list[k][j++] = ipath[i];
    }
  }

#if 0
  printf ("Include Path List:\n");
  for (i = 0; i < MAX_INCLUDE_PATH; i++) {
    if (include_path_list[i]) {
      printf ("\t%s\n", include_path_list[i]);
    }
  }
#endif

  return 0;
}

int
main (int argc, char * argv[])
{
  int yyres = 0, c = 0;
  opterr = 0;
  char * cwd = NULL;

  /* Parse argument options */
  while ((c = getopt (argc, argv, "i:s:rtdbmhv")) != -1) {
    switch (c) {
      case 'i':
        include_path = malloc (strlen (optarg) + 1);
        memcpy (include_path, optarg, strlen (optarg) + 1);
        if (process_include_path (include_path) < 0) {
          fprintf (stderr, "Include path processing failed!\n");
          exit (1);
        }
        break;
      case 's':
        if (access (optarg, F_OK) < 0) {
          fprintf (stderr, "%s does not exist!\n", optarg);
          exit (1);
        }
        if (!check_sdvos_root (optarg)) exit (1);
        sdvos_root = malloc (strlen (optarg) + 1);
        memcpy (sdvos_root, optarg, strlen (optarg) + 1);
        break;
      case 'r':
        rflag = TRUE;
        break;
      case 't':
        tflag = TRUE;
        break;
      case 'd':
        dflag = TRUE;
        break;
      case 'b':
        bflag = TRUE;
        break;
      case 'm':
        mflag = TRUE;
        break;
      case 'h':
        print_help (argv[0]);
        exit (0);
      case 'v':
        printf ("SDVOS System Generator Version: %s\n", version_string);
        printf ("Author: Ye Li (liye@sdvos.org)\n");
        exit (0);
      case '?':
        if (optopt == 'i' || optopt == 's') {
          fprintf (stderr, "Option -%c requires argument!\n", optopt);
        } else {
          fprintf (stderr, "Unknow option: -%c!\n", optopt);
        }
        printf ("\n");
        print_help (argv[0]);
        exit (1);
      default:
        print_help (argv[0]);
        exit (1);
    }
  }

  if (optind >= argc) {
    fprintf (stderr, "OIL config file not specified!\n");
    printf ("\n");
    print_help (argv[0]);
    exit (1);
  }

  /* Open OIL configuration file for parser */
  if (!newfile (strdup (argv[optind]), 0)) {
    fprintf (stderr, "Failed to open OIL configration file!\n");
    exit (1);
  }

  yyres = yyparse ();

  if (yyres) {
    fprintf (stderr, "OIL file parsing failed!\n");
    exit (1);
  }

  printf ("OIL Version: %s\n", oil_version);

  update_oil_objects ();

  if (dflag) dump_all_objects ();

  if (rflag) show_stats ();

  if (!sdvos_root) {
    if (!rflag && !dflag) {
      fprintf (stderr, "SDVOS source root directory not specified!\n\n");
      print_help (argv[0]);
    }
  } else {
    generate_code ();

    if (mflag && !tflag) {
      cwd = getcwd (NULL, 0);
      fprintf (stdout, "Building system image...\n");

      if (chdir (sdvos_root) < 0) {
        fprintf (stderr, "Cannot change directory to %s!\n", sdvos_root);
        exit (1);
      }

      if (system ("make") == -1) {
        fprintf (stderr, "Failed executing make!\n");
      }

      /* Change back to working directory */
      if (chdir (cwd) < 0) {
        fprintf (stderr, "Cannot change directory to %s!\n", cwd);
        exit (1);
      }
    }
  }

  exit (0);
}

/* vi: set et ai sw=2 sts=2: */
