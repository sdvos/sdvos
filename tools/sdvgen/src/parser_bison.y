%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <list.h>
#include <oil_object.h>
#include <parser.h>
#include <debug.h>
#include "parser_bison.h"

extern void yyerror (const char *s, ...);
extern void sderror (const char *s, int line, ...);
%}

/* %define parse.error verbose */

%union {
  double d;
  char * s;
  int i;
  unsigned int ui;
  oil_attribute_value_t * v;
  oil_object_list_t * l;
  oil_param_t * p;
}

/* Tokens */
%token <i> OS
%token <i> TASK
%token <i> COUNTER
%token <i> ALARM
%token <i> RESOURCE
%token <i> EVENT
%token <i> ISR
%token <i> MESSAGE
%token <i> COM
%token <i> NM
%token <i> APPMODE
%token <i> IPDU
%token <i> CPU
%token <i> SCHEDULETABLE
%token <i> OS_TYPE
%token <i> TASK_TYPE
%token <i> COUNTER_TYPE
%token <i> ALARM_TYPE
%token <i> RESOURCE_TYPE
%token <i> EVENT_TYPE
%token <i> ISR_TYPE
%token <i> MESSAGE_TYPE
%token <i> COM_TYPE
%token <i> NM_TYPE
%token <i> APPMODE_TYPE
%token <i> IPDU_TYPE
%token <s> OIL_VERSION
%token <i> IMPLEMENTATION
%token <i> TYPE_UINT32
%token <i> TYPE_INT32
%token <i> TYPE_UINT64
%token <i> TYPE_FLOAT
%token <i> TYPE_ENUM
%token <i> TYPE_BOOLEAN
%token <i> WITH_AUTO
%token <i> NO_DEFAULT
%token <i> AUTO
%token <i> TYPE_TRUE
%token <i> TYPE_FALSE
%token <i> TYPE_STRING
%token <s> STRING_CONST
%token <s> ID
%token <i> EXTENDED
%token <i> STANDARD
%token <i> NON
%token <i> FULL
%token <i> INTERNAL
%token <i> LINKED
%token <i> ALARMCALLBACK
%token <i> ACTIVATETASK
%token <i> SETEVENT
%token <i> IMPLICIT
%token <i> EXPLICIT
%token <i> NONE
%token <i> ABSOLUTE
%token <i> RELATIVE
%token <i> SYNCHRON
%token <i> EXPIRYPOINT
%token <i> EXPIRYPOINT_TYPE

%token <v> HEX_INT
%token <v> OCT_INT
%token <v> DEC_INT

%token <i> ATTR_STATUS
%token <i> ATTR_STARTUPHOOK
%token <i> ATTR_ERRORHOOK
%token <i> ATTR_SHUTDOWNHOOK
%token <i> ATTR_PRETASKHOOK
%token <i> ATTR_POSTTASKHOOK
%token <i> ATTR_USEGETSERVICEID
%token <i> ATTR_USEPARAMETERACCESS
%token <i> ATTR_USERESSCHEDULER
%token <i> ATTR_DEBUGLEVEL
%token <i> ATTR_BOARD
%token <i> ATTR_DRIVER
%token <i> ATTR_PRIORITY
%token <i> ATTR_SCHEDULE
%token <i> ATTR_ACTIVATION
%token <i> ATTR_STACKSIZE
%token <i> ATTR_AUTOSTART
%token <i> ATTR_MASK
%token <i> ATTR_DEFAULT
%token <i> ATTR_MINCYCLE
%token <i> ATTR_MAXALLOWEDVALUE
%token <i> ATTR_TICKSPERBASE
%token <i> ATTR_RESOURCEPROPERTY
%token <i> ATTR_LINKEDRESOURCE
%token <i> ATTR_ALARMTIME
%token <i> ATTR_CYCLETIME
%token <i> ATTR_ACTION
%token <i> ATTR_ALARMCALLBACKNAME
%token <i> ATTR_CATEGORY
%token <i> ATTR_VECTOR
%token <i> ATTR_DURATION
%token <i> ATTR_REPEATING
%token <i> ATTR_SYNCSTRATEGY
%token <i> ATTR_PRECISION
%token <i> ATTR_STARTMODE
%token <i> ATTR_STARTVALUE
%token <i> ATTR_OFFSET
%token <i> ATTR_MAXLENGTHEN
%token <i> ATTR_MAXSHORTEN
%token <i> ATTR_SCHEDTBLACTION

%type <s> oil_version
%type <i> attr_name
%type <i> obj_name
%type <i> oil_obj
%type <i> action_value
%type <v> attr_value
%type <p> param
%type <l> param_list

%start oil_file

%%

/* Implementation definition is ignored. */
oil_file : oil_version app_def {}
;

oil_version : OIL_VERSION '=' STRING_CONST ';' {
  oil_version = $3;
}
;

app_def : CPU ID '{' obj_def_list '}' desc ';' {
  cpu_name = $2;
}
;

