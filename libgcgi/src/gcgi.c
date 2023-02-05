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

#include "gcgi_private.h"

static const char* const envVars[] = 
    {
	"HTTP_COOKIE",
	"HTTP_REFERER",
	"AUTH_TYPE", 
	"CONTENT_LENGTH",
	"CONTENT_TYPE",
	"GATEWAY_INTERFACE",
	"PATH_INFO",
	"PATH_TRANSLATED",
	"QUERY_STRING",
	"REMOTE_ADDR",
	"REMOTE_HOST",
	"REMOTE_IDENT",
	"REMOTE_USER",
	"REQUEST_METHOD",
	"SCRIPT_NAME",
	"SERVER_NAME",
	"SERVER_PORT",
	"SERVER_PROTOCOL",
	"SERVER_SOFTWARE"
    };

#define ENVCOUNT 19

/*------ Global Variables ------*/
static CgiQuery *cgiQuery         = NULL;
static int       debug            = 0;
static char     *envVariablesFile = NULL;
static char     *cgiQueryFile     = NULL;
static size_t    fieldLimit       = 0;
static size_t    queryLimit       = 0;

static const char gcgi_ident[] =
    "$GCGI: GCGI " VERSION " Copyright (C) 2001-2002 Julian Catchen $\n"
    "$Authors:  Julian Catchen, topeka@catchen.org $";

gcgiReturnType 
initCgi( ) 
{
    int numBytes, gcgifd;

    if (debug)
	gcgiLoadEnvVariables(envVariablesFile);

    /* Dup stdout to gcgiOut */
    gcgifd = dup(fileno(stdout));
    gcgiOut = fdopen(gcgifd, "w");

    /* Create the cgiQuery object. */
    createCgiQuery();

    if (cgiQuery->env[gcgiRequestMethod] == NULL ) 
	return GCGIFATALERROR;
    
    if (! strncasecmp(cgiQuery->env[gcgiRequestMethod],"GET", 3) ) {
	parseGetQueryString(&numBytes);
	if (numBytes < 0)
	    return GCGIFATALERROR;
    }
    else if (! strncasecmp(cgiQuery->env[gcgiRequestMethod],"POST", 4) ) {
	parsePostQueryString(&numBytes);
	if (numBytes < 0)
	    return GCGIFATALERROR;
    }
    else
	return GCGIFATALERROR;
  
    return GCGISUCCESS;
}


void 
freeCgi( )
{
    freeCgiQuery();

    if (envVariablesFile != NULL) XFREE(envVariablesFile);
    if (cgiQueryFile != NULL) XFREE(cgiQueryFile);
    
}


/* Limits specified in number of bytes. */
gcgiReturnType
gcgiSetLimits(size_t flimit, size_t qlimit)
{
    fieldLimit = flimit;
    queryLimit = qlimit;

    return GCGISUCCESS;
}


gcgiReturnType 
parsePostQueryString(int *numBytes)
{
    FILE   *input;
    char   *querystring;
    size_t  clen;
    int     result;

    querystring = NULL;
    result      = 0;

    /* Return if we don't have the content length env variable. */
    if (cgiQuery->env[gcgiContentLength] == NULL || cgiQuery->env[gcgiContentType] == NULL)
	return GCGIFATALERROR;
    
    clen = strtol(cgiQuery->env[gcgiContentLength], NULL, 10);
    /* Content Length was out of range */
    if (errno == ERANGE)
	return GCGIFATALERROR;

    if (debug) {
	if ((input = fopen(cgiQueryFile, "r")) == NULL)
	    return GCGIFATALERROR;
    }
    else {
	input = stdin;
    }

    /* Standard URL Encoded POST string. */
    if (strncasecmp(cgiQuery->env[gcgiContentType], "application/x-www-form-urlencoded", 33) == 0) {
	querystring = readQueryFromStream(input, clen);
	parseUrlEncoded(querystring, clen);

	*numBytes = clen;
    }
    
    /* RFC2388 Encoding */
    else if (strncasecmp(cgiQuery->env[gcgiContentType], "multipart/form-data", 19) == 0) {
	/* Pass the file to the MIME library to parse. */
	if ((result = parseFormData(input)) == GCGIFATALERROR)
	    return GCGIFATALERROR;
	
	*numBytes = clen;    
    }
   
    /* Unknown Encoding. */
    else {
	fprintf(stderr,"Unknown Encoding.\n");
	return GCGIFATALERROR;
    }

    if (debug) fclose(input);
	
    XFREE(querystring);
	
    if (result == GCGITRUNCATED)
	return GCGITRUNCATED;
    else
	return GCGISUCCESS;
}


