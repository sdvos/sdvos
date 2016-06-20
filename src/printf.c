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
 * @file   src/printf.c
 * @author Ye Li (liye@sdvos.org)
 * @brief  Simplified version of printf and sprintf.
 */
#include <sdvos_printf.h>

/* NUM_DIGITS - 1 is the max number of digits we can print */
/* 12 is enough for 32-bit numbers */
#define NUM_DIGITS  12

#ifndef NULL
#define NULL  ((void *) 0)
#endif

/** User provided putchar function */
static void (*usr_putchar) (char ch) = NULL;

/**
 * @brief Putchar used by printf
 *
 * This is required by vprint. Generic signature.
 *
 * @param[in] buf
 *   Not used. For type conformance only.
 * @param[in] ch
 *   Character to be printed
 */
static void
stdout_putchar (char ** buf, char ch)
{
  usr_putchar (ch);
}

/** Lower case digit array for itoa */
static const char ldigitarray[] = "0123456789abcdef";
/** Upper case digit array for itoa */
static const char udigitarray[] = "0123456789ABCDEF";

/**
 * @brief Customized version of uitoa
 *
 * This is a customized version of uitoa.
 *
 * @param[in] value
 *   Integer to be converted to string
 * @param[in] radix
 *   Radix of the integer provided
 * @param[in] upper
 *   Use upper case for hex digits if true
 * @param[out] buf
 *   Buffer for the result string
 */
static void
sdvos_uitoa (unsigned int value, unsigned int radix,
             unsigned char upper, char * buf)
{
  unsigned char ndigits = 1;
  unsigned int tmp_value = value / radix;

  while (tmp_value != 0) {
    tmp_value /= radix;
    ndigits++;
  }

  if (ndigits >= NUM_DIGITS) {
    /* Too many digits to handle */
    ndigits = NUM_DIGITS - 1;
  }

  buf[ndigits] = '\0';

  for (; ndigits > 0; --ndigits, value /= radix) {
    buf[ndigits - 1] = (upper) ? udigitarray[value % radix] :
                                 ldigitarray[value % radix];
  }
}

/**
 * @brief Customized version of itoa
 *
 * This is a customized version of itoa. It uses uito
 * internally. Radix will be set to 10 since only decimal
 * and hex are supported.
 *
 * @param[in] value
 *   Integer to be converted to string
 * @param[out] buf
 *   Buffer for the result string
 */
static void
sdvos_itoa (int value, char * buf)
{
  if (value < 0) {
    value = -value;
    *buf++ = '-';
  }
  sdvos_uitoa (value, 10, 0, buf);
}

/**
 * @brief Convert character to digit
 *
 * This function converts a character to a digit.
 *
 * @param[in] ch
 *   Character to be converted
 *
 * @return
 *   The digit
 */
static unsigned char
to_digit (char ch)
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
 * @brief Customized version of atoi
 *
 * This is a customized version of atoi. The major
 * difference here is after calling the function, buf
 * will already be advanced to next character to be
 * processed.
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
sdvos_atoi (const char ** buf, int radix, int * value)
{
  const char * b = *buf;
  unsigned char digit = 0;
  char ch = *b++;
  int v = 0;

  while ((digit = to_digit (ch)) >= 0) {
    if (digit > radix) break;
    v = v * radix + digit;
    ch = *b++;
  }
  *buf = --b;
  *value = v;

  return;
}

/**
 * @brief Helper function to print a buffer with padding
 *
 * @param[in] putc_fun
 *   Putchar function
 * @param[in] out_buf
 *   Data to be passed to putc_fun
 * @param[in] buf
 *   Buffer containing string to be printed
 * @param[in] len
 *   Length of content to be printed. If smaller than
 *   buffer length, the whole buffer will be printed.
 *   Otherwise, leading extra space will be filled with
 *   ' ' character.
 */
static void
put_buf (void (* putc_fun) (char **, char), char ** out_buf,
         char * buf, int len)
{
  char pad = ' ';
  char ch = 0;
  char * b = buf;

  while (*b++ && len > 0) len--;
  while (len-- > 0) putc_fun (out_buf, pad);
  while ((ch = *buf++)) putc_fun (out_buf, ch);
}

/**
 * @brief Internal putchar used by sprintf
 *
 * @param[in] buf
 *   Reference to the buffer to be printed to
 * @param[in] ch
 *   Character to be printed
 */
static void
vputc (char ** buf, char ch)
{
  *((*buf)++) = ch;
}

/**
 * @brief Internal printf used by both printf and sprintf
 *
 * This is the internal printf used to implement both
 * printf and sprintf.
 *
 * @param[in] putc_fun
 *   Putchar function
 * @param[in] out_buf
 *   Reference to the output buffer
 * @param[in] fmt
 *   Printf format string
 * @param[in] va
 *   Variable argument list
 */
static void
sdvos_vprintf (void (* putc_fun) (char **, char), char ** out_buf,
               const char * fmt, va_list va)
{
  char buf[NUM_DIGITS];
  char ch = 0;
  int width = 0;

  while ((ch = *(fmt++))) {
    if (ch != '%') {
      putc_fun (out_buf, ch);
    } else {
      ch = *(fmt++);

      if (ch >= '0' && ch <= '9') {
        /* Only fldwidth is supported */
        fmt--;
        sdvos_atoi (&fmt, 10, &width);
        ch = *(fmt++);
      }

      switch (ch) {
        case 'u' :
          sdvos_uitoa (va_arg (va, unsigned int), 10, 0, buf);
          put_buf (putc_fun, out_buf, buf, width);
          break;
        case 'd' :
          sdvos_itoa (va_arg (va, int), buf);
          put_buf (putc_fun, out_buf, buf, width);
          break;
        case 'x' :
        case 'X' :
          sdvos_uitoa (va_arg (va, unsigned int), 16,
                       ((ch == 'X') ? 1 : 0), buf);
          put_buf (putc_fun, out_buf, buf, width);
          break;
        case 'c' :
          putc_fun (out_buf, (char) (va_arg (va, int)));
          break;
        case 's' :
          put_buf (putc_fun, out_buf,
                   va_arg (va, char *), width);
          break;
        case '%' :
          putc_fun (out_buf, ch);
        default:
          break;
      }
    }
  }
}

void
sdvos_printf(const char *fmt, ...)
{
  va_list va;

  /* printf not initialized */
  if (!usr_putchar) return;

  va_start (va, fmt);
  sdvos_vprintf (stdout_putchar, NULL, fmt, va);
  va_end (va);
}

void
sdvos_sprintf(char * buf, const char *fmt, ...)
{
  va_list va;
  if (!buf) return;
  va_start (va, fmt);
  sdvos_vprintf (vputc, &buf, fmt, va);
  vputc (&buf, 0);
  va_end (va);
}

void
sdvos_init_printf (void (*putc) (char))
{
  usr_putchar = putc;
}

/* vi: set et ai sw=2 sts=2: */
