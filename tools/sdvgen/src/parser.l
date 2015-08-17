%option noyywrap yylineno nounput

%x COMMENT
%x INCL
%x IMPL

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <parser.h>
#include <list.h>
#include <unistd.h>
#include <oil_object.h>
#include <errno.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "parser_bison.h"

extern void yyerror (const char *s, ...);
extern void sderror (const char *s, int line, ...);
static int brace_count = 0;
static int incl_flag = 0;

struct buf_stack {
  struct buf_stack * prev;
  YY_BUFFER_STATE state;
  int lineno;
  char * file_name;
  FILE * file;
} * cur_buf = NULL;

FILE *
newfile (char * name, int flag)
{
  FILE * file = NULL;
  struct buf_stack *buf = malloc (sizeof (struct buf_stack));

  if (name[0] == '/') {
    /* Path is absolute. No need to check include path. */
    flag = 0;
  }

  file = fopen (name, "r");

  if ((!file) && (flag == 1)) {
    int i = 0, s = 0;
    char * tmp_name = NULL;

    for (i = 0; i < MAX_INCLUDE_PATH; i++) {
      if (include_path_list[i]) {
        s = strlen (include_path_list[i]) + strlen (name) + 1;
        tmp_name = malloc (s);
        if (!tmp_name) {
          fprintf (stderr, "malloc failed in newfile()!\n");
          return NULL;
        }
        snprintf (tmp_name, s, "%s%s", include_path_list[i], name);
        /* If file is found, stop searching */
        if ((file = fopen (tmp_name, "r"))) {
          free (tmp_name);
          break;
        }
        free (tmp_name);
      } else {
        break;
      }
    }

    return file;
  }

  if (!file) {
    fprintf (stderr, "Cannot open %s!\n", name);
    return NULL;
  }

  if (!buf) {
    fprintf (stderr, "Cannot open allocate buffer!\n");
    return NULL;
  }

  /* remember state */
  if (cur_buf) cur_buf->lineno = yylineno;
  buf->prev = cur_buf;

  /* set up current entry */
  buf->state = yy_create_buffer (file, YY_BUF_SIZE);

  buf->file = file;
  buf->file_name = name;
  yy_switch_to_buffer (buf->state);
  cur_buf = buf;
  yylineno = 1;
  cur_file = name;

  return file;
}

int
popfile (void)
{
  struct buf_stack * buf = cur_buf;
  struct buf_stack * prev_buf;

  if(!buf) return 0;

  fclose (buf->file);
  yy_delete_buffer (buf->state);

  /* Switch back to previous buffer */
  prev_buf = buf->prev;
  free (buf);

  if(!prev_buf) return 0;

  yy_switch_to_buffer (prev_buf->state);
  cur_buf = prev_buf;
  /* Restore line number */
  yylineno = cur_buf->lineno;
  cur_file = cur_buf->file_name;

  return 1;
}

%}

 /* Universal Character Name */
UCN     (\\u[0-9a-fA-F]{4}|\\U[0-9a-fA-F]{8})

 /* Float Exponent */
EXP     ([Ee][-+]?[0-9]+)

 /* Integer Length */
ILEN    ([Uu](L|l|LL|ll)?|(L|l|LL|ll)[Uu]?)

%%

 /* Comment */
"/*"                   { BEGIN(COMMENT); }
<COMMENT>"*/"          { BEGIN(INITIAL); }
<COMMENT>([^*]|\n)+|.
<COMMENT><<EOF>>       { fprintf (stderr, "%s:%d: Unterminated Comment\n",
                         cur_file, yylineno); return 0; }

 /* Implementation definition (Ignore) */
 /* Definitely not the smartest thing to do... */
IMPLEMENTATION         { BEGIN(IMPL); }
<IMPL>"{"              { brace_count++; }
<IMPL>"};" |
<IMPL>"}," |
<IMPL>"}"              { brace_count--;
                         if (brace_count == 0) {
                           BEGIN(INITIAL);
                         } else if (brace_count < 0) {
                           fprintf (stderr, "%s:%d: Illegal implementation definition!\n", cur_file, yylineno);
                           return 0;
                         } }
<IMPL>([^{}]|\n)+|.
<IMPL><<EOF>>          { fprintf (stderr, "%s:%d: Implementation definition is incomplete!\n", cur_file, yylineno);
                         return 0; }

 /* C++ Comment */
"//".*\n

"="  |
"{"  |
"}"  |
"+"  |
"-"  |
":"  |
"["  |
"]"  |
","  |
"\"" |
";"                { return yytext[0]; }

