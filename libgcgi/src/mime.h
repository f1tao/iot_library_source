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

#ifndef __MIME_H__
#define __MIME_H__

#include "common.h"
#include "crlfstringbuf.h"
#include "parse.h"
#include "gcgi_private.h"

BEGIN_C_DECLS


/*------ Structures ------*/
typedef struct mimepart {
    char            *version;
    MimeType         type;
    char            *subtype;
    MimeEncoding     encoding;
    char            *boundary;
    char            *name;
    char            *filename;
    char            *charset;
    MimeDisposition  disposition;
    MimeFormat       format;
    char            *description;
    char            *body;
    size_t           bodylen;
    size_t           bodysize;
    size_t           truncated;
    struct mimepart *next;
} MimePart; 


/*------ Structure Functions ------*/
void             mimeCreateMimePart(MimePart **part);
void             mimeFreeMimePart(MimePart *part);

/*------ MIME-Handling Functions ------*/
MimePart        *mimeParseMimeMessage(FILE *mimeIn, size_t mimeLimit, size_t mimePartLimit);
void             mimeFreeMimeMessage(MimePart *mime);
void             mimeParseHeaders(CRLFStringBuf *mimeIn, MimePart *part);
void             mimeParseHeadersFromEnv(MimePart *part);
void             mimeFillMimeStruct(MimePart *part, char *type, char *subtype, char *encoding, 
				    char *name, char *boundary, char *filename, char *desc, 
				    char *disp, char *dname, char *charset, char *format);
void             mimeParseContentType(char *text, char **type, char **subtype, 
				      char **charset, char **name, char **boundary, char **format);
void             mimeParseContentTransferEncoding(char *text, char **encoding);
void             mimeParseContentDisposition(char *text, char **disp, char **filename, char **name);
void             mimeParseContentDescription(char *text, char **desc);
size_t           mimeParseMimeBody(CRLFStringBuf *mimeIn, char *sboundary, char *eboundary, 
				   MimePart *part, size_t mimePartLimit, size_t mimeLimit);
MimeHeader       mimeHeaderName(char *text);

int              mimeReturnHeader(CRLFStringBuf *mimeIn, char **text, int *size);
int              mimeReturnHeaderByName(FILE *mimeIn, char *header, char **text, int *size);
int              mimeRemovePat(char *text, char *pattern, int stringLen);

END_C_DECLS

#endif  /* __MIME_H__ */
