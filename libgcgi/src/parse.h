/* -*-mode:c; c-style:k&r; c-basic-offset:4; -*- */
/*
 * GCGI Library, implementing NCSA'a Common Gateway Interface and RFC2338.
 * Copyright (C) 2001-2002 Julian Catchen, julian@catchen.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __PARSE_H__
#define __PARSE_H__

#include        "common.h"

BEGIN_C_DECLS

int      replacePat(char **string, char *pattern, char *replace, int *size);
int      numOccurarnces(char *string, char *pattern);
int      removePat(char *string, char *pattern, int stringLen);
int      regexPat(char *string,char *pattern,int stringLen);
void     insertString(char **string, char *insert, int offset, int *size);
int      tokenizeString(char *string, int size, char ***output);
int      tokenizeURLString(char *string, int size, char ***output);
int      freeStringArray(char **string);
int      parseToken(char *token, char **property, char **value);
int      readCRLFLine(FILE *input, char **line, int *size);

END_C_DECLS

#endif  /* __PARSE_H__ */