OS                 { yylval.i = OS;
                     return OS; }
TASK               { yylval.i = TASK;
                     return TASK; }
COUNTER            { yylval.i = COUNTER;
                     return COUNTER; }
ALARM              { yylval.i = ALARM;
                     return ALARM; }
RESOURCE           { yylval.i = RESOURCE;
                     return RESOURCE; }
EVENT              { yylval.i = EVENT;
                     return EVENT; }
ISR                { yylval.i = ISR;
                     return ISR; }
MESSAGE            { yylval.i = MESSAGE;
                     return MESSAGE; }
COM                { yylval.i = COM;
                     return COM; }
NM                 { yylval.i = NM;
                     return NM; }
APPMODE            { yylval.i = APPMODE;
                     return APPMODE; }
IPDU               { yylval.i = IPDU;
                     return IPDU; }
CPU                { yylval.i = CPU;
                     return CPU; }
SCHEDULETABLE      { yylval.i = SCHEDULETABLE;
                     return SCHEDULETABLE; }
OS_TYPE            { yylval.i = OS_TYPE;
                     return OS_TYPE; }
TASK_TYPE          { yylval.i = TASK_TYPE;
                     return TASK_TYPE; }
COUNTER_TYPE       { yylval.i = COUNTER_TYPE;
                     return COUNTER_TYPE; }
ALARM_TYPE         { yylval.i = ALARM_TYPE;
                     return ALARM_TYPE; }
RESOURCE_TYPE      { yylval.i = RESOURCE_TYPE;
                     return RESOURCE_TYPE; }
EVENT_TYPE         { yylval.i = EVENT_TYPE;
                     return EVENT_TYPE; }
ISR_TYPE           { yylval.i = ISR_TYPE;
                     return ISR_TYPE; }
MESSAGE_TYPE       { yylval.i = MESSAGE_TYPE;
                     return MESSAGE_TYPE; }
COM_TYPE           { yylval.i = COM_TYPE;
                     return COM_TYPE; }
NM_TYPE            { yylval.i = NM_TYPE;
                     return NM_TYPE; }
APPMODE_TYPE       { yylval.i = APPMODE_TYPE;
                     return APPMODE_TYPE; }
IPDU_TYPE          { yylval.i = IPDU_TYPE;
                     return IPDU_TYPE; }
OIL_VERSION        { yylval.i = OIL_VERSION;
                     return OIL_VERSION; }
UINT32             { yylval.i = TYPE_UINT32;
                     return TYPE_UINT32; }
INT32              { yylval.i = TYPE_INT32;
                     return TYPE_INT32; }
UINT64             { yylval.i = TYPE_UINT64;
                     return TYPE_UINT64; }
FLOAT              { yylval.i = TYPE_FLOAT;
                     return TYPE_FLOAT; }
ENUM               { yylval.i = TYPE_ENUM;
                     return TYPE_ENUM; }
STRING             { yylval.i = TYPE_STRING;
                     return TYPE_STRING; }
BOOLEAN            { yylval.i = TYPE_BOOLEAN;
                     return TYPE_BOOLEAN; }
WITH_AUTO          { yylval.i = WITH_AUTO;
                     return WITH_AUTO; }
NO_DEFAULT         { yylval.i = NO_DEFAULT;
                     return NO_DEFAULT; }
AUTO               { yylval.i = AUTO;
                     return AUTO; }
TRUE               { yylval.i = TYPE_TRUE;
                     return TYPE_TRUE; }
FALSE              { yylval.i = TYPE_FALSE;
                     return TYPE_FALSE; }
EXTENDED           { yylval.i = EXTENDED;
                     return EXTENDED; }
STANDARD           { yylval.i = STANDARD;
                     return STANDARD; }
NON                { yylval.i = NON;
                     return NON;}
FULL               { yylval.i = FULL;
                     return FULL;}
INTERNAL           { yylval.i = INTERNAL;
                     return INTERNAL;}
LINKED             { yylval.i = LINKED;
                     return LINKED;}
ALARMCALLBACK      { yylval.i = ALARMCALLBACK;
                     return ALARMCALLBACK; }
ACTIVATETASK       { yylval.i = ACTIVATETASK;
                     return ACTIVATETASK; }
SETEVENT           { yylval.i = SETEVENT;
                     return SETEVENT; }
EXPLICIT           { yylval.i = EXPLICIT;
                     return EXPLICIT; }
IMPLICIT           { yylval.i = IMPLICIT;
                     return IMPLICIT; }
NONE               { yylval.i = NONE;
                     return NONE; }
