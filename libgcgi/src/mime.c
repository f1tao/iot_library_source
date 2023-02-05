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

#include "mime.h"

static const char* const mimeStrTypes[] = {
    "text", 
    "image", 
    "audio", 
    "video", 
    "application", 
    "multipart", 
    "message", 
    "unknown"
};

static const char* const mimeStrEncodings[] = {
    "7bit", 
    "8bit", 
    "binary", 
    "quoted-printable", 
    "base64", 
    "unknown"
};


void 
mimeCreateMimePart(MimePart **part)
{
    *part = XMALLOC(MimePart, 1);

    (*part)->version     = NULL;
    (*part)->type        = unknown;
    (*part)->subtype     = NULL;
    (*part)->encoding    = unknown;
    (*part)->boundary    = NULL;
    (*part)->name        = NULL;
    (*part)->filename    = NULL;
    (*part)->charset     = NULL;
    (*part)->body        = NULL;
    (*part)->bodylen     = 0;
    (*part)->bodysize    = 0;
    (*part)->truncated   = 0;
    (*part)->next        = NULL;
    (*part)->description = NULL;
    (*part)->description = NULL;
}


void 
mimeFreeMimePart(MimePart *part)
{
    if (part->version     != NULL) XFREE(part->version);
    if (part->subtype     != NULL) XFREE(part->subtype);
    if (part->boundary    != NULL) XFREE(part->boundary);
    if (part->name        != NULL) XFREE(part->name);
    if (part->filename    != NULL) XFREE(part->filename);
    if (part->charset     != NULL) XFREE(part->charset);
    if (part->body        != NULL) XFREE(part->body);
    if (part->description != NULL) XFREE(part->description);
  
    XFREE(part);
}


MimePart *
mimeParseMimeMessage(FILE *mimeIn, size_t mimeLimit, size_t mimePartLimit)
{
    MimePart      *mime, *part, *cur;
    CRLFStringBuf *sbuf;
    char          *sboundary, *eboundary, *buffer;
    int            blen;

    mimeCreateMimePart(&mime);

    sbuf = crlfsbuf_new(3, mimeIn, mimePartLimit, mimeLimit);

    mimeParseHeadersFromEnv(mime);
    cur = mime;

    blen = strlen(mime->boundary);
    sboundary = XMALLOC(char, blen + 3);
    eboundary = XMALLOC(char, blen + 5);
    sprintf(sboundary, "--%s", mime->boundary);
    sprintf(eboundary, "--%s--", mime->boundary);
    
    buffer = crlfsbuf_read(sbuf);
    while ((strncmp(buffer,eboundary,strlen(eboundary)) != 0) && buffer != NULL) {
	if (! strncmp(buffer,sboundary,strlen(sboundary))) {
	    mimeCreateMimePart(&part);
	    mimeParseHeaders(sbuf, part);

	    /* Forward to the MIME-part body. */
	    do {
		buffer = crlfsbuf_read(sbuf);
	    } while (strncmp(buffer, "\r\n", 2) != 0 && buffer != NULL);

	    mimeParseMimeBody(sbuf, sboundary, eboundary, part, mimePartLimit, mimeLimit);

	    cur->next = part;
	    cur = cur->next;
	}

	/* Have we surpassed the MIME message limit? */
	if (mimeLimit > 0 && crlfsbuf_truncated(sbuf)) {
	    mime->truncated++;
	    break;
	}

	buffer = crlfsbuf_read(sbuf);
    }

    XFREE(sboundary);
    XFREE(eboundary);

    crlfsbuf_free(sbuf);

    return mime;
}


extern void      
mimeFreeMimeMessage(MimePart *mime)
{
    MimePart *m, *n;
  
    m = mime;
    n = m->next;

    while (n != NULL) {
	mimeFreeMimePart(m);
	m = n;
	n = n->next;
    }
    mimeFreeMimePart(m);
}


