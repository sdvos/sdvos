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
 * @file   src/shell.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Simple Embedded Shell
 */
#include <osek/osek.h>
#include <debug.h>
#include <sdvos.h>
#include <board.h>
#include <sdvos_printf.h>

DeclareTask (shell);

/** Backspace ASCII character */
#define ASCII_BS        8
/** Line Feed ASCII character */
#define ASCII_LF       10
/** Carriage Return ASCII character */
#define ASCII_CR       13
/** ASCII Escape */
#define ASCII_ESC      27
/** ASCII Del */
#define ASCII_DEL     127
/** Newline character */
#define NLCHAR   ASCII_CR

/** Max command length */
#define MAX_CMD_LEN    64
/** Max command argument */
#define MAX_ARGC        3

/**
 * @def MOVE_CURSOR_LEFT
 * @brief Macro to move cursor to the left by n lines
 *
 * @param[in] n
 *   Number of lines
 */
#define MOVE_CURSOR_LEFT(n)              \
  sdvos_printf ("%c[%dD", ASCII_ESC, n)

/**
 * @def MOVE_CURSOR_RIGHT
 * @brief Macro to move cursor to the right by n lines
 *
 * @param[in] n
 *   Number of lines
 */
#define MOVE_CURSOR_RIGHT(n)             \
  sdvos_printf ("%c[%dC", ASCII_ESC, n)

/**
 * @def MOVE_CURSOR_HOME
 * @brief Macro to move cursor to the home position
 */
#define MOVE_CURSOR_HOME()               \
  sdvos_printf ("%c[H", ASCII_ESC)

/**
 * @def CLEAR_SCREEN
 * @brief Macro to clear the screen
 */
#define CLEAR_SCREEN()                   \
  sdvos_printf ("%c[2J", ASCII_ESC)

/* Stringification macros */
#define XSTR(s) STR(s)
#define STR(s) #s

/* get/putchar from UART driver */
extern int uart_getchar (void);
extern int uart_putchar (char);

/** Shell command number type */
typedef enum cmd_t {
  CMD_CLEAR = 0,
  CMD_EXIT,
  CMD_HELP,
  CMD_TICK,
  CMD_TASK,
  CMD_UNAME,
} CmdType;

/** Shell command strings */
static char * cmd_names[] = {
  "clear",
  "exit",
  "help",
  "tick",
  "task",
  "uname"
};

/** Command string array */
static char command_string[MAX_CMD_LEN];

/**
 * @brief Convert character to digit
 *
 * This internal function converts a character to a digit.
 *
 * @param[in] ch
 *   Character to be converted
 *
 * @return
 *   The digit
 */
static unsigned char
shell_to_digit (char ch)
{
  if (ch >= '0' && ch <= '9')
    return ch - '0';
  else if (ch >= 'a' && ch <= 'f')
    return ch- 'a' + 10;
  else if (ch >= 'A' && ch <= 'F')
    return ch- 'A' + 10;
  else return -1;
}

/**
 * @brief Simple internal version of atoi
 *
 * @param[in] buf
 *   Reference to the buffer containing the string to be
 *   converted to an integer
 * @param[in] radix
 *   Radix of the result
 * @param[out] value
 *   The result integer
 */
static void
shell_atoi (const char * buf, int radix, int * value)
{
  unsigned char digit = 0;
  char ch = *buf++;
  int v = 0;

  while ((digit = shell_to_digit (ch)) >= 0) {
    if (digit > radix) break;
    v = v * radix + digit;
    ch = *buf++;
  }
  *value = v;

  return;
}

/**
 * @brief Internal string comparison function
 *
 * @param[in] s1
 *   String to be compared with
 * @param[in] s2
 *   String to be compared against
 * @param[in] n
 *   Max number of characters to be compared
 */
static int
strncmp (const char * s1, const char * s2, unsigned int n)
{
  for (; n > 0; s1++, s2++, --n) {
    if (*s1 != *s2) {
      return (((*(unsigned char *) s1) <
              (*(unsigned char *) s2)) ? -1 : +1);
    } else if (*s1 == '\0') {
      return 0;
    }
  }

  return 0;
}

/**
 * @brief Simple getline function used by shell
 *
 * This function captures a whole user input line ended
 * with NLCHAR. Trailing spaces and BS/DEL are also
 * handled inside the function.
 */