obj_name : OS ID {
  oil_attribute_value_t * attr = malloc (sizeof (oil_attribute_value_t));
  if (!oil_os) {
    oil_os = new_oil_object (oil_os_object_t);
    /* Default values */
    oil_os->status = OS_STATUS_STANDARD;
    oil_os->startup_hook = FALSE;
    oil_os->error_hook = FALSE;
    oil_os->shutdown_hook = FALSE;
    oil_os->pretask_hook = FALSE;
    oil_os->posttask_hook = FALSE;
    oil_os->use_getserviceid = FALSE;
    oil_os->use_parameteraccess = FALSE;
    oil_os->use_resscheduler = TRUE;
    oil_os->debuglevel = 2;
    oil_os->board = NULL;
  } else {
    if (strncmp ($2, oil_os->name, strlen (oil_os->name))) {
      yyerror ("Multiple OS object detected!");
      YYABORT;
    }
  }
  cur_oil_object.object_type = OS;
  cur_oil_object.object = oil_os;
  $$ = $1;
  attr->v.s = $2;
  attr->value_type = VALUE_TYPE_STRING;
  if (object_add_param (&cur_oil_object, USR_ATTR_NAME, attr) < 0) {
    YYABORT;
  }
}
        | TASK ID {
  cur_oil_object.object_type = TASK;
  cur_oil_object.object = get_task_object ($2);
  $$ = $1;
}
        | COUNTER ID {
  cur_oil_object.object_type = COUNTER;
  cur_oil_object.object = get_counter_object ($2);
  $$ = $1;
}
        | ALARM ID {
  cur_oil_object.object_type = ALARM;
  cur_oil_object.object = get_alarm_object ($2);
  $$ = $1;
}
        | RESOURCE ID {
  cur_oil_object.object_type = RESOURCE;
  cur_oil_object.object = get_resource_object ($2);
  $$ = $1;
}
        | EVENT ID {
  cur_oil_object.object_type = EVENT;
  cur_oil_object.object = get_event_object ($2);
  $$ = $1;
}
        | ISR ID {
  cur_oil_object.object_type = ISR;
  cur_oil_object.object = get_isr_object ($2);
  $$ = $1;
}
        | MESSAGE ID {
  yyerror ("Message object is not supported!");
  YYABORT;
}
        | COM ID {
  yyerror ("COM object is not supported!");
  YYABORT;
}
        | NM ID {
  yyerror ("NM object is not supported!");
  YYABORT;
}
        | APPMODE ID {
  cur_oil_object.object_type = APPMODE;
  cur_oil_object.object = get_appmode_object ($2);
  $$ = $1;
}
        | IPDU ID {
  yyerror ("IPDU object is not supported!");
  YYABORT;
}
        | SCHEDULETABLE ID {
  cur_oil_object.object_type = SCHEDULETABLE;
  cur_oil_object.object = get_sched_tbl_object ($2);
  $$ = $1;
}
        | EXPIRYPOINT ID {
  cur_oil_object.object_type = EXPIRYPOINT;
  cur_oil_object.object = get_expiry_point_object ($2);
  $$ = $1;
}
;

oil_obj : OS { $$ = $1; }
        | TASK { $$ = $1; }
        | COUNTER { $$ = $1; }
        | ALARM { $$ = $1; }
        | RESOURCE { $$ = $1; }
        | EVENT { $$ = $1; }
        | ISR { $$ = $1; }
        | MESSAGE { $$ = $1; }
        | COM { $$ = $1; }
        | NM { $$ = $1; }
        | APPMODE { $$ = $1; }
        | IPDU { $$ = $1; }
        | SCHEDULETABLE { $$ = $1; }
        | EXPIRYPOINT { $$ = $1; }
;

attr_name : oil_obj { $$ = $1; }
          | ATTR_STATUS { $$ = $1; }
          | ATTR_STARTUPHOOK { $$ = $1; }
          | ATTR_ERRORHOOK { $$ = $1; }
          | ATTR_SHUTDOWNHOOK { $$ = $1; }
          | ATTR_PRETASKHOOK { $$ = $1; }
          | ATTR_POSTTASKHOOK { $$ = $1; }
          | ATTR_USEGETSERVICEID { $$ = $1; }
          | ATTR_USEPARAMETERACCESS { $$ = $1; }
          | ATTR_USERESSCHEDULER { $$ = $1; }
          | ATTR_DEBUGLEVEL { $$ = $1; }
          | ATTR_BOARD { $$ = $1; }
          | ATTR_DRIVER { $$ = $1; }
          | ATTR_PRIORITY { $$ = $1; }
          | ATTR_SCHEDULE { $$ = $1; }
          | ATTR_ACTIVATION { $$ = $1; }
          | ATTR_STACKSIZE { $$ = $1; }
          | ATTR_AUTOSTART { $$ = $1; }
          | ATTR_MASK { $$ = $1; }
          | ATTR_DEFAULT { $$ = $1; }
          | ATTR_MINCYCLE { $$ = $1; }
          | ATTR_MAXALLOWEDVALUE { $$ = $1; }
          | ATTR_TICKSPERBASE { $$ = $1; }
          | ATTR_RESOURCEPROPERTY { $$ = $1; }
          | ATTR_LINKEDRESOURCE { $$ = $1; }
          | ATTR_ALARMTIME { $$ = $1; }
          | ATTR_CYCLETIME { $$ = $1; }
          | ATTR_ACTION { $$ = $1; }
          | ATTR_ALARMCALLBACKNAME { $$ = $1; }
          | ATTR_CATEGORY { $$ = $1; }
          | ATTR_VECTOR { $$ = $1; }
          | ATTR_DURATION { $$ = $1; }
          | ATTR_REPEATING { $$ = $1; }
          | ATTR_SYNCSTRATEGY { $$ = $1; }
          | ATTR_PRECISION { $$ = $1; }
          | ATTR_STARTMODE { $$ = $1; }
          | ATTR_STARTVALUE { $$ = $1; }
          | ATTR_OFFSET { $$ = $1; }
          | ATTR_MAXLENGTHEN { $$ = $1; }
          | ATTR_MAXSHORTEN { $$ = $1; }
          | ATTR_SCHEDTBLACTION { $$ = $1; }
