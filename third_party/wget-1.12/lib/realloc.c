/* realloc() function that is glibc compatible.

   Copyright (C) 1997, 2003, 2004, 2006, 2007, 2009 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* written by Jim Meyering and Bruno Haible */

#include <config.h>

/* Only the AC_FUNC_REALLOC macro defines 'realloc' already in config.h.  */
#ifdef realloc
# define NEED_REALLOC_GNU 1
#endif

/* Infer the properties of the system's malloc function.
   Only the AC_FUNC_MALLOC macro defines 'malloc' already in config.h.  */
#if GNULIB_MALLOC_GNU && !defined malloc
# define SYSTEM_MALLOC_GLIBC_COMPATIBLE 1
#endif

/* Below we want to call the system's malloc and realloc.
   Undefine the symbols here so that including <stdlib.h> provides a
   declaration of malloc(), not of rpl_malloc(), and likewise for realloc.  */
#undef malloc
#undef realloc

/* Specification.  */
#include <stdlib.h>

#include <errno.h>

/* Below we want to call the system's malloc and realloc.
   Undefine the symbols, if they were defined by gnulib's <stdlib.h>
   replacement.  */
#undef malloc
#undef realloc

/* Change the size of an allocated block of memory P to N bytes,
   with error checking.  If N is zero, change it to 1.  If P is NULL,
   use malloc.  */

void *
rpl_realloc (void *p, size_t n)
{
  void *result;

#if NEED_REALLOC_GNU
  if (n == 0)
    {
      n = 1;

      /* In theory realloc might fail, so don't rely on it to free.  */
      free (p);
      p = NULL;
    }
#endif

  if (p == NULL)
    {
#if GNULIB_REALLOC_GNU && !NEED_REALLOC_GNU && !SYSTEM_MALLOC_GLIBC_COMPATIBLE
      if (n == 0)
	n = 1;
#endif
      result = malloc (n);
    }
  else
    result = realloc (p, n);

#if !HAVE_REALLOC_POSIX
  if (result == NULL)
    errno = ENOMEM;
#endif

  return result;
}