gcgiReturnType 
parseGetQueryString(int *numBytes)
{
    size_t clen;
    
    if (cgiQuery->env[gcgiQueryString] == NULL)
	return GCGIFATALERROR;

    clen = strlen(cgiQuery->env[gcgiQueryString]);
 
    if (clen <= 0)
	return GCGIFATALERROR;

    *numBytes = parseUrlEncoded(cgiQuery->env[gcgiQueryString], clen);

    return GCGISUCCESS;
}


gcgiReturnType 
parseUrlEncoded(char *querystring, int clen) 
{
    QueryStringNode *qstring; 
    char            *beg, *end, *qlen;
    char            *offset, *p; 
    int              i;

    beg = end = qlen = NULL;

    offset = querystring + strlen(querystring);
    /* Count the number of fileds in the Query String. */
    for (p = querystring, i = 0; p < offset; p++)
	if (*p == '&')
	    i++;
    /* There is one more field than '&' characters. */
    i++;
    /* Create the QueryStringColl Array */
    createQueryStringCollArray(i);

    qlen = querystring + clen;
    for (beg = querystring; beg < qlen && end < qlen; beg = end+1) {
	/* Malloc the queryString object and initialize it. */
	createQueryStringNode(&qstring);

	/* Get Field Name */
	for (end = beg; *end != '=' && end < qlen; end++);
	qstring->field = XMALLOC(char, end - beg + 1);
	strncpy(qstring->field, beg, end-beg);
	qstring->field[end-beg] = '\0';                      /* Zero out the string */

	/* Get Data */
	for (beg = end+1; *end != '&' && end < qlen; end++);
	qstring->data = XMALLOC(char, end - beg + 1);
	strncpy(qstring->data, beg, end-beg);
	qstring->data[end-beg] = '\0';                       /* Zero out the string */
	qstring->size = end - beg + 1;
	
	decodeUrl(qstring);

	/* Add struct to linked list. */
	insertQueryStringNode(qstring);

    } /* for (beg = querystring; *beg < qlen && *end < qlen; beg++) */
    
    return GCGISUCCESS;
}


gcgiReturnType 
parseFormData(FILE *data)
{
    MimePart *mime, *n;
    QueryStringNode *node;
    int len, i, trunc;

    if ((mime = mimeParseMimeMessage(data, queryLimit, fieldLimit)) == NULL)
	return GCGIFATALERROR;
    
    /* Check if the MIME message was truncated. */
    trunc = mime->truncated;

    /* Count the number of fileds in the Query String. */
    for (n = mime->next, i = 0; n != NULL; n = n->next) i++;
    createQueryStringCollArray(i);

    for (n = mime->next; n != NULL; n = n->next) {
	createQueryStringNode(&node);
	len = strlen(n->name);
	node->field = XMALLOC(char, len + 1);
	strncpy(node->field, n->name, len);
	node->field[len] = '\0';

	node->type     = n->type;
	node->encoding = n->encoding;

	len = strlen(n->subtype);
	node->subtype = XMALLOC(char, len + 1);
	strncpy(node->subtype, n->subtype, len);
	node->subtype[len] = '\0';

	if (n->filename != NULL) {
	    len = strlen(n->filename);
	    node->filename = XMALLOC(char, len + 1);
	    strncpy(node->filename, n->filename, len);
	    node->filename[len] = '\0';
	}

	len = n->bodylen;
	node->data = XMALLOC(char, len + 1);
	/* Remove final "\r\n" that separated the boundary from the body. */
	if (n->body[len-1] == '\n' && n->body[len-2] == '\r') 
	    len -= 2;
	
	memcpy(node->data, n->body, len);
	node->data[len] = '\0';
	node->size = len;
	node->truncated = n->truncated;
	
	if (debug)
	    fprintf(stderr,"N: Field: %s, Type: %d, Subtype: %s\n",n->name, n->type, n->subtype);
	insertQueryStringNode(node);
	node = NULL;
    }

    /* Free the MIME structures. */
    mimeFreeMimeMessage(mime);

    if (trunc)
	return GCGITRUNCATED;
    else
	return GCGISUCCESS;
}


void 
decodeUrl(QueryStringNode *qstring)
{
    char *f, *d;
    int  flen, dlen;

    gcgiDecodeUrlEncodedString(qstring->field, &f, &flen);
    strncpy(qstring->field, f, flen);
    qstring->field[flen-1] = '\0';

    gcgiDecodeUrlEncodedString(qstring->data, &d, &dlen);
    strncpy(qstring->data, d, dlen);
    qstring->data[dlen-1] = '\0';

    XFREE(d);
    XFREE(f);
}