static char *
uart_getline ()
{
  int c = 0, i = 0;
  int index = 0;

  for (;;) {
    c = uart_getchar ();

    if (c == NLCHAR) {
      command_string[index] = '\0';
      /* Remove trailing spaces */
      for (i = index - 1; i >= 0; i--) {
        if (command_string[i] == ' ')
          command_string[i] = '\0';
        else
          break;
      }
      uart_putchar ('\n');
      return command_string;
    } else if (c == ASCII_BS || c == ASCII_DEL) {
      if (index > 0) {
        index--;
        MOVE_CURSOR_LEFT (1);
        uart_putchar (' ');
        MOVE_CURSOR_LEFT (1);
      }
      continue;
    } else if (c < 32) {
      /* Ignore all the other non-printing characters */
      continue;
    }

    command_string[index++] = (char) c;
    uart_putchar ((char) c);

    /* Did we reach command buffer limit? */
    if (index >= (MAX_CMD_LEN - 1)) {
      command_string[MAX_CMD_LEN - 1] = '\0';
      return command_string;
    }
  }
}

/**
 * @brief Internal function that prints shell banner
 */
static void
print_shell_banner ()
{
  sdvos_printf ("+----------------------------------------------+\n");
  sdvos_printf ("| Standard Dependable Vehicle Operating System |\n");
  sdvos_printf ("|  Copyright (C) 2015 Ye Li (liye@sdvos.org)   |\n");
  sdvos_printf ("|           Simple Embedded Shell              |\n");
  sdvos_printf ("+----------------------------------------------+\n");
  sdvos_printf ("\n");
}

/**
 * @brief Input line parser
 *
 * This function parses the input line from uart_getline.
 * The command number and arguments will be returned to
 * caller for further processing.
 *
 * @param[in] input
 *   Input line
 * @param[out] command
 *   Command number parsed from input
 * @param[out] argv
 *   Arguments array (allocated by caller)
 * @param[out] argc
 *   Number of arguments
 */
static void
parse_input_line (char * input, CmdType * command,
                  char * (*argv)[], int * argc)
{
  int i = 0;
  int num_commands = sizeof (cmd_names) / sizeof (cmd_names[0]);

  *argc = 0;
  *command = -1;

  /* Ignore leading spaces */
  while ((*input) == ' ') {
    input++;
  }

  for (i = 0; i < MAX_CMD_LEN; i++) {
    if (input[i] == '\0') {
      break;
    } else if (input[i] == ' ') {
      if (((*argc) < MAX_ARGC) && (input[i + 1] != ' ')) {
        (*argv)[(*argc)++] = input + i + 1;
      }
      input[i] = '\0';
    }
  }

  for (i = 0; i < num_commands; i++) {
    if (strncmp (cmd_names[i], input,
        sizeof (cmd_names[i])) == 0) {
      *command = i;
      break;
    }
  }
}

/* The shell task */
TASK (shell)
{
  char * input = NULL;
  CmdType command = -1;
  int num_commands = sizeof (cmd_names) / sizeof (cmd_names[0]);
  int i = 0;
  char * argv[MAX_ARGC];
  int argc = 0;

  sdvos_printf ("\n");
  print_shell_banner ();

  for (;;) {
    sdvos_printf ("Sesh# ");
    input = uart_getline ();

    if (input[0] == '\0') {
      continue;
    }

    /* Parse the input to get command and args */
    parse_input_line (input, &command, &argv, &argc);

    /* Process command */
    switch (command) {
      case CMD_CLEAR :
      {
        CLEAR_SCREEN ();
        MOVE_CURSOR_HOME ();
        continue;
      }
      case CMD_EXIT :
      {
        TerminateTask ();
      }
      case CMD_TASK :
      {
        int tid = (int) INVALID_TASK;
        if (argc != 1) {
          sdvos_printf ("No task id!\n");
        } else {
          shell_atoi (argv[0], 10, &tid);
          ActivateTask ((TaskType) tid);
        }
        continue;
      }
      case CMD_TICK :
      {
        TickType v;
        GetCounterValue (SYS_COUNTER, &v);
        sdvos_printf ("%d\n", v);
        continue;
      }
      case CMD_UNAME :
      {
        sdvos_printf ("SDVOS %d.%d.%d " XSTR(BOARDNAME) "\n",
                      GET_KERNEL_VERSION (CURRENT_KERNEL_VERSION),
                      GET_MAJOR_REVISION (CURRENT_KERNEL_VERSION),
                      GET_MINOR_REVISION (CURRENT_KERNEL_VERSION));
        continue;
      }
      case CMD_HELP :
      {
        for (i = 0; i < num_commands; i++) {
          sdvos_printf ("%s\t", cmd_names[i]);
          if (((i + 1) % 4) == 0) {
            sdvos_printf ("\n");
          }
        }

        if ((num_commands % 4) != 0) {
          sdvos_printf ("\n");
        }
        continue;
      }
      default :
        sdvos_printf ("Unknown command: %s\n", input);
        continue;
    }
  }

  TerminateTask ();

  return E_OK;
}

/* vi: set et ai sw=2 sts=2: */