void 
mimeParseHeaders(CRLFStringBuf *mimeIn, MimePart *part)
{
    char      *type, *subtype, *encoding, *name, *boundary;
    char      *filename, *desc, *disp, *dname, *charset, *format;
    char      *buf;
    int        size, endOfHeaders;
    MimeHeader header;

    type = subtype = encoding = name = boundary = NULL;
    format = filename = desc = disp = dname = charset = NULL;

    do {
	endOfHeaders = mimeReturnHeader(mimeIn, &buf, &size);
	header = mimeHeaderName(buf);
	
	switch (header) {
	case contentType:
	    mimeParseContentType(buf, &type, &subtype, &charset, &name, &boundary, &format);
	    break;
	case contentEncoding:
	    mimeParseContentTransferEncoding(buf, &encoding);
	    break;
	case contentDisposition:
	    mimeParseContentDisposition(buf, &disp, &filename, &dname);
	    break;
	case contentDescription:
	    mimeParseContentDescription(buf, &desc);
	    break;
	default:
	    break;
	} 

	XFREE(buf);
	size = 0;

    } while (endOfHeaders >= 0);

    mimeFillMimeStruct(part, type, subtype, encoding, name, boundary, filename, desc, 
		       disp, dname, charset, format);
}


void
mimeParseHeadersFromEnv(MimePart *part)
{
    char *buf, *type, *subtype, *encoding, *name, *boundary, *env;
    char *filename, *desc, *disp, *dname, *charset, *format;
    int   len;

    type = subtype = encoding = name = boundary = NULL;
    format = filename = desc = disp = dname = charset = NULL;

    env = gcgiFetchEnvVar(gcgiContentType);
    len = strlen(env);
    buf = XMALLOC(char, len + 17);
    snprintf(buf, len+16, "Content-Type: %s\r\n", env);
    buf[len+16] = '\0';

    mimeParseContentType(buf, &type, &subtype, &charset, &name, &boundary, &format);

    mimeFillMimeStruct(part, type, subtype, encoding, name, boundary, filename, desc, 
		       disp, dname, charset, format);

    XFREE(buf);
}

void
mimeFillMimeStruct(MimePart *part, char *type, char *subtype, char *encoding, 
		   char *name, char *boundary, char *filename, char *desc, 
		   char *disp, char *dname, char *charset, char *format)
{
    /* Assume ascii text when no "Content-Type" header present. */
    if (type == NULL) {
	part->type = text;
	subtype = XMALLOC(char, 6);
	strcpy(subtype,"plain");
    }
    else if (strncasecmp("text", type, strlen(type)) == 0) 
	part->type = text;
    else if (strncasecmp("image", type, strlen(type)) == 0)
	part->type = image;
    else if (strncasecmp("audio", type, strlen(type)) == 0)
	part->type = audio;  
    else if (strncasecmp("video", type, strlen(type)) == 0)
	part->type = video;  
    else if (strncasecmp("application", type, strlen(type)) == 0)
	part->type = application;  
    else if (strncasecmp("multipart", type, strlen(type)) == 0)
	part->type = multipart;  
    else if (strncasecmp("message", type, strlen(type)) == 0)
	part->type = message;  
    else
	part->type = unknown;

    /* Assume the format is fixed when no "Content-Type" header present. */
    if (format == NULL) 
	part->format = fixed;
    else if (strncasecmp("fixed", format, strlen(format)) == 0) 
	part->format = fixed;
    else if (strncasecmp("flowed", format, strlen(format)) == 0)
	part->format = flowed;
    else 
	part->format = fixed;

    /* Assume 7Bit encoding when no "Content-Transfer-Encoding" header present. */
    if (encoding == NULL) {
	type == text ? (part->encoding = sevenbit) : (part->encoding = binary);
    }
    else if (strncasecmp("7bit",encoding,strlen(encoding)) == 0) 
	part->encoding = sevenbit;
    else if (strncasecmp("8bit",encoding,strlen(encoding)) == 0)
	part->encoding = eightbit;
    else if (strncasecmp("binary",encoding,strlen(encoding)) == 0)
	part->encoding = binary;  
    else if (strncasecmp("quoted-printable",encoding,strlen(encoding)) == 0)
	part->encoding = quotedprintable;  
    else if (strncasecmp("base64",encoding,strlen(encoding)) == 0)
	part->encoding = basesixtyfour;
    else
	part->encoding = sevenbit;	       
    
    if (disp == NULL) 
	part->disposition = attachment;
    else if (strncasecmp("inline", disp, strlen(disp)) == 0) 
	part->disposition = inlined;
    else if (strncasecmp("attachment", disp, strlen(disp)) == 0)
	part->disposition = attachment;
    else if (strncasecmp("form-data", disp, strlen(disp)) == 0)
	part->disposition = formdata;
    else
	part->disposition = attachment;        

    if (subtype != NULL)    part->subtype = subtype;
    if (filename != NULL)   part->filename = filename;
    if (name != NULL)       part->name = name;
    else if (dname != NULL) part->name = dname;
    if (desc != NULL)       part->description = desc;
    if (charset != NULL)    part->charset = charset;
    if (boundary != NULL)   part->boundary = boundary;

    if (type != NULL)       XFREE(type);
    if (format != NULL)     XFREE(format);
    if (disp != NULL)       XFREE(disp);
}


