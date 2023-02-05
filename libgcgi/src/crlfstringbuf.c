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

#include "crlfstringbuf.h"

static int  readCRLFLine(FILE *stream, char **line, size_t *size, size_t linelimit, int *endofstream, int *truncated);
static void fillBuffer(CRLFStringBuf *crlfsbuf, int startPos, int endPos);

CRLFStringBuf *
crlfsbuf_new(int bufferSize, FILE *stream, size_t linelimit, size_t buflimit)
{
    static CRLFStringBuf *crlfsbuf;
    int i;

    if (bufferSize <= 2)
	return NULL;
    
    crlfsbuf         = XMALLOC(CRLFStringBuf, 1);
    crlfsbuf->numb   = bufferSize;
    crlfsbuf->stream = stream;

    /* Malloc the array of buffers. */
    crlfsbuf->buf    = XMALLOC(char *, crlfsbuf->numb);

    /* Initialize char pointers to NULL */
    for (i = 0; i < crlfsbuf->numb; i++)
	(crlfsbuf->buf)[i] = NULL;

    /* Malloc the array of buffer sizes. */
    crlfsbuf->size    = XMALLOC(size_t, crlfsbuf->numb);

    /* Initialize buffer sizes to 0. */
    for (i = 0; i < crlfsbuf->numb; i++)
	(crlfsbuf->size)[i] = 0;

    /* Malloc the array of buffer sizes. */
    crlfsbuf->length = XMALLOC(size_t, crlfsbuf->numb);

    /* Initialize buffer sizes to 0. */
    for (i = 0; i < crlfsbuf->numb; i++)
	(crlfsbuf->length)[i] = 0;

    /* Start index off at position 0. */
    crlfsbuf->index = 0;

    /* Start Number of buffers full at 0. */
    crlfsbuf->bufsfull = 0;

    /* Set the line length limit. */
    crlfsbuf->lineLimit = linelimit;

    /* Set the buffer length limit. */
    crlfsbuf->bufLimit = buflimit;

    /* Set endofstream indicator flag to 0. */
    crlfsbuf->endofstream = 0;

    /* Set the truncation flag to 0. */
    crlfsbuf->truncated = 0;

    /* Set total number of bytes read from stream to 0. */
    crlfsbuf->bytesread = 0;

    /* Fill the buffer. */
    fillBuffer(crlfsbuf, 0, crlfsbuf->numb);
    
    return crlfsbuf;
}


char *
crlfsbuf_read(CRLFStringBuf *sb)
{
    int cur;

    /* Reached end of stream and no buffers left.  CRLFStringBuf is empty. */
    if (sb->endofstream && sb->bufsfull == 0)
	return NULL;

    cur = sb->index % sb->numb;
    
    sb->index++;
    sb->bufsfull--;

    /* Make sure we have at least two buffers left or        *
     * crlfsbuf_peek() won't work. If the number of buffers  * 
     * is 2 and we haven't reached the end of the stream,    * 
     * then fill the buffer back up.                         */
    if (sb->bufsfull <= 1 && !sb->endofstream) {
	fillBuffer(sb, sb->index + 1, sb->numb - 1);
    }

    //fprintf(stderr, "Reading CRLF String: %s", sb->buf[cur]);

    return (sb->buf)[cur];
}


size_t
crlfsbuf_readdata(CRLFStringBuf *sb, char **buffer, size_t *size, size_t *length)
{
    int cur;

    /* Reached end of stream and no buffers left.  CRLFStringBuf is empty. */
    if (sb->endofstream && sb->bufsfull == 0) {
	*buffer = NULL;
	*size   = 0;
	*length = 0; 
	return 0;
    }

    cur = sb->index % sb->numb;
    
    sb->index++;
    sb->bufsfull--;

    /* Make sure we have at least two buffers left or        *
     * crlfsbuf_peek() won't work. If the number of buffers  * 
     * is 2 and we haven't reached the end of the stream,    * 
     * then fill the buffer back up.                         */
    if (sb->bufsfull <= 1 && !sb->endofstream) {
	fillBuffer(sb, sb->index + 1, sb->numb - 1);
    }

    *buffer = (sb->buf)[cur];
    *size   = (sb->size)[cur];
    *length = (sb->length)[cur];

    //fprintf(stderr, "Reading CRLF Data: %s", *buffer);

    return sb->length[cur];
}


char *
crlfsbuf_peek(CRLFStringBuf *sb)
{
    return ((sb->buf)[sb->index % sb->numb]);
}


