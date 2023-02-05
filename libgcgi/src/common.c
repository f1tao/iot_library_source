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

#include "common.h"

FILE * 
tempFile(char *prefix, char **filename, int *fd) 
{
  FILE        *retFile;
  char        *tmpdir  = NULL;
  static char *pattern = NULL;
  mode_t       old_mode;

  if (! (tmpdir = getenv("TMPDIR"))) 
    tmpdir = getenv("TMP");

  if (! tmpdir) tmpdir = "/tmp";
  
  pattern = XMALLOC(char, (strlen(tmpdir)+strlen(prefix)+8));

  strcpy(pattern,tmpdir);
  strcat(pattern,"/");
  strcat(pattern, prefix);
  strcat(pattern, "XXXXXX");
  
  old_mode = umask(077);
  *fd = mkstemp(pattern);
  umask(old_mode);

  if (*fd < 0) {
    fprintf(stderr,"Unable to open temporary file");
    return NULL;
  }

  if (! (retFile = fdopen(*fd, "w"))) {
    fprintf(stderr,"Unable to get temporary file's descriptor");
    return NULL;
  }
  
  *filename = pattern;

  return retFile;
}


char *
readQueryFromStream(FILE *input, int clen)
{
    size_t       read;
    off_t        offset;
    static char *querystring;

    read           = 0;
    offset         = 0;
    querystring    = XMALLOC(char, clen+1);
    memset(querystring, 0, clen + 1);

    /* Read in the query from STDIN */
    do {
	read = fread(querystring + offset, sizeof(char), clen - offset, input);
	if (ferror(input)) 
	    return NULL;
	else if (read <= 0)
	    continue;
	offset += read;
    } while (offset < clen && read > 0);

    return querystring;
}


void *
xmalloc (size_t num)
{
  void *new = malloc (num);
  if (!new) {
    fprintf(stderr,"Memory exhausted.\n");
    exit(1);
  }
  return new;
}


void *
xrealloc (void *p, size_t num)
{
  void *new;

  if (!p)
    return xmalloc (num);

  new = realloc (p, num);
  if (!new) {
    fprintf(stderr,"Memory exhausted.\n");
    exit(1);
  }

  return new;
}