void 
mimeParseContentType(char *text, char **type, char **subtype, 
		     char **charset, char **name, char **boundary, char **format)
{
    char  *p, *q;
    char **tokens;
    char  *property, *value;
    int    tlen;
    int    i;

    *type = *subtype = *charset = *name = *boundary = *format = NULL;
    text? (tlen  = strlen(text)) : (tlen = 0);

    tokenizeString(text, tlen, &tokens);

    parseToken(tokens[1], &property, &value);

    /* Figure out the type of message, example: Content-Type: text/html */
    for (p = q = value; *p != '/' && *p != '\0'; p++);
    *type = XMALLOC(char, p - q + 1);
    strncpy(*type, q, p - q);
    (*type)[p-q] = '\0';
    
    /* Figure out the subtype */
    for (q = p; *q != '\0'; q++);
    *subtype = XMALLOC(char, q - p + 1);
    strncpy(*subtype, p + 1, q - p);
    (*subtype)[q-p] = '\0';

    XFREE(value);

    for (i = 2; tokens[i] != '\0'; i++) {
	parseToken(tokens[i], &property, &value);
	
	if (property == NULL)
	    XFREE(value);

	else if (!strncasecmp(property, "charset", 7))
	    *charset = value;

	else if (!strncasecmp(property, "name", 4))
	    *name = value;

	else if (!strncasecmp(property, "boundary", 8))
	    *boundary = value;

	else if (!strncasecmp(property, "format", 6))
	    *format = value;

	else
	    XFREE(value);

	XFREE(property);
    }

    freeStringArray(tokens);
}


void 
mimeParseContentTransferEncoding(char *text, char **encoding)
{
    char **tokens;
    char  *property, *value;
    int    tlen;

    *encoding = NULL;
    text? (tlen = strlen(text)) : (tlen = 0);

    tokenizeString(text, tlen, &tokens);

    parseToken(tokens[1], &property, &value);
    (*encoding) = value;
}


void 
mimeParseContentDisposition(char *text, char **disp, char **filename, char **name)
{
    int   tlen;
    int   i;
    char  *property, *value, *p;
    char **tokens;

    *disp = *filename = *name = NULL;
    text? (tlen = strlen(text)) : (tlen = 0);

    tokenizeString(text, tlen, &tokens);

    parseToken(tokens[1], &property, &value);
    (*disp) = value;

    for (i = 2; tokens[i] != '\0'; i++) {
	parseToken(tokens[i], &property, &value);

	if (property == NULL)
	    XFREE(value);
	
	if (!strncasecmp(property, "filename", 8))
	    *filename = value;

	else if (!strncasecmp(property, "name", 4))
	    *name = value;

	else
	    XFREE(value);

	XFREE(property);
    }

    /* 
       Some browsers, like MS Internet Explorer send the entire path
       for the file.  The standard states that the only the filename should
       be included, so we check for the full path and remove it if present.
    */
    if (*filename && strlen(*filename) > 0) {
	/* Forward to the end of the string. */
	for (p = *filename; *p != '\0'; p++);
	/* Iterate backwards until a backslash or forward slash or the beginning of the string is found. */
	for (; *p != '\\' && *p != '/' && p > *filename; p--);
	if (*p == '\\' || *p == '/') {
	    strcpy(*filename, p+1);
	}
    }

    freeStringArray(tokens);
}