gcgiReturnType  
gcgiSendContentType(char *mimeType, char *name, char *charset, HTTPHeader header)
{
    fprintf(gcgiOut, "Content-Type: %s", mimeType);

    if ( (charset!= NULL) && (strlen(charset) > 0) )
	fprintf(gcgiOut, "; charset=\"%s\"", charset);

    if ( (name!= NULL) && (strlen(name) > 0) )
	fprintf(gcgiOut, "; name=\"%s\"", name);

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendContentDisp(MimeDisposition disp, char *filename, HTTPHeader header)
{
    char *strDisp[] = { "inlined",
			"attachment",
			"formdata"    };

    fprintf(gcgiOut, "Content-Disposition: %s", strDisp[disp]);

    if ( (filename!= NULL) && (strlen(filename) > 0) )
	fprintf(gcgiOut, "; filename=\"%s\"", filename);

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendContentLength(int length, HTTPHeader header)
{
    fprintf(gcgiOut, "Content-Length: %d", length);

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendLocation(char *redirectURL)
{
    fprintf(gcgiOut, "Location: %s\r\n\r\n", redirectURL);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendStatus(int status, char *message)
{
    fprintf(gcgiOut, "HTTP/1.1 %d %s\r\n\r\n", status, message);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendCacheControl(char *cache, HTTPHeader header)
{
    fprintf(gcgiOut, "Cache-Control: %s", cache);

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiSendCookie(char *name, char *value, char *path, char *domain, char *expires, int secure, HTTPHeader header)
{
    char *cookieEncoded;

    gcgiEncodeBaseSixtyFourString(value, strlen(value), &cookieEncoded);
    
    fprintf(gcgiOut,
	    "Set-Cookie: %s=%s; path=%s; domain=%s;", 
	    name, cookieEncoded, path, domain);

    if (expires && strlen(expires) > 0)
	fprintf(gcgiOut, " expires=%s;", expires);

    if (secure)
	fprintf(gcgiOut, " secure");

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    XFREE(cookieEncoded);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiFetchCookies(char ***cookies)
{
    tokenizeString(cgiQuery->env[gcgiHttpCookie], strlen(cgiQuery->env[gcgiHttpCookie]), cookies);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiParseCookie(char *cookie, char **name, char **value)
{
    char *encoded, *decoded;
    int   size;

    parseToken(cookie, name, &encoded);

    gcgiDecodeBaseSixtyFourString(encoded, &decoded, &size);

    size++;
    decoded = XREALLOC(char, decoded, size);
    decoded[size-1] = '\0';

    *value = decoded;
    XFREE(encoded);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiFreeCookies(char **cookies)
{
    freeStringArray(cookies);

    return GCGISUCCESS;
}


#ifdef USE_SSL
gcgiReturnType  
gcgiSendEncryptedCookie(char *name, char *value, char *path, char *domain, char *expires, 
			int secure, unsigned char *key, HTTPHeader header)
{
    char          *cookieEncoded;
    char          *ciphertext, *digestEncoded;
    unsigned char *digest;
    int            ctlen, dlen;

    ciphertext    = NULL;
    digest        = NULL;
    digestEncoded = NULL;
    ctlen         = 0;
    dlen          = 0;

    encryptString(value, strlen(value), key, &ciphertext, &ctlen);
    generateStringHMAC(ciphertext, ctlen, &digest, &dlen);
    gcgiEncodeBaseSixtyFourString(ciphertext, ctlen, &cookieEncoded);
    gcgiEncodeBaseSixtyFourString(digest, dlen, &digestEncoded);
    
    fprintf(gcgiOut,
	    "Set-Cookie: %s=%s&%s; path=%s; domain=%s;", 
	    name, cookieEncoded, digestEncoded, path, domain);

    if (expires && strlen(expires) > 0)
	fprintf(gcgiOut, " expires=%s;", expires);

    if (secure)
	fprintf(gcgiOut, " secure");

    if (header == LAST)
	fprintf(gcgiOut, "\r\n");

    fprintf(gcgiOut, "\r\n");

    XFREE(cookieEncoded);
    XFREE(ciphertext);
    XFREE(digest);
    XFREE(digestEncoded);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiParseEncryptedCookie(char *cookie, unsigned char *key, char **name, char **value)
{
    char          **tokens;
    int             size;
    char           *ciphertext, *cdigest, *encoded, *decoded;
    unsigned char  *digest;
    int             ctlen, dlen, cdlen;

    ciphertext = NULL;
    digest     = NULL;
    cdigest    = NULL;
    ctlen      = 0;
    dlen       = 0;
    cdlen      = 0;

    parseToken(cookie, name, &encoded);

    tokenizeURLString(encoded, strlen(encoded), &tokens);
    gcgiDecodeBaseSixtyFourString(tokens[0], &ciphertext, &ctlen);
    decryptString(ciphertext, ctlen, key, &decoded, &size);
    gcgiDecodeBaseSixtyFourString(tokens[1], &cdigest, &cdlen);
    generateStringHMAC(ciphertext, ctlen, &digest, &dlen);

    if (dlen != cdlen && strncmp(digest, cdigest, dlen) != 0)
	return GCGIBADDATA;

    size++;
    decoded = XREALLOC(char, decoded, size);
    decoded[size-1] = '\0';

    *value = decoded;

    freeStringArray(tokens);
    XFREE(encoded);
    XFREE(ciphertext);
    XFREE(digest);
    XFREE(cdigest);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiGenerateKey(unsigned char **key)
{
    *key = generateKey();

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiWriteKeyToFile(unsigned char *key, char *path)
{
    if (writeKeyToFile(key, path) < 0)
	return GCGIFATALERROR;

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiReadKeyFromFile(char *path, unsigned char **key)
{
    if (readKeyFromFile(path, key) < 0)
	return GCGIFATALERROR;

    return GCGISUCCESS;
}
#endif


gcgiReturnType
gcgiNumFormFields(int *ret)
{
    *ret = cgiQuery->queryCount;

    return GCGISUCCESS;
}


gcgiReturnType
gcgiNumFields(char *field, int *ret)
{
    int i;
    
    if (findQueryStringColl(field, &i) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = cgiQuery->query[i]->num;
    
    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchInteger(char *field, int *ret, int defaultRet)
{
    int i;
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = defaultRet;
	return GCGIFIELDNOTFOUND;
    }

    if (qs->size <= 1 && qs->data[0] == '\0') {
	*ret = defaultRet;
	return GCGIFIELDEMPTY;
    }

    i = strtol(qs->data,NULL,10);

    if (errno == ERANGE) {
	*ret = defaultRet;
	return GCGIBADDATA;
    }
    else
	*ret = i;

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchIntegerNext(char *field, int *ret, int defaultRet)
{
    int i;
    QueryStringNode *qs;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = defaultRet;
	return GCGIFIELDNOTFOUND;
    }

    if (qs->size <= 1 && qs->data[0] == '\0') {
	*ret = defaultRet;
	return GCGIFIELDEMPTY;
    }

    i = strtol(qs->data,NULL,10);

    if (errno == ERANGE) {
	*ret = defaultRet;
	return GCGIBADDATA;
    }
    else
	*ret = i;

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchDouble(char *field, double *ret, double defaultRet)
{
    double i;
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = defaultRet;
	return GCGIFIELDNOTFOUND;
    }

    if (qs->size <= 1 && qs->data[0] == '\0') {
	*ret = defaultRet;
	return GCGIFIELDEMPTY;
    }

    i = strtod(qs->data, NULL);

    if (errno == ERANGE) {
	*ret = defaultRet;
	return GCGIBADDATA;
    }
    else
	*ret = i;

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchDoubleNext(char *field, double *ret, double defaultRet)
{
    double i;
    QueryStringNode *qs;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = defaultRet;
	return GCGIFIELDNOTFOUND;
    }

    if (qs->size <= 1 && qs->data[0] == '\0') {
	*ret = defaultRet;
	return GCGIFIELDEMPTY;
    }

    i = strtod(qs->data, NULL);

    if (errno == ERANGE) {
	*ret = defaultRet;
	return GCGIBADDATA;
    }
    else
	*ret = i;

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldLength(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = strlen(qs->data);
    
    if (*ret == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldLengthCur(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNodeCur(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = strlen(qs->data);

    if (*ret == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldLengthNext(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = strlen(qs->data);

    if (*ret == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldSize(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = qs->size;
    
    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldSizeCur(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNodeCur(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = qs->size;
    
    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFieldSizeNext(char *field, int *ret)
{
    QueryStringNode *qs;
    
    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }

    *ret = qs->size;
    
    return GCGISUCCESS;
}


char *
gcgiFetchEnvVar(int env)
{
    return cgiQuery->env[env];
}


gcgiReturnType 
gcgiFetchString(char *field, char *ret, int max)
{
    int i;
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	ret[0] = '\0';
	return GCGIFIELDNOTFOUND;
    }
    
    i = strlen(qs->data);

    if (i >= max) 
	i = max-1;
    
    strncpy(ret, qs->data, i);
    ret[i] = '\0';

    if (i == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchStringNext(char *field, char *ret, int max)
{
    int i;
    QueryStringNode *qs;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	ret[0] = '\0';
	return GCGIFIELDNOTFOUND;
    }
  
    i = strlen(qs->data);
    if (i >= max) 
	i = max-1;
    
    strncpy(ret, qs->data, i);
    ret[i] = '\0';

    if (i == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType  
gcgiFetchStringNoNewLines(char *field, char *ret, int max)
{
    int len,i,j;
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	ret[0] = '\0';
	return GCGIFIELDNOTFOUND;
    }
  
    len = strlen(qs->data);
    if (len >= max) 
	len = max-1;

    for (j = 0, i = 0; i <= len && qs->data[j] != '\0'; j++, i++) 
	if (qs->data[j] == '\r' && qs->data[j+1] == '\n') {
	    ret[i] = ' ';
	    j++;
	}
	else if (qs->data[j] == '\n') {
	    ret[i] = ' ';
	}
	else
	    ret[i] = qs->data[j];
    
    if (i <= len)
	ret[i] = '\0';
    else
	ret[len] = '\0';
    
    if (len == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType  
gcgiFetchStringNoNewLinesNext(char *field, char *ret, int max)
{
    int len,i,j;
    QueryStringNode *qs;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND) {
	ret[0] = '\0';
	return GCGIFIELDNOTFOUND;
    }
  
    len = strlen(qs->data);
    if (len >= max) 
	len = max-1;

    for (j = 0, i = 0; i <= len && qs->data[j] != '\0'; j++, i++) 
	if (qs->data[j] == '\r' && qs->data[j+1] == '\n') {
	    ret[i] = ' ';
	    i++;
	    j += 2;
	}
	else if (qs->data[j] == '\n') {
	    ret[i] = ' ';
	    j++;
	}
	else
	    ret[i] = qs->data[j];
    
    if (i <= len)
	ret[i] = '\0';
    else
	ret[len] = '\0';
    
    if (i == 0)
	return GCGIFIELDEMPTY;
    else
	return GCGISUCCESS;
}


gcgiReturnType
gcgiFetchData(char *field, char *ret, int max, MimeType *type, 
	      char **subtype, MimeEncoding *encoding, char **filename, int *truncated)
{
    size_t i;
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) 
	return GCGIFIELDNOTFOUND;
  
    if (qs->truncated)
	*truncated = 1;
    else
	*truncated = 0;

    i = qs->size;
    if (i >= max) 
	i = max;
    
    if (i == 0)
	return GCGIFIELDNOTFOUND;

    memcpy(ret, qs->data, i);

    *type     = qs->type;
    *encoding = qs->encoding;

    if (subtype != NULL) {
	*subtype = XMALLOC(char, strlen(qs->subtype)+1);
	strcpy(*subtype, qs->subtype);
    }

    if (filename != NULL) {
	*filename = XMALLOC(char, strlen(qs->filename)+1);
	strcpy(*filename, qs->filename);
    }

    return GCGISUCCESS;
}


gcgiReturnType
gcgiFetchDataNext(char *field, char *ret, int max, MimeType *type, 
		  char **subtype, MimeEncoding *encoding, char **filename, int *truncated)
{
    size_t i;
    QueryStringNode *qs;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND)
	return GCGIFIELDNOTFOUND;
  
    i = qs->size;
    if (i >= max) 
	i = max;
    
    if (i == 0)
	return GCGIFIELDNOTFOUND;

    memcpy(ret, qs->data, i);

    *type     = qs->type;
    *encoding = qs->encoding;

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiFetchMultipleString(char *field, char ***data)
{
    int i, j;
    QueryStringNode *qs;
    QueryStringColl *qsc;

    if (findQueryStringColl(field, &i) == GCGIFIELDNOTFOUND) {
	*data = NULL;
	return GCGIFIELDNOTFOUND;
    }
    
    qsc = cgiQuery->query[i];

    /* Malloc the array of char pointers. */
    *data = XMALLOC(char *, qsc->num + 1);

    (*data)[qsc->num] = '\0';

    for (qs = qsc->beg, j = 0; qs != NULL; qs = qs->next, j++) {
	(*data)[j] = XMALLOC(char, qs->size);
	strncpy((*data)[j], qs->data, qs->size - 1);
	(*data)[j][qs->size - 1] = '\0';
    }

    return GCGISUCCESS;
}


gcgiReturnType
gcgiFreeMultipleString(char **data)
{
    int i; 

    if (data == NULL)
	return GCGISUCCESS;

    for (i = 0; data[i] != NULL; i++)
	XFREE(data[i]);

    XFREE(data);
    return GCGISUCCESS;
}


gcgiReturnType  
gcgiResetMultipleField(char *field)
{
    int i;

    if (findQueryStringColl(field, &i) == GCGIFIELDNOTFOUND) 
	return GCGIFIELDNOTFOUND;
    
    cgiQuery->query[i]->cur = cgiQuery->query[i]->beg;
    
    return GCGISUCCESS;
}


gcgiReturnType
gcgiFetchSelectIndex(char *field, char **data, int size, int *ret, int defaultVal)
{
    QueryStringNode *qs;
    int i;

    *ret = -1;

    if (findQueryStringNode(field, &qs) == GCGIFIELDNOTFOUND)
	return GCGIFIELDNOTFOUND;

    for (i = 0; data[i] != NULL && i < size; i++) 
	if (strncmp(data[i],qs->data,qs->size) == 0)
	    *ret = i;
    
    if (*ret == -1) {
	*ret = defaultVal;
	return GCGIFIELDNOTFOUND;
    }

    return GCGISUCCESS;
}


gcgiReturnType   
gcgiFetchCheckbox(char *field, int *ret)
{
    QueryStringNode *qs;

    if (findQueryStringNodeFirst(field, &qs) == GCGIFIELDNOTFOUND) {
	*ret = 0;
	return GCGIFIELDNOTFOUND;
    }
    
    *ret = 1;

    return GCGISUCCESS;
}


gcgiReturnType
gcgiFetchMultipleCheckbox(char *field, char **data, int size, int **ret)
{
    QueryStringNode *qs;
    int i;

    for (i = 0; data[i] != NULL && i < size; i++) {
	if (findQueryStringNodeByData(field, data[i], &qs) == GCGIFIELDNOTFOUND)
	    (*ret)[i] = 0;
	else
	    (*ret)[i] = 1;
    }
    
    return GCGISUCCESS;
}


gcgiReturnType  
gcgiLoadEnvVariables(char *path)
{
    FILE *envfile;
    char  line[256], *p;
    char *input;
    int   ilen, len;
    int   isize;

    input = NULL;

    if ((envfile = fopen(path,"r")) == NULL) 
	return GCGIFATALERROR;
 
    while (!feof(envfile)) {
	/* Allocate a new string */
	ilen  = 0;
	isize = 256;
	input = XMALLOC(char, isize);
	memset(input, 0, isize);

	do {
	    fgets(line, 256, envfile);
	    len = strlen(line);
	    if (ilen + len <= isize - 1) {
		strcat(input, line);
		ilen += len;
	    }
	    else {
		isize = isize * 2;
		ilen += len;
		input = XREALLOC(char, input, isize);
		strcat(input, line);
	    }	    	    
	} while (!feof(envfile) && input[ilen-1] != '\n');

	/* Remove the newline. */
	for (p = input; *p != '\n' && p < (input+ilen); p++);
	if (*(p-1) == '\r') *p = '\0';
	if (*p     == '\n') *p = '\0';

	/* String becomes part of the environment */
	if (putenv(input) < 0) {
	    fclose(envfile);
	    XFREE(input);
	    return GCGIFATALERROR;
	}
    }

    fclose(envfile);
    XFREE(input);

    return GCGISUCCESS;
}


gcgiReturnType 
gcgiSaveEnvVariables(char *path)
{
    FILE *envfile;
    int i;

    if ((envfile = fopen(path,"w")) == NULL) {
	fprintf(stderr, "Unable to open file to save environment variables to: %s\n", path);	
	return GCGIFATALERROR;
    }
  
    for (i = 0; i < ENVCOUNT; i++) {
	if (cgiQuery->env[i] != NULL)
	    fprintf(envfile, "%s=%s\n", envVars[i], cgiQuery->env[i]);
    }
    
    fclose(envfile);

    return GCGISUCCESS;
}


gcgiReturnType  
gcgiDebug(char *envVarsPath, char *cgiQueryPath)
{
    int evlen, cqlen;
    
    evlen = strlen(envVarsPath);
    cqlen = strlen(cgiQueryPath);

    debug++;
    envVariablesFile  = XMALLOC(char, evlen + 1);
    strncpy(envVariablesFile, envVarsPath, evlen);
    envVariablesFile[evlen] = '\0';

    cgiQueryFile = XMALLOC(char, cqlen + 1);
    strncpy(cgiQueryFile, cgiQueryPath, cqlen);
    cgiQueryFile[cqlen] = '\0';

    return GCGISUCCESS;
}


gcgiReturnType 
createCgiQuery( )
{
    int i;

    cgiQuery = XMALLOC(CgiQuery, 1);
  
    if ((cgiQuery->env = (char **) malloc(ENVCOUNT * sizeof(char *))) == NULL)
	return GCGIFATALERROR;

    for (i = 0; i < ENVCOUNT; i++)
	cgiQuery->env[i] = NULL;
    cgiQuery->envCount   =  0;

    /* Populate the env array. */
    getEnvVariables();
    //gcgiSaveEnvVariables();
  
    /* Set the queryStringColl pointer to NULL for now. */
    cgiQuery->query = NULL;
    
    return GCGISUCCESS;
}


gcgiReturnType 
freeCgiQuery( )
{
    int i;

    if (cgiQuery == NULL)
	return GCGISUCCESS;

    /* Free the env variables array */
    for (i = 0; i < cgiQuery->envCount; i++) 
	if (cgiQuery->env[i] != NULL) XFREE(cgiQuery->env[i]);
  
    /* Free the QueryNodeColl array */
    for (i = 0; i < cgiQuery->queryCount; i++)
	freeQueryStringColl(cgiQuery->query[i]);
    
    XFREE(cgiQuery->env);
    XFREE(cgiQuery->query);
    XFREE(cgiQuery);
    
    return GCGISUCCESS;
}


gcgiReturnType 
getEnvVariables( void )
{
    int   i, len;
    char *e;

    for (i = 0; i < ENVCOUNT; i++) {
	if ((e = getenv(envVars[i])) != NULL) {
	    len = strlen(e);
	    cgiQuery->env[i] = XMALLOC(char, len + 1);
	    strncpy(cgiQuery->env[i], e, len);
	    cgiQuery->env[i][len] = '\0';
	    cgiQuery->envCount++;
	}
	else 
	    cgiQuery->env[i] = NULL;
    }
  
    return GCGISUCCESS;
}


gcgiReturnType 
insertQueryStringNode(QueryStringNode *q)
{
    int i, res;
    QueryStringColl *qsc;

    res = findQueryStringColl(q->field, &i);

    /* A collection for this field doen not exist. */
    if (res == GCGIFIELDNOTFOUND) { 
	//qsc = cgiQuery->query[cgiQuery->queryCount];
	createQueryStringColl(&qsc);
	cgiQuery->query[cgiQuery->queryCount] = qsc;
	i = cgiQuery->queryCount;
	cgiQuery->queryCount++;
    }
    else {
	qsc = cgiQuery->query[i];
    }
    
    /* Insert the node at the end of the collection's node list. */
    if (qsc->end == NULL) {
	qsc->end = q;
	qsc->beg = q;
	qsc->cur = q;
    }
    else {
	qsc->end->next = q;
	qsc->end = q;
    }
    q->next = NULL;
    qsc->num++;
    
    return GCGISUCCESS;
}


gcgiReturnType 
findQueryStringColl(char *field, int *index)
{
    int flen, qlen, len;
    QueryStringColl **qsc;

    flen = strlen(field);
    if (cgiQuery == NULL) {
	*index = -1;
	return GCGIFIELDNOTFOUND;
    }

    qsc = cgiQuery->query;

    for (*index = 0; *index < cgiQuery->queryCount; (*index)++) {
	qlen = strlen(qsc[*index]->beg->field);          
	(qlen >= flen) ? (len = qlen) : (len = flen);

	if (! strncmp(field, qsc[*index]->beg->field, len))
	    return GCGISUCCESS;
    }

    return GCGIFIELDNOTFOUND;
}


gcgiReturnType
findQueryStringNodeFirst(char *field, QueryStringNode **q)
{
    int flen, i, res;

    flen = strlen(field);
    if (cgiQuery == NULL || cgiQuery->queryCount == 0) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
  
    res = findQueryStringColl(field, &i);
    
    if (res == GCGIFIELDNOTFOUND) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
    
    if (cgiQuery->query[i]->beg != NULL)
	*q = cgiQuery->query[i]->beg;
    else {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
  
    return GCGISUCCESS;
}


gcgiReturnType 
findQueryStringNode(char *field, QueryStringNode **q) 
{
    int flen, i, res;

    flen = strlen(field);
    if (cgiQuery == NULL || cgiQuery->queryCount == 0) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
  
    res = findQueryStringColl(field, &i);
    
    if (res == GCGIFIELDNOTFOUND) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
    
    if (cgiQuery->query[i]->cur != NULL)
	*q = cgiQuery->query[i]->cur;
    else {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
  
    /* Return the current item in the list and then increment it. */
    if (cgiQuery->query[i]->cur->next != NULL)
	cgiQuery->query[i]->cur = cgiQuery->query[i]->cur->next;
    else
	cgiQuery->query[i]->cur = NULL;
    
    return GCGISUCCESS;
}


gcgiReturnType 
findQueryStringNodeCur(char *field, QueryStringNode **q) 
{
    int flen, i, res;

    flen = strlen(field);
    if (cgiQuery == NULL || cgiQuery->queryCount == 0) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
  
    res = findQueryStringColl(field, &i);
    
    if (res == GCGIFIELDNOTFOUND) {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
    
    /* Return the current item in the list but don't increment it. */
    if (cgiQuery->query[i]->cur != NULL)
	*q = cgiQuery->query[i]->cur;
    else {
	*q = NULL;
	return GCGIFIELDNOTFOUND;
    }
    
    return GCGISUCCESS;
}


gcgiReturnType
findQueryStringNodeByData(char *field, char *data, QueryStringNode **q)
{
    QueryStringNode *qs;
    int i;

    if (findQueryStringColl(field, &i) == GCGIFIELDNOTFOUND)
	return GCGIFIELDNOTFOUND;

    for (qs = cgiQuery->query[i]->beg;
	 qs != NULL && strncmp(qs->data, data, qs->size) != 0;
	 qs = qs->next);

    if (qs != NULL) {
	*q = qs;
	return GCGISUCCESS;
    }
    else
	return GCGIFIELDNOTFOUND;
}


gcgiReturnType 
createQueryStringNode(QueryStringNode **q)
{
    *q = XMALLOC(QueryStringNode, 1);
    (*q)->next      = NULL;
    (*q)->field     = NULL;
    (*q)->data      = NULL;
    (*q)->subtype   = NULL;
    (*q)->filename  = NULL;
    (*q)->size      = 0;
    (*q)->encoding  = sevenbit;
    (*q)->type      = text;

    return GCGISUCCESS;
}


gcgiReturnType 
freeQueryStringNode(QueryStringNode *q)
{
    if (q->data != NULL)    XFREE(q->data);
    if (q->field != NULL)   XFREE(q->field);
    if (q->subtype != NULL) XFREE(q->subtype);
    if (q->subtype != NULL) XFREE(q->filename);
    q->next = NULL;
    
    XFREE(q);
  
    return 0;
}


gcgiReturnType
createQueryStringColl(QueryStringColl **q)
{
    *q = XMALLOC(QueryStringColl, 1);

    (*q)->num = 0;
    (*q)->beg = NULL;
    (*q)->end = NULL;
    (*q)->cur = NULL;

    return GCGISUCCESS;
}


gcgiReturnType
createQueryStringCollArray(int numColls)
{
    int i;

    /* Malloc the query[] array and clear it. */
    if ((cgiQuery->query = (QueryStringColl **) malloc(numColls * sizeof(QueryStringColl *))) == NULL)
	return GCGIFATALERROR;
    
    for (i = 0; i < numColls; i++) 
	cgiQuery->query[i] = NULL;

    cgiQuery->queryCount = 0;

    return GCGISUCCESS;
}


gcgiReturnType
freeQueryStringColl(QueryStringColl *q)
{
    QueryStringNode *m, *n;
    
    for (m = n = q->beg; n != NULL; m = n) {
	n = n->next;
	freeQueryStringNode(m);
    }

    q->beg = NULL;
    q->end = NULL;
    q->cur = NULL;

    XFREE(q);
    
    return GCGISUCCESS;
}


gcgiReturnType 
printQueryStringColl(QueryStringColl *q, FILE *stream)
{
    QueryStringNode *node;

    for (node = q->beg; node != NULL; node = node->next) {
	printQueryStringNode(node, stream);
    }

    return GCGISUCCESS;
}


gcgiReturnType 
printQuery(FILE *stream)
{
    int i;
  
    for (i = 0; i < cgiQuery->queryCount; i++) {
	fprintf(stream, "QueryString Collection #%d:\n", i);
	printQueryStringColl(cgiQuery->query[i], stream);
    }
    
    return GCGISUCCESS;
}


gcgiReturnType 
printQueryStringNode(QueryStringNode *q, FILE *stream)
{
    fprintf(stream, "     %s: %s\n", q->field, q->data);
  
    return GCGISUCCESS;
}