ABSOLUTE           { yylval.i = ABSOLUTE;
                     return ABSOLUTE; }
RELATIVE           { yylval.i = RELATIVE;
                     return RELATIVE; }
SYNCHRON           { yylval.i = SYNCHRON;
                     return SYNCHRON; }
EXPIRYPOINT        { yylval.i = EXPIRYPOINT;
                     return EXPIRYPOINT; }
EXPIRYPOINT_TYPE   { yylval.i = EXPIRYPOINT_TYPE;
                     return EXPIRYPOINT_TYPE; }

 /* Attributes */
 /* OS Object */
STATUS             { yylval.i = ATTR_STATUS;
                     return ATTR_STATUS; }
STARTUPHOOK        { yylval.i = ATTR_STARTUPHOOK;
                     return ATTR_STARTUPHOOK; }
ERRORHOOK          { yylval.i = ATTR_ERRORHOOK;
                     return ATTR_ERRORHOOK; }
SHUTDOWNHOOK       { yylval.i = ATTR_SHUTDOWNHOOK;
                     return ATTR_SHUTDOWNHOOK; }
PRETASKHOOK        { yylval.i = ATTR_PRETASKHOOK;
                     return ATTR_PRETASKHOOK; }
POSTTASKHOOK       { yylval.i = ATTR_POSTTASKHOOK;
                     return ATTR_POSTTASKHOOK; }
USEGETSERVICEID    { yylval.i = ATTR_USEGETSERVICEID;
                     return ATTR_USEGETSERVICEID; }
USEPARAMETERACCESS { yylval.i = ATTR_USEPARAMETERACCESS;
                     return ATTR_USEPARAMETERACCESS; }
USERESSCHEDULER    { yylval.i = ATTR_USERESSCHEDULER;
                     return ATTR_USERESSCHEDULER; }
DEBUGLEVEL         { yylval.i = ATTR_DEBUGLEVEL;
                     return ATTR_DEBUGLEVEL; }
BOARD              { yylval.i = ATTR_BOARD;
                     return ATTR_BOARD; }
DRIVER             { yylval.i = ATTR_DRIVER;
                     return ATTR_DRIVER; }
 /* Task Object */
PRIORITY           { yylval.i = ATTR_PRIORITY;
                     return ATTR_PRIORITY; }
SCHEDULE           { yylval.i = ATTR_SCHEDULE;
                     return ATTR_SCHEDULE; }
ACTIVATION         { yylval.i = ATTR_ACTIVATION;
                     return ATTR_ACTIVATION; }
STACKSIZE          { yylval.i = ATTR_STACKSIZE;
                     return ATTR_STACKSIZE; }
AUTOSTART          { yylval.i = ATTR_AUTOSTART;
                     return ATTR_AUTOSTART; }

 /* Event Object */
MASK               { yylval.i = ATTR_MASK;
                     return ATTR_MASK; }

 /* AppMode Object */
DEFAULT            { yylval.i = ATTR_DEFAULT;
                     return ATTR_DEFAULT; }
 /* Counter Object */
MINCYCLE           { yylval.i = ATTR_MINCYCLE;
                     return ATTR_MINCYCLE; }
MAXALLOWEDVALUE    { yylval.i = ATTR_MAXALLOWEDVALUE;
                     return ATTR_MAXALLOWEDVALUE; }
TICKSPERBASE       { yylval.i = ATTR_TICKSPERBASE;
                     return ATTR_TICKSPERBASE; }

 /* Resource Object */
RESOURCEPROPERTY   { yylval.i = ATTR_RESOURCEPROPERTY;
                     return ATTR_RESOURCEPROPERTY; }
LINKEDRESOURCE     { yylval.i = ATTR_LINKEDRESOURCE;
                     return ATTR_LINKEDRESOURCE; }

 /* Alarm Object */
ALARMTIME          { yylval.i = ATTR_ALARMTIME;
                     return ATTR_ALARMTIME; }
CYCLETIME          { yylval.i = ATTR_CYCLETIME;
                     return ATTR_CYCLETIME; }
ACTION             { yylval.i = ATTR_ACTION;
                     return ATTR_ACTION; }
ALARMCALLBACKNAME  { yylval.i = ATTR_ALARMCALLBACKNAME;
                     return ATTR_ALARMCALLBACKNAME; }

 /* ISR Object */
CATEGORY           { yylval.i = ATTR_CATEGORY;
                     return ATTR_CATEGORY; }
VECTOR             { yylval.i = ATTR_VECTOR;
                     return ATTR_VECTOR; }

 /* SCHEDULETABLE Object */
