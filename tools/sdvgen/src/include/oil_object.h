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

#ifndef _OIL_OBJECT_H_
#define _OIL_OBJECT_H_

#include <stdint.h>
#include <stdbool.h>
#include <list.h>
#include <limits.h>

#define MAX_PRIORITY            (255)
#define MAX_ACTIVATION          (255)
#define MAX_STACKSIZE           (UINT_MAX)
#define MAX_MASK                (0xFF)
#define MAX_MINCYCLE            (0xFFFFFFFF)
#define MAX_MAXALLOWEDVALUE     (0xFFFFFFFF)
#define MAX_TICKSPERBASE        (0xFFFFFFFF)
#define MAX_ALARMTIME           (0xFFFFFFFF)
#define MAX_CYCLETIME           (0xFFFFFFFF)
#define MAX_VECTOR              (UINT_MAX)
#define MAX_DURATION            (0xFFFF)

#define ERR_OBJECT              (-1)
#define ERR_ATTRIBUTE           (-2)

/* Attribute Value Types */
#define VALUE_TYPE_OS_STATUS    (0x00)
#define VALUE_TYPE_BOOL         (0x01)
#define VALUE_TYPE_STRING       (0x02)
#define VALUE_TYPE_INT          (0x03)
#define VALUE_TYPE_OS_SCHEDULE  (0x04)
#define VALUE_TYPE_BOOL_LIST    (0x05)
#define VALUE_TYPE_RES_RESPROP  (0x05)
#define VALUE_TYPE_ACTION_LIST  (0x06)
#define VALUE_TYPE_STRING_CONST (0x07)
#define VALUE_TYPE_AUTO         (0x08)
#define VALUE_TYPE_STRATEGY     (0x09)
#define VALUE_TYPE_START_MODE   (0x0A)

#define USR_ATTR_NAME           (0x00)

#ifndef TRUE
#define TRUE    (0x1)
#endif

#ifndef FALSE
#define FALSE    (0x0)
#endif

typedef enum os_status {
  OS_STATUS_STANDARD = 0x0,
  OS_STATUS_EXTENDED
} os_status_t;

typedef struct oil_os_object {
  char * name;
  /* Default is OS_STATUS_STANDARD */
  os_status_t status;
  /* Default is FALSE */
  bool startup_hook;
  /* Default is FALSE */
  bool error_hook;
  /* Default is FALSE */
  bool shutdown_hook;
  /* Default is FALSE */
  bool pretask_hook;
  /* Default is FALSE */
  bool posttask_hook;
  /* Default is FALSE */
  bool use_getserviceid;
  /* Default is FALSE */
  bool use_parameteraccess;
  /* Default is TRUE */
  bool use_resscheduler;
  /* Default is 2 */
  uint32_t debuglevel;
  /* Default is NULL */
  char * board;
  /* Default is FALSE */
  bool shell;
} oil_os_object_t;

typedef struct oil_appmode_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  /* Default is FALSE */
  bool default_appmode;
} oil_appmode_object_t;

typedef enum task_schedule {
  TASK_SCHEDULE_NON = 0x0,
  TASK_SCHEDULE_FULL
} task_schedule_t;

typedef struct oil_task_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  /* Default is -1 */
  uint32_t priority;
  uint32_t activation;
  task_schedule_t schedule;
  uint32_t stacksize;
  /* Default is FALSE */
  bool autostart;
  oil_object_list_t * appmode;
  oil_object_list_t * resource;
  oil_object_list_t * event;
} oil_task_object_t;

typedef struct oil_counter_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  uint32_t min_cycle;
  uint32_t max_allowed_value;
  uint32_t ticks_per_base;
} oil_counter_object_t;

typedef struct oil_event_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  /* Default is -1 */
  uint64_t mask;
} oil_event_object_t;

struct oil_resource_object;

typedef enum resource_type {
  RESOURCE_TYPE_STANDARD = 0x0,
  RESOURCE_TYPE_LINKED,
  RESOURCE_TYPE_INTERNAL
} resource_type_t;

typedef struct resource_property {
  /* Default is -1 */
  resource_type_t type;
  struct oil_resource_object * linked_resource;
} resource_property_t;

typedef struct oil_resource_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  uint32_t priority;
  resource_property_t property;
} oil_resource_object_t;

typedef enum action_type {
  ACTION_TYPE_ACTIVATETASK = 0x0,
  ACTION_TYPE_SETEVENT,
  ACTION_TYPE_ALARMCALLBACK
} action_type_t;

typedef struct action {
  /* Default is -1 */
  action_type_t type;
  oil_task_object_t * task;
  oil_event_object_t * event;
  char * alarm_callback_name;
} action_t;

