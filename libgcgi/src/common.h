/* -*-mode:c; c-style:k&r; c-basic-offset:4; -*- */
/*
 * GCGI Library, implementing NCSA'a Common Gateway Interface and RFC2338.
 * Copyright (C) 2001-2002 Julian Catchen, julian@catchen.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
#   define BEGIN_C_DECLS    extern "C" {
#   define END_C_DECLS      }
#else
#   define BEGIN_C_DECLS
#   define END_C_DECLS
#endif

#if HAVE_CONFIG_H
#   include <config.h>
#endif

#include <stdio.h>
#include <sys/types.h> 

#if STDC_HEADERS
#   include <stdlib.h>
#   include <string.h>
#elif HAVE_STRINGS_H
#   include <strings.h>
#endif /* STDC_HEADERS */

#if HAVE_UNISTD_H
#   include <unistd.h>    /* unlink() function          */
#endif

#if HAVE_ERRNO_H
#   include <errno.h>
#endif
#ifndef errno
extern int errno;         /* Some systems #define this  */
#endif

#include <regex.h>        /* Regular Expression Library */
#include <ctype.h>        /* isspace() function         */
#include <math.h>         /* floor() function           */
#include <sys/stat.h>     /* umask() function           */


BEGIN_C_DECLS

/*------ Memory Allocation Functions/Macros ------*/
#define XMALLOC(type, num)                                  \
        ((type *) xmalloc ((num) * sizeof(type)))
#define XREALLOC(type, p, num)                              \
        ((type *) xrealloc ((p), (num) * sizeof(type)))
#define XFREE(stale)                            do {        \
        if (stale) { free (stale);  stale = 0; }            \
                                                } while (0)

void        *xmalloc  (size_t num);
void        *xrealloc (void *p, size_t num);
FILE        *tempFile (char *prefix, char **filename, int *fd);
char        *readQueryFromStream(FILE *stream, int clen);

END_C_DECLS

#endif /* __COMMON_H__ */