void 
mimeParseContentDescription(char *text, char **desc)
{
    char *p, *q, *offset;
    int   tlen;

    *desc = NULL;
    text? (tlen = strlen(text)) : (tlen = 0);
    offset = text + tlen;

    for (q = text; *q != '\r' && q < offset; p++);
    
    for (p = text; isspace((int)*p); p++);  /* Remove whitespace from beggining of string. */
    *desc = XMALLOC(char, q - p + 1);
    strncpy(*desc, q, p-q);
    (*desc)[p-q] = 0;
}


size_t 
mimeParseMimeBody(CRLFStringBuf *mimeIn, char *sboundary, char *eboundary, 
		  MimePart *part, size_t mimePartLimit, size_t mimeLimit)
{
    char  *line, *peek;
    int    i, endOfPart;
    int    llen, size;

    /* This message is MIME encoded, look for s/eboundary as end of Message. */
    endOfPart = 0;
    
    part->bodylen  = 0;
    part->bodysize = 256;
    part->body     = XMALLOC(char, part->bodysize);
    memset(part->body, 0, part->bodysize);

    do {
        crlfsbuf_readdata(mimeIn, &line, &size, &llen);

        /* Check if we have reached the end of the MIME body. */
	peek = crlfsbuf_peek(mimeIn);
        if ((strncmp(peek, eboundary, strlen(eboundary)) == 0) || 
	    (strncmp(peek, sboundary, strlen(sboundary)) == 0))
            endOfPart++;
	
	/* If reading the previous line pushes us over the limit,  *
	 * only copy enough into the MIME part to equal the limit. */
	if (mimePartLimit > 0 && (part->bodylen + llen) >= mimePartLimit) {
	    llen -= (part->bodylen + llen) - mimePartLimit;
	}

	if (!part->truncated) {
	    if (part->bodylen + llen <= part->bodysize - 1) {
		memcpy(part->body + part->bodylen, line, llen);
		part->bodylen += llen;
	    }
	    else {
		part->bodysize = (part->bodylen + llen) * 2;
		part->body     = XREALLOC(char, part->body, part->bodysize);
		memcpy(part->body + part->bodylen, line, llen);
		part->bodylen += llen;
	    }
	}

	/* If the input was too long and truncated, read *
	 * and discard the rest of the MIME part.        */
	if (mimePartLimit > 0 && part->bodylen >= mimePartLimit)
	    part->truncated++;

	/* If the entire MIME input was greater than the limit, exit. */
	if (mimeLimit > 0 && crlfsbuf_truncated(mimeIn)) {
	    part->truncated++;
	    break;
	}

    } while (!endOfPart && line != NULL);
    
    /* If binary encoded, remove the final CRLF that seperates the MIME boundary. */
    if (part->encoding == binary || part->encoding == unknown) {
	if (part->body[part->bodylen - 1] == '\n' && part->body[part->bodylen - 2] == '\r') {
	    part->body[part->bodylen - 1] = '\0';
	    part->body[part->bodylen - 2] = '\0';
	    part->bodylen -= 2;
	}
    }
    /* Else, if not text, but encoded, remove CRLF endings. */
    else if (part->encoding == basesixtyfour && part->encoding == quotedprintable) {
	i = removePat(part->body, "\r\n", part->bodylen);
	part->bodylen -= i*2;
    }

    return part->bodylen;
}


