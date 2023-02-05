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

#ifndef __CRLFSTRINGBUF_H__
#define __CRLFSTRINGBUF_H__

#include "common.h"

/* 
 * The CRLF string buffer is a structure used to buffer 
 * CRLF terminated strings.  Useful for reading incoming
 * streams containing MIME documents, or HTTP data. The 
 * stream buffer allows the user to peek ahead at the data
 * to look for MIME boundaries or the end of e-mail headers.
 */

typedef struct crlfstringbuf 
{
    int       numb;         /* Number of line buffers           */
    size_t    index;        /* Current Buffer                   */
    int       bufsfull;     /* Number of buffers full           */
    size_t    bytesread;    /* Number of bytes read from stream */
    char    **buf;          /* Array of Line Buffers            */
    size_t   *size;         /* Array of Buffer Sizes            */
    size_t   *length;       /* Array of how full each buffer is */
    FILE     *stream;       /* Stread to read from              */
    size_t    lineLimit;    /* Single line limit                */
    size_t    bufLimit;     /* Buffer upper-bound limit         */
    int       endofstream;  /* End of Stream Flag               */
    int       truncated;    /* Line Truncated Flag              */
} CRLFStringBuf;


CRLFStringBuf  *crlfsbuf_new(int bufferSize, FILE *stream, size_t lineLimit, size_t bufferLimit);
char           *crlfsbuf_read(CRLFStringBuf *sb);
size_t          crlfsbuf_readdata(CRLFStringBuf *sb, char **buffer, size_t *size, size_t *length);
char           *crlfsbuf_peek(CRLFStringBuf *sb);
size_t          crlfsbuf_linesread(CRLFStringBuf *sb);
size_t          crlfsbuf_bytesread(CRLFStringBuf *sb);
int             crlfsbuf_truncated(CRLFStringBuf *sb);
void            crlfsbuf_dump(CRLFStringBuf *sb, FILE *stream);
void            crlfsbuf_free(CRLFStringBuf *sb);

#endif /* __CRLFSTRINGBUF_H__ */
