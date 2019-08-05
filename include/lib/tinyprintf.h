/*
File: tinyprintf.h

Copyright (C) 2004  Kustaa Nyholm

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef __TFP_PRINTF__
#define __TFP_PRINTF__

#include <stdarg.h>

/* Global configuration */

/* Set this to 0 if you do not want to provide tfp_printf */
#ifndef TINYPRINTF_DEFINE_TFP_PRINTF
# define TINYPRINTF_DEFINE_TFP_PRINTF 1
#endif

/* Set this to 0 if you do not want to provide
   tfp_sprintf/snprintf/vsprintf/vsnprintf */
#ifndef TINYPRINTF_DEFINE_TFP_SPRINTF
# define TINYPRINTF_DEFINE_TFP_SPRINTF 1
#endif

/* Set this to 0 if you do not want tfp_printf and
   tfp_{vsn,sn,vs,s}printf to be also available as
   printf/{vsn,sn,vs,s}printf */
#ifndef TINYPRINTF_OVERRIDE_LIBC
# define TINYPRINTF_OVERRIDE_LIBC 1
#endif

/* Optional external types dependencies */

#if TINYPRINTF_DEFINE_TFP_SPRINTF
# include <sys/types.h>  /* size_t */
#endif

/* Declarations */

#ifdef __GNUC__
# define _TFP_SPECIFY_PRINTF_FMT(fmt_idx,arg1_idx) \
    __attribute__((format (printf, fmt_idx, arg1_idx)))
#else
# define _TFP_SPECIFY_PRINTF_FMT(fmt_idx,arg1_idx)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

typedef void (*putcf) (void *, char);

/*
   'tfp_format' really is the central function for all tinyprintf. For
   each output character after formatting, the 'putf' callback is
   called with 2 args:
     - an arbitrary void* 'putp' param defined by the user and
       passed unmodified from 'tfp_format',
     - the character.
   The 'tfp_printf' and 'tfp_sprintf' functions simply define their own
   callback and pass to it the right 'putp' it is expecting.
*/
void tfp_format(void *putp, putcf putf, const char *fmt, va_list va);

#if TINYPRINTF_DEFINE_TFP_SPRINTF
int tfp_vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int tfp_snprintf(char *str, size_t size, const char *fmt, ...) \
     _TFP_SPECIFY_PRINTF_FMT(3, 4);
int tfp_vsprintf(char *str, const char *fmt, va_list ap);
int tfp_sprintf(char *str, const char *fmt, ...) \
    _TFP_SPECIFY_PRINTF_FMT(2, 3);
# if TINYPRINTF_OVERRIDE_LIBC
#  define vsnprintf tfp_vsnprintf
#  define snprintf tfp_snprintf
#  define vsprintf tfp_vsprintf
#  define sprintf tfp_sprintf
# endif
#endif

#if TINYPRINTF_DEFINE_TFP_PRINTF
void init_printf(void *putp, putcf putf);
void tfp_printf(char *fmt, ...) _TFP_SPECIFY_PRINTF_FMT(1, 2);
# if TINYPRINTF_OVERRIDE_LIBC
#  define printf tfp_printf
# endif
#endif

#ifdef  __cplusplus
}
#endif

#endif