MimeHeader
mimeHeaderName(char *text)
{
    MimeHeader htype;

    htype = unknownHeader;

    switch (text[0]) {
    case 'c':
    case 'C':
	if (strncasecmp("Content-Type: ", text, 14) == 0)
	    htype = contentType;

	else if (strncasecmp("Content-Transfer-Encoding: ", text, 27) == 0)
	    htype = contentEncoding;

	else if (strncasecmp("Content-Disposition: ", text, 21) == 0)
	    htype = contentDisposition;

	else if (strncasecmp("Content-Description: ", text, 21) == 0)
	    htype = contentDescription;
	break;
    case 'm':
    case 'M':
	if (strncasecmp("Mime-Version: ", text, 14) == 0)
	    htype = mimeVersion;
	break;
    default:
	htype = unknownHeader;
    }

    return htype;
}


/* Returns the next header from the incomming stream. */
int 
mimeReturnHeader(CRLFStringBuf *mimeIn, char **text, int *size)
{
    int   lsize;
    int   llen, tlen;
    char  *line, *peek;
    int   endOfHeaders;
    
    llen  = 0;
    lsize = 0;
    
    tlen  = 0;
    *size = 256;
    *text = XMALLOC(char, *size);
    memset(*text, 0, *size);

    endOfHeaders = 0;

    do {
        line = crlfsbuf_read(mimeIn);

        /* Check if we have reached the end of the headers. */
	peek = crlfsbuf_peek(mimeIn);
        if (peek[0] == '\r' && peek[1] == '\n' && peek[2] == '\0')
            endOfHeaders++;
	
	llen = strlen(line);
	if (tlen + llen <= *size - 1) {
	    strcat(*text, line);
	    tlen += llen;
	}
	else {
	    *size  = (*size+lsize) * 2;
	    tlen  += llen;
	    *text  = XREALLOC(char, *text, *size);
	    strcat(*text, line);
	}
    } while (!endOfHeaders && isspace((int)peek[0]) && line != NULL);

    replacePat(text, "\r\n", " ", size);

    if (endOfHeaders || line == NULL)
        return -1;
    else 
        return 0;
}

int 
mimeReturnHeaderByName(FILE *mimeIn, char *header, char **text, int *size)
{
    off_t pos;
    int   hsize, lsize;
    int   llen, tlen;
    char  *line;
    int   endOfHeaders, foundHeader;
    
    pos   = ftell(mimeIn);
    header? (hsize = strlen(header)) : (hsize = 0);

    llen  = 0;
    lsize = 256;
    line  = XMALLOC(char, lsize);
    memset(line, 0, lsize);

    tlen  = 0;
    *size = 256;
    *text = XMALLOC(char, *size);
    memset(*text, 0, *size);

    endOfHeaders = 0;
    foundHeader  = 0;

    do {
        readCRLFLine(mimeIn, &line, &lsize);

        /* Check if we have reached the end of the headers. */
        if (line[0] == '\r' && line[1] == '\n' && line[2] == '\0')
            endOfHeaders++;

        /* Found the header. */
        if (strncasecmp(line, header, hsize) == 0 || foundHeader) {

            if (foundHeader && !isspace((int)line[0]))
                break;

            foundHeader++;

            llen = strlen(line);
            if (tlen + llen <= *size - 1) {
                strcat(*text, line);
                tlen += llen;
            }
            else {
                *size  = (*size+lsize) * 2;
                tlen  += llen;
                *text  = XREALLOC(char, *text, *size);
                strcat(*text, line);
            }
        }
    } while (!endOfHeaders && !feof(mimeIn));

    replacePat(text, "\r\n", " ", size);
    replacePat(text, header, "", size);

    /* Put the file pointer back where we got it. */
    fseek(mimeIn, pos - ftell(mimeIn), SEEK_CUR);

    XFREE(line);

    if (endOfHeaders && !foundHeader) 
        return -1;
    else 
        return 0;
}
