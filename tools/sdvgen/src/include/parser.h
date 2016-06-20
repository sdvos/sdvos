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

#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <oil_object.h>

#define MAX_INCLUDE_PATH    32
#define MAX_APPS_OBJ       128

struct buf_stack;
extern struct buf_stack * cur_buf;

extern char * cur_file;
extern FILE * newfile (char *, int);
extern char * strdup (const char *);
extern int popfile (void);
extern int fileno(FILE *stream);
extern int yylineno;
extern int yyparse (void);
extern int yylex (void);
extern FILE * yyin;
extern char * apps_obj_list[];
extern bool build_apps_config_obj_list (void);
extern char * path_alloc (size_t *);
extern int copy_config_files (FILE *, FILE *, FILE *, FILE *);

extern uint32_t num_tasks;
extern uint32_t num_events;
extern uint32_t num_resources;
extern uint32_t num_appmodes;
extern uint32_t num_counters;
extern uint32_t num_alarms;
extern uint32_t num_sched_tbls;
extern uint32_t num_isrs;
extern uint32_t num_isr1s;
extern uint32_t num_isr2s;
extern uint32_t num_drivers;
extern uint32_t max_prio;
extern uint32_t max_activation;
extern uint64_t max_mask;
extern uint64_t max_tick;
extern uint32_t max_irq;
extern uint64_t masks;
extern bool rflag, tflag, dflag, bflag;
extern bool mult_task_per_prio, mult_activation;
extern bool with_sched_tbl, with_sched_tbl_sync;
extern bool mult_schedtbl_per_cntr;
extern char * include_path;
extern char * include_path_list[];
extern char * sdvos_root;
extern oil_object_list_t * oil_tasks;
extern oil_os_object_t * oil_os;
extern oil_object_list_t * oil_appmodes;
extern oil_object_list_t * oil_events;
extern oil_object_list_t * oil_resources;
extern oil_object_list_t * oil_counters;
extern oil_object_list_t * oil_alarms;
extern oil_object_list_t * oil_isrs;
extern oil_object_list_t * oil_sched_tbls;
extern oil_object_list_t * oil_expiry_points;
extern oil_object_list_t * oil_drivers;
extern char * oil_version;
extern char * cpu_name;

#endif

/* vi: set et ai sw=2 sts=2: */