DURATION           { yylval.i = ATTR_DURATION;
                     return ATTR_DURATION; }
REPEATING          { yylval.i = ATTR_REPEATING;
                     return ATTR_REPEATING; }
SYNCSTRATEGY       { yylval.i = ATTR_SYNCSTRATEGY;
                     return ATTR_SYNCSTRATEGY; }
PRECISION          { yylval.i = ATTR_PRECISION;
                     return ATTR_PRECISION; }
STARTMODE          { yylval.i = ATTR_STARTMODE;
                     return ATTR_STARTMODE; }
STARTVALUE         { yylval.i = ATTR_STARTVALUE;
                     return ATTR_STARTVALUE; }
OFFSET             { yylval.i = ATTR_OFFSET;
                     return ATTR_OFFSET; }
MAXLENGTHEN        { yylval.i = ATTR_MAXLENGTHEN;
                     return ATTR_MAXLENGTHEN; }
MAXSHORTEN         { yylval.i = ATTR_MAXSHORTEN;
                     return ATTR_MAXSHORTEN; }
SCHEDTBLACTION     { yylval.i = ATTR_SCHEDTBLACTION;
                     return ATTR_SCHEDTBLACTION; }

[ \t]          { /* Ignore White Spaces */ }
\n             { /* Ignore New Line */ }

 /* string literal */
L?\"([^"\\]|\\['"?\\abfnrtv]|\\[0-7]{1,3}|\\[Xx][0-9a-fA-F]+|{UCN})*\" {
  yylval.s = strdup ((const char *) yytext);
  return STRING_CONST;
}

 /* integers */
0[0-7]*{ILEN}? {
  oil_attribute_value_t * attr = malloc (sizeof (oil_attribute_value_t));
  attr->v.s8b = strtoll ((const char *) yytext, NULL, 0);
  if ((errno == ERANGE && (attr->v.s8b == LLONG_MAX ||
      attr->v.s8b == LLONG_MIN)) || (errno != 0 && attr->v.s8b == 0)) {
    yyerror ("Invalid Integer Value!");
    yyterminate ();
  }
  attr->value_type = VALUE_TYPE_INT;
  yylval.v = attr;
  return OCT_INT;
}
[1-9][0-9]*{ILEN}? {
  oil_attribute_value_t * attr = malloc (sizeof (oil_attribute_value_t));
  errno = 0;
  attr->v.s8b = strtoll ((const char *) yytext, NULL, 0);
  if ((errno == ERANGE && (attr->v.s8b == LLONG_MAX ||
      attr->v.s8b == LLONG_MIN)) || (errno != 0 && attr->v.s8b == 0)) {
    yyerror ("Invalid Integer Value!");
    yyterminate ();
  }
  attr->value_type = VALUE_TYPE_INT;
  yylval.v = attr;
  return DEC_INT;
}
0[Xx][0-9a-fA-F]+{ILEN}? {
  oil_attribute_value_t * attr = malloc (sizeof (oil_attribute_value_t));
  attr->v.s8b = strtoll ((const char *) yytext, NULL, 16);
  if ((errno == ERANGE && (attr->v.s8b == LLONG_MAX ||
      attr->v.s8b == LLONG_MIN)) || (errno != 0 && attr->v.s8b == 0)) {
    yyerror ("Invalid Integer Value!");
    yyterminate ();
  }
  attr->value_type = VALUE_TYPE_INT;
  yylval.v = attr;
  return HEX_INT;
}

 /* Identifiers */
([_a-zA-Z]|{UCN})([_a-zA-Z0-9]|{UCN})*  {
  yylval.s = strdup ((const char *) yytext);
  return ID;
}

 /* Recognize an include */
^"#"[ \t]*include[ \t]*[\"]     { incl_flag = 0; BEGIN INCL; }
^"#"[ \t]*include[ \t]*[<]      { incl_flag = 1; BEGIN INCL; }
<INCL>[^>\"]+     {
  { int c; while ((c = input()) && c != '\n'); }
  if (!newfile (strdup (yytext), incl_flag)) {
    fprintf (stderr, "Failed to open %s!\n", yytext);
    yyterminate ();
  }
  BEGIN INITIAL;
}
<INCL>.|\n        {
  fprintf (stderr, "%s:%d bad include line\n",
           cur_file, yylineno);
  BEGIN INITIAL;
}
<<EOF>>           { if(!popfile()) yyterminate(); }

.      { fprintf (stderr, "%s:%d: Parser Error!\n",
                  cur_file, yylineno); }

%%
/* vi: set et ai sw=2 sts=2: */