typedef struct oil_alarm_object {
  uint32_t id;
  /* Always set when object is created */
  char * name;
  oil_counter_object_t * counter;
  action_t action;
  /* Default is FALSE */
  bool autostart;
  oil_object_list_t * appmode;
  /* Default is -1 */
  uint32_t alarm_time;
  /* Default is -1 */
  uint32_t cycle_time;
} oil_alarm_object_t;

typedef struct oil_isr_object {
  /* Default is -1 */
  uint32_t vector;
  /* Always set when object is created */
  char * name;
  uint32_t category;
  oil_object_list_t * resource;
  oil_object_list_t * message;
} oil_isr_object_t;

typedef enum sched_tbl_sync_strategy {
  SCHEDTBL_SYNC_NONE = 0x0,
  SCHEDTBL_SYNC_IMPLICIT,
  SCHEDTBL_SYNC_EXPLICIT
} sched_tbl_sync_strategy_t;

typedef enum sched_tbl_start_mode {
  SCHEDTBL_STARTMODE_NONE = 0x0,
  SCHEDTBL_STARTMODE_ABSOLUTE,
  SCHEDTBL_STARTMODE_RELATIVE,
  SCHEDTBL_STARTMODE_SYNCHRON
} sched_tbl_start_mode_t;

typedef struct oil_expiry_point_event_object {
  oil_task_object_t * task;
  oil_event_object_t * event;
} oil_expiry_point_event_object_t;

typedef struct oil_expiry_point_object {
  /* Always set when object is created */
  char *name;
  /* Default is -1 */
  uint32_t offset;
  /* Default is -1 */
  uint32_t max_lengthen;
  /* Default is -1 */
  uint32_t max_shorten;
  oil_object_list_t * tasks;
  /* Default is 0 */
  uint32_t num_tasks;
  oil_object_list_t * task_events;
  /* Default is 0 */
  uint32_t num_task_events;
} oil_expiry_point_object_t;

typedef struct oil_sched_tbl_object {
  uint32_t id;
  /* Always set when object is created */
  char *name;
  oil_counter_object_t * counter;
  /* Default is FALSE */
  bool repeating;
  /* Default is -1 */
  uint32_t duration;
  uint32_t delay;
  /* Default is -1 */
  uint32_t precision;
  /* Default is SCHEDTBL_SYNC_NONE */
  sched_tbl_sync_strategy_t sync_strategy;
  /* Defautl is SCHEDTBL_STARTMODE_NONE */
  sched_tbl_start_mode_t start_mode;
  /* Default is FALSE */
  bool autostart;
  /* Default is -1 */
  uint32_t start_value;
  oil_object_list_t * appmode;
  oil_object_list_t * exps;
  uint32_t num_exps;
} oil_sched_tbl_object_t;

typedef struct oil_driver_object {
  uint32_t id;
  char * name;
  char * file_name;
} oil_driver_object_t;

typedef struct generic_oil_object {
  int object_type;
  void * object;
} generic_oil_object_t;

typedef struct oil_attribute_value {
  int value_type;
  int lineno;
  union _v {
    uint32_t u4b;
    uint64_t u8b;
    int32_t s4b;
    int64_t s8b;
    float fp;
    double dfp;
    bool b;
    char * s;
  } v;
  oil_object_list_t * list;
} oil_attribute_value_t;

typedef struct oil_param {
  int attribute;
  oil_attribute_value_t * value;
} oil_param_t;

#define new_oil_object(t) ((t*) malloc (sizeof (t)))

extern oil_object_list_t * oil_tasks;
extern oil_os_object_t * oil_os;
extern oil_object_list_t * oil_appmodes;
extern oil_object_list_t * oil_events;
extern oil_object_list_t * oil_resources;
extern generic_oil_object_t cur_oil_object;
extern char * oil_version;
extern char * cpu_name;
extern int object_add_param (generic_oil_object_t * object,
                             int attribute,
                             oil_attribute_value_t * value);
extern oil_task_object_t * get_task_object (const char * name);
extern oil_event_object_t * get_event_object (const char * name);
extern oil_resource_object_t * get_resource_object (const char * name);
extern oil_appmode_object_t * get_appmode_object (const char * name);
extern oil_counter_object_t * get_counter_object (const char * name);
extern oil_alarm_object_t * get_alarm_object (const char * name);
extern oil_isr_object_t * get_isr_object (const char * name);
extern oil_sched_tbl_object_t * get_sched_tbl_object (const char * name);
extern oil_expiry_point_object_t * get_expiry_point_object (const char * name);
extern oil_driver_object_t * get_driver_object (const char * name);
#endif

/* vi: set et ai sw=2 sts=2: */
