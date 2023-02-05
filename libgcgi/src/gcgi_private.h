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

#ifndef __GCGI_PRIVATE_H__
#define __GCGI_PRIVATE_H__

#include "gcgi.h"
#include "common.h"
#include "parse.h"
#include "mime.h"

#ifdef USE_SSL
#   include "crypt.h"
#endif

BEGIN_C_DECLS

/*------ Structures ------*/
typedef struct querystringnode {
    char         *field;
    char         *data;
    size_t        size;
    MimeType      type;
    char         *subtype;
    MimeEncoding  encoding;
    char         *filename;
    size_t        truncated;
    struct querystringnode *next;
} QueryStringNode;


typedef struct querystringcoll {
    size_t           num;
    QueryStringNode *beg;
    QueryStringNode *end;
    QueryStringNode *cur;
} QueryStringColl;


typedef struct cgiquery {
    char            **env;
    size_t            envCount;
    size_t            queryCount;
    QueryStringColl **query;
} CgiQuery;


/*------ Structure Functions ------*/
gcgiReturnType         createCgiQuery( void );
gcgiReturnType         freeCgiQuery( void );
gcgiReturnType         insertQueryStringNode(QueryStringNode *q);
gcgiReturnType         createQueryStringNode(QueryStringNode **q);
gcgiReturnType         freeQueryStringNode(QueryStringNode *q);
gcgiReturnType         findQueryStringNodeFirst(char *field, QueryStringNode **q);
gcgiReturnType         findQueryStringNode(char *field, QueryStringNode **q);
gcgiReturnType         findQueryStringNodeCur(char *field, QueryStringNode **q);
gcgiReturnType         findQueryStringNodeByData(char *field, char *data, QueryStringNode **q);
gcgiReturnType         createQueryStringColl(QueryStringColl **q);
gcgiReturnType         createQueryStringCollArray(int numColls); 
gcgiReturnType         freeQueryStringColl(QueryStringColl *q);
gcgiReturnType         findQueryStringColl(char *field, int *index);
gcgiReturnType         getEnvVariables( void );

/*------ Debugging Functions ------*/
gcgiReturnType         printQueryStringNode(QueryStringNode *q, FILE *stream);
gcgiReturnType         printQueryStringColl(QueryStringColl *q, FILE *stream);

/*------ Parse Functions------*/
gcgiReturnType         parsePostQueryString(int *numBytes);
gcgiReturnType         parseGetQueryString(int *numBytes);
gcgiReturnType         parseUrlEncoded(char *querystring, int clen);
gcgiReturnType         parseFormData(FILE *data);
void                   decodeUrl(QueryStringNode *qString);

END_C_DECLS

#endif /* __GCGI_PRIVATE_H__ */