;

action_value : ALARMCALLBACK { $$ = $1; }
             | ACTIVATETASK { $$ = $1; }
             | SETEVENT { $$ = $1; }
;

attr_value : EXTENDED {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_OS_STATUS;
  $$->lineno = yylineno;
}
           | STANDARD {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_OS_STATUS;
  $$->lineno = yylineno;
}
           | TYPE_TRUE {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.b = TRUE;
  $$->value_type = VALUE_TYPE_BOOL;
  $$->lineno = yylineno;
}
           | TYPE_TRUE '{' param_list '}' {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.b = TRUE;
  $$->value_type = VALUE_TYPE_BOOL_LIST;
  $$->list = $3;
  $$->lineno = yylineno;
}
           | TYPE_FALSE {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.b = FALSE;
  $$->value_type = VALUE_TYPE_BOOL;
  $$->lineno = yylineno;
}
           | NON {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_OS_SCHEDULE;
  $$->lineno = yylineno;
}
           | FULL {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_OS_SCHEDULE;
  $$->lineno = yylineno;
}
           | INTERNAL {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_RES_RESPROP;
  $$->lineno = yylineno;
}
           | LINKED '{' param_list '}' {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_RES_RESPROP;
  $$->list = $3;
  $$->lineno = yylineno;
}
           | action_value '{' param_list '}' {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_ACTION_LIST;
  $$->list = $3;
  $$->lineno = yylineno;
}
           | EXPLICIT {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_STRATEGY;
  $$->lineno = yylineno;
}
           | IMPLICIT {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_STRATEGY;
  $$->lineno = yylineno;
}
           | NONE {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_STRATEGY;
  $$->lineno = yylineno;
}
           | ABSOLUTE {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_START_MODE;
  $$->lineno = yylineno;
}
           | RELATIVE {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_START_MODE;
  $$->lineno = yylineno;
}
           | SYNCHRON {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_START_MODE;
  $$->lineno = yylineno;
}
           | STRING_CONST {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s = $1;
  $$->value_type = VALUE_TYPE_STRING_CONST;
  $$->lineno = yylineno;
}
           | ID {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s = $1;
  $$->value_type = VALUE_TYPE_STRING;
  $$->lineno = yylineno;
}
           | AUTO {
  $$ = malloc (sizeof (oil_attribute_value_t));
  $$->v.s4b = $1;
  $$->value_type = VALUE_TYPE_AUTO;
  $$->lineno = yylineno;
}
           | HEX_INT {
  $$ = $1;
  $$->lineno = yylineno;
}
           | OCT_INT {
  $$ = $1;
  $$->lineno = yylineno;
}
           | DEC_INT {
  $$ = $1;
  $$->lineno = yylineno;
}
;

param : attr_name '=' attr_value desc ';' {
  $$ = new_oil_object (oil_param_t);
  $$->attribute = $1;
  $$->value = $3;
}
;

param_list : /* Empty list */ { $$ = NULL; }
           | param param_list {
  oil_object_list_t * plist = NULL;
  if ($2 == NULL) {
    $$ = object_list_add (&plist, $1);
  } else {
    $$ = object_list_add (&$2, $1);
  }
}
;

obj_def : obj_name desc ';' {}
        | obj_name '{' param_list '}' desc ';' {
  oil_param_t * param = NULL;
  oil_object_list_t * index = NULL;
  for_each (param, $3, index) {
    if (object_add_param (&cur_oil_object, param->attribute,
        param->value) < 0) {
      YYABORT;
    }
  }
}
;

obj_def_list : /* Empty definition */ {}
             | obj_def obj_def_list {
}
;

desc : /* Empty Description */ {}
     | ':' STRING_CONST { /* Do nothing */ }
;

