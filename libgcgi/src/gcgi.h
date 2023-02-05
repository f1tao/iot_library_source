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

#ifndef __GCGI_H__
#define __GCGI_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*------ Enumerations ------*/
typedef enum {
    gcgiHttpCookie = 0,
    gcgiHttpReferer,
    gcgiAuthType,
    gcgiContentLength,
    gcgiContentType,
    gcgiGatewayInterface,
    gcgiPathInfo,
    gcgiPathTranslated,
    gcgiQueryString,
    gcgiRemoteAddr,
    gcgiRemoteHost,
    gcgiRemoteIdent,
    gcgiRemoteUser,
    gcgiRequestMethod,
    gcgiScriptName,
    gcgiServerName,
    gcgiServerPort,
    gcgiServerProtocol,
    gcgiServerSoftware
} gcgiEnvVars;

/*------ Return Values ------*/
typedef enum {
    GCGIFATALERROR = -1,
    GCGISUCCESS,
    GCGIFIELDNOTFOUND,
    GCGIFIELDEMPTY,
    GCGITRUNCATED,
    GCGIBADDATA
} gcgiReturnType;


/*------ MIME types ------*/
typedef enum {
    text, 
    image, 
    audio, 
    video, 
    application, 
    multipart, 
    message, 
    unknown
} MimeType;

typedef enum {
    sevenbit, 
    eightbit, 
    binary, 
    quotedprintable, 
    basesixtyfour
} MimeEncoding;

typedef enum {
    fixed,
    flowed
} MimeFormat;

typedef enum {
    inlined,
    attachment,
    formdata
} MimeDisposition;

typedef enum {
    contentType,
    contentEncoding,
    contentDisposition,
    contentDescription,
    mimeVersion,
    unknownHeader
} MimeHeader;
	
typedef enum {
    NONE = 0,
    COOKIE,
    DISPOSITION,
    LENGTH,
    LAST
} HTTPHeader;

/*------ CGI Functions ------*/

/* Initialization Functions */
gcgiReturnType  initCgi( void );
void            freeCgi( void );
gcgiReturnType  gcgiSetLimits(size_t fieldLimit, size_t queryLimit);

/* HTTP Header Functions */
gcgiReturnType  gcgiSendContentType(char *mimeType, char *name, char *charset, HTTPHeader header);
gcgiReturnType  gcgiSendContentDisp(MimeDisposition disp, char *filename, HTTPHeader header);
gcgiReturnType  gcgiSendContentLength(int length, HTTPHeader header);
gcgiReturnType  gcgiSendCacheControl(char *cache, HTTPHeader header);
gcgiReturnType  gcgiSendLocation(char *redirectURL);
gcgiReturnType  gcgiSendStatus(int status, char *message);

/* Cookie Handling Functions */
gcgiReturnType  gcgiSendCookie(char *name, char *value, char *path, char *domain, char *expires, 
			       int secure, HTTPHeader header);
gcgiReturnType  gcgiFetchCookies(char ***cookies);
gcgiReturnType  gcgiParseCookie(char *cookie, char **name, char **value);
gcgiReturnType  gcgiFreeCookies(char **cookies);

gcgiReturnType  gcgiSendEncryptedCookie(char *name, char *value, char *path, char *domain, char *expires, 
					int secure, unsigned char *key, HTTPHeader header);
gcgiReturnType  gcgiParseEncryptedCookie(char *cookie, unsigned char *key, char **name, char **value);
gcgiReturnType  gcgiGenerateKey(unsigned char **key);
gcgiReturnType  gcgiWriteKeyToFile(unsigned char *key, char *path);
gcgiReturnType  gcgiReadKeyFromFile(char *path, unsigned char **key);

/* Data Fetching Functions */
char           *gcgiFetchEnvVar(int env);
gcgiReturnType  gcgiFetchInteger(char *field, int *ret, int defaultRet);
gcgiReturnType  gcgiFetchIntegerNext(char *field, int *ret, int defaultRet);
gcgiReturnType  gcgiFetchDouble(char *field, double *ret, double defaultRet);
gcgiReturnType  gcgiFetchDoubleNext(char *field, double *ret, double defaultRet);
gcgiReturnType  gcgiFetchString(char *field, char *ret, int max);
gcgiReturnType  gcgiFetchStringNext(char *field, char *ret, int max);
gcgiReturnType  gcgiFetchStringNoNewLines(char *field, char *ret, int max);
gcgiReturnType  gcgiFetchStringNoNewLinesNext(char *field, char *ret, int max);
gcgiReturnType  gcgiFetchData(char *field, char *ret, int max, MimeType *type, 
			      char **subtype, MimeEncoding *encoding, char **filename, int *truncated);
gcgiReturnType  gcgiFetchDataNext(char *field, char *ret, int max, MimeType *type, 
				  char **subtype, MimeEncoding *encoding, char **filename, int *truncated);
gcgiReturnType  gcgiFieldLength(char *field, int *ret);
gcgiReturnType  gcgiFieldLengthCur(char *field, int *ret);
gcgiReturnType  gcgiFieldLengthNext(char *field, int *ret);
gcgiReturnType  gcgiFieldSize(char *field, int *ret);
gcgiReturnType  gcgiFieldSizeCur(char *field, int *ret);
gcgiReturnType  gcgiFieldSizeNext(char *field, int *ret);

gcgiReturnType  gcgiNumFormFields(int *ret);
gcgiReturnType  gcgiNumFields(char *field, int *ret);

gcgiReturnType  gcgiFetchCheckbox(char *field, int *ret);
gcgiReturnType  gcgiFetchMultipleCheckbox(char *field, char **data, int size, int **ret);
gcgiReturnType  gcgiFetchMultipleString(char *field, char ***data);
gcgiReturnType  gcgiFreeMultipleString(char **data);
gcgiReturnType  gcgiFetchSelectIndex(char *field, char **data, int size, int *ret, int defaultVal);
gcgiReturnType  gcgiResetMultipleField(char *field);
gcgiReturnType  gcgiLoadEnvVariables(char *path);
gcgiReturnType  gcgiSaveEnvVariables(char *path);
gcgiReturnType  gcgiDebug(char *envVarsPath, char *cgiQueryPath);

/* Encoding/Decoding Functions */
gcgiReturnType  gcgiDecodeBaseSixtyFourString(char *text, char **decodedText, int *numBytes);
gcgiReturnType  gcgiEncodeBaseSixtyFourString(char *text, int numBytes, char **encodedText);
gcgiReturnType  gcgiDecodeQuotedPrintableString(char *text, char **decodedText, int *size);
gcgiReturnType  gcgiEncodeQuotedPrintableString(char *text, char **encodedText, int *size);
gcgiReturnType  gcgiDecodeRfc2047String(char *text, char **charset, char **decodedText);
gcgiReturnType  gcgiDecodeUrlEncodedString(char *text, char **decodedText, int *size);
gcgiReturnType  gcgiEncodeUrlString(char *text, char **encodedText, int *size);

/*------ Debugging Functions ------*/
gcgiReturnType  printQuery(FILE *stream);

/*------ Global Variables ------*/
FILE *gcgiOut;

#ifdef __cplusplus
};
#endif

#endif  /* __GCGI_H__ */