size_t
crlfsbuf_linesread(CRLFStringBuf *sb)
{
    return sb->index;
}


int
crlfsbuf_truncated(CRLFStringBuf *sb)
{
    return sb->truncated;
}


size_t
crlfsbuf_bytesread(CRLFStringBuf *sb)
{
    return sb->bytesread;
}


void
crlfsbuf_dump(CRLFStringBuf *sb, FILE *stream)
{
    int i;

    for (i = 0; i < sb->numb; i++) {
	if ((sb->buf)[i] != NULL)
	    fwrite((sb->buf)[i], sizeof(char), (sb->length)[i], stream);
    }
}


void
crlfsbuf_free(CRLFStringBuf *sb)
{
    int i;

    /* Free buffer array. */
    for (i = 0; i < sb->numb; i++) {
	/*fprintf(stderr, 
		"[%d] Attempting to free crlfsbuf: size: %d, length: %d\n", 
		i, sb->size[i], sb->length[i]);
	*/
	XFREE((sb->buf)[i]);
    }

    XFREE(sb->buf);

    /* Free buffer size array. */
    XFREE(sb->size);

    /* Free buffer length array. */
    XFREE(sb->length);

    XFREE(sb);
}


/* Fill the internal buffer back up.  Check that we have  *
 * not violated either the line limit or the buffer limit *
 * in the process.                                        */
static void 
fillBuffer(CRLFStringBuf *crlfsbuf, int startPos, int endPos)
{
    int i, blimit, limit, truncated;

    limit     = 0;
    blimit    = 0;
    truncated = 0;

    for (; crlfsbuf->bufsfull < endPos && !crlfsbuf->endofstream; startPos++) {
	i = startPos % crlfsbuf->numb;

	/* If the buffer limit is less than the line limit, make sure we only read that much. */
	(crlfsbuf->bufLimit > 0) ? (blimit = crlfsbuf->bufLimit - crlfsbuf->bytesread) : (blimit = 0);
	(crlfsbuf->lineLimit > 0 && crlfsbuf->lineLimit <= blimit) ? (limit = crlfsbuf->lineLimit) : (limit = blimit);	

	crlfsbuf->length[i] = readCRLFLine(crlfsbuf->stream, &(crlfsbuf->buf[i]), &(crlfsbuf->size[i]),
				           limit, &crlfsbuf->endofstream, &truncated);

	crlfsbuf->bytesread += crlfsbuf->length[i];
	crlfsbuf->bufsfull++;

	if (truncated && crlfsbuf->bytesread >= crlfsbuf->bufLimit)
	    crlfsbuf->truncated++;

	/*fprintf(stderr, "[End of Stream: %d] [Index: %d] Reading CRLF String: %s", 
	  crlfsbuf->endofstream, i, crlfsbuf->buf[i]);*/
    }
}


/* Read a line in from the file stream and check to make  *
 * sure that the entire string was read. The string is    *
 * terminated by "\r\n" (CRLF). If the character array is *
 * too short, make it bigger.                             *
 *                                                        *
 * Return the number of characters read.                  */
static int
readCRLFLine(FILE *stream, char **line, size_t *size, size_t linelimit, int *endofstream, int *truncated)
{
    unsigned char c;
    size_t        llen;

    /* Return if end of stream. */
    if (feof(stream) || *endofstream)
	return 0;
    
    /* If the string is NULL malloc it to an initial size. */
    if ((*line) == NULL && (*size) == 0) {
	(*size) = 256;
	(*line) = XMALLOC(char, *size);
    }

    memset(*line, 0, *size);
    llen = 0;

    /* Make sure we read an entire line ending with CRLF ("\r\n"). */
    do {	    
	c = fgetc(stream);

	if (feof(stream))
	    break;

	if (llen + 1 <= (*size) - 1) {
	    (*line)[llen] = c;
	    llen++;
	}
	else {
	    *size *= 2;
	    *line  = XREALLOC(char, *line, *size);
	    (*line)[llen] = c;
	    llen++;
	}

	/* Check that we have not read in more than the limit. */
	if (linelimit > 0 && llen >= linelimit) {
	    fprintf(stderr, "readCRLFLine: LineLimit: %d, Line Length: %d\n", linelimit, llen);
	    (*truncated)++;
	    break;
	}

    } while ((*line)[llen - 1] != '\n');
    
    /* Ensure that the line is always NULL-terminated. */
    (*line)[llen] = '\0';

    if (feof(stream))
	*endofstream = 1;
    else 
	*endofstream = 0;

    return llen;
}
