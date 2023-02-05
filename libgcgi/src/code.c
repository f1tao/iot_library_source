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

static char hextochar(char *hex);
static void chartohex(char c, char *hex, int size);
static void plustospace(char *string);

/*------ Base64 Encoding Table ------*/
static const char mimeBase64[] = { 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/' 
};


gcgiReturnType 
gcgiDecodeBaseSixtyFourString(char *text, char **decodedText, int *numBytes)
{
  int  len, j, k, index;
  char *p, *plen;
  unsigned char input[4]  = {0,0,0,0};
  unsigned char output[3] = {0,0,0};
  
  /* Find the end of the string. Remove '\r', '\n', and '='. */
  for (plen = text; *plen != '=' && *plen != '\r' && *plen != '\n' && *plen != '\0'; plen++);
  len          = plen - text - 1;
  *numBytes    = (3 * (len / 4)) + (len % 4);
  *decodedText = XMALLOC(char, *numBytes);
  k            = 0;
  plen--;

  for (p = text, j = 0; p <= plen; p++, j++) {
    index = j % 4;    

    if (*p >= 'A' && *p <= 'Z')
      input[index] = *p-65;
    else if (*p >= 'a' && *p <= 'z')
      input[index] = *p-71;
    else if (*p >= '0' && *p <= '9')     
      input[index] = *p+4;
    else if (*p == '+')
      input[index] = 62;
    else if (*p == '/')
      input[index] = 63;
 
    if (index == 3 || p == plen) {
      output [0] = (input [0] << 2)          | ((input [1] & 0x30) >> 4);
      output [1] = ((input [1] & 0x0F) << 4) | ((input [2] & 0x3C) >> 2);
      output [2] = ((input [2] & 0x03) << 6) | (input [3] & 0x3F);
      
      if (p < plen) { 
	(*decodedText)[k++] = output[0];
	(*decodedText)[k++] = output[1];
	(*decodedText)[k++] = output[2];
      }
      else {
	(*decodedText)[k++] = output[0];
	index >= 2? (*decodedText)[k++] = output[1]:0;
	index == 3? (*decodedText)[k++] = output[2]:0;
      }
      
      output[0] = output[1] = output[2] = 0;
      input[0]  = input[1]  = input[2]  = input[3] = 0;
    }
  }

  return GCGISUCCESS;
}


gcgiReturnType
gcgiEncodeBaseSixtyFourString(char *text, int numBytes, char **encodedText)
{
  unsigned char input[3]  = {0,0,0};
  unsigned char output[4] = {0,0,0,0};
  int   index, i, j, size;
  char *p, *plen;

  plen           = text + numBytes - 1;
  size           = (4 * (numBytes / 3)) + (numBytes % 3? 4 : 0) + 1;
  (*encodedText) = XMALLOC(char, size);
  j              = 0;
  
  for  (i = 0, p = text;p <= plen; i++, p++) {
    index = i % 3;
    input[index] = *p;

    if (index == 2 || p == plen) {
      output[0] = ((input[0] & 0xFC) >> 2);
      output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
      output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
      output[3] = (input[2] & 0x3F);
      
      (*encodedText)[j++] = mimeBase64[output[0]];
      (*encodedText)[j++] = mimeBase64[output[1]];
      (*encodedText)[j++] = index == 0? '=' : mimeBase64[output[2]];
      (*encodedText)[j++] = index <  2? '=' : mimeBase64[output[3]];
      
      input[0] = input[1] = input[2] = 0;
    }
  } 
  (*encodedText)[j] = '\0';

  return GCGISUCCESS;
}


gcgiReturnType 
gcgiDecodeQuotedPrintableString(char *text, char **decodedText, int *size)
{
    int  decoded, len, i;
    char *p,*q;
    char hex[3];
    
    q = p = text;
    text ? (len = strlen(text)) : (len = 0);
    *decodedText = XMALLOC (char, (len + 1));
    *size = len + 1;
    
    for (p = text, i = 0; p < (len + text); p++, i++) {
	if (*p == '=') {                    
	    q = p + 1;
	    
	    if (*q == '\n') {                      /* Soft Line Break. */
		p += 2;
		i--;
	    }
	    else if (*q == '\r') {
		p += 3;
		i--;
	    }
	    else {                                 /* Encoded Character. */
		strncpy(hex,q,2);
		hex[2] = 0;
		decoded = strtol(hex,NULL,16);
		(*decodedText)[i] = (char) decoded;
		p += 2;
	    }
	}
	else {
	    (*decodedText)[i] = (char) *p;
	}
    }
    (*decodedText)[i] = '\0';

    return GCGISUCCESS;
}


gcgiReturnType
gcgiEncodeQuotedPrintableString(char *text, char **encodedText, int *size)
{
    int len, i, columns;
    char *textlen, *p, hex[3];

    text? (len = strlen(text)) : (len = 0);
    textlen = text + len;

    (*encodedText) = XMALLOC(char, (len * 3) + 1);
    *size          = (len * 3) + 1;
    i              = 0;

    for (p = text, columns = 0; p < textlen && *p != '\0'; p++, columns++) {
	if ((*p >= '!' && *p <= '<') || (*p >= '>' && *p <= '~'))
	    (*encodedText)[i++] = *p;
	else 
	    switch (*p) {
	    case '\n':
	      if (*(p-1) != '\r') {
		(*encodedText)[i++] = '\r';
		(*encodedText)[i++] = '\n';
	      }
	      columns = 0;
	      break;
	    case '\t':
	    case ' ' :
	      if (*(p+1) != '\r' && *(p+1) != '\n') {
		(*encodedText)[i++] = *p;
		break;
	      }	   
	    default:
		chartohex(*p, hex, 3);
		(*encodedText)[i++] = '=';
		(*encodedText)[i++] = hex[0];
		(*encodedText)[i++] = hex[1];
		break;
	    }
	if (columns == 76) {
	  (*encodedText)[i++] = '=';
	  (*encodedText)[i++] = '\r';
	  (*encodedText)[i++] = '\n';
	  columns = 0;
	}
    }
    (*encodedText)[i++] = '\0';

    return GCGISUCCESS;
}


/* Examples (From RFC247):
 * =?ISO-8859-1?Q?Andr=E9?=
 * =?ISO-8859-1?B?SWYgeW91IGNhbiByZWFkIHRoaXMgeW8=?= */ 
gcgiReturnType
gcgiDecodeRfc2047String(char *text, char **charset, char **decodedText)
{
    char *p, *q, *r, *offset;
    char *decode, *encoding;
    int  len, dlen, i, numBytes, size;
    
    text ? (len = strlen(text)) : (len = 0);
    offset       = text + len;
    *decodedText = NULL;
    *charset     = NULL;
    
    /* Do a sanity check to ensure the string is not malformed. */
    for (p = text, i = 0; p < offset; p++) { if (*p == '?') i++; }
    if (text[0] != '=' || text[len-1] != '=' || i != 4)
	return GCGIBADDATA;
    
    /* Get the charset. */
    for (p = text + 2, q = p+1; *q != '?' && q < offset; q++);
    *charset = XMALLOC(char, (q - p + 1));
    strncpy(*charset, p, q-p);
    (*charset)[q-p] = '\0';
    
    /* Get the encoding. */
    for (p = q+1, q = p+1; *q != '?' && q < offset; q++);
    encoding = XMALLOC(char, (q - p + 1));
    strncpy(encoding, p, q-p);
    encoding[q-p] = '\0';
    
    /* Get the encoded string. */
    for (p = q+1, q = p+1; *q != '?' && q < offset; q++);
    decode = XMALLOC(char, (q - p + 1));
    strncpy(decode, p, q-p);
    decode[q-p] = '\0';
    
    if (! strcasecmp(encoding,"B")) {    
	gcgiDecodeBaseSixtyFourString(decode, decodedText, &numBytes);
	(*decodedText)[numBytes] = '\0';
    }
    else if (! strcasecmp(encoding,"Q")) {    
	gcgiDecodeQuotedPrintableString(decode, decodedText, &size);
	
	/* Make one more extra pass to handle the "_" to SPACE variant 
	   that RFC2047 defines for Quoted Printable decoding. */
	dlen = strlen(*decodedText);
	for (r = *decodedText; r < (*decodedText)+dlen; r++)
	    if (*r == '_') { *r = (char) 20; }
	
    }
    else { /* Unknown Encoding */
	free(charset);
	*charset = NULL;
	return GCGIBADDATA;
    }
    
    XFREE(decode);

    return GCGISUCCESS;
}


gcgiReturnType
gcgiDecodeUrlEncodedString(char *text, char **decodedText, int *size)
{
    int   len, i;
    char *textlen, *p;

    len     = strlen(text);
    textlen = text + len;

    (*decodedText) = XMALLOC(char, len + 1);
    *size = len + 1;

    for (p = text, i = 0; p < textlen && *p != '\0'; p++, i++) 
	if (*p == '%') {
	    (*decodedText)[i] = hextochar(p+1);
	    p += 2;
	}
	else
	    (*decodedText)[i] = *p;
    (*decodedText)[i++] = '\0';

    plustospace(*decodedText);

    return GCGISUCCESS;
}


/* According to RFC2396 */
gcgiReturnType
gcgiEncodeUrlString(char *text, char **encodedText, int *size)
{
    int   len, i;
    char *textlen, *p, hex[3];

    len     = strlen(text);
    textlen = text + len;

    (*encodedText) = XMALLOC(char, (len * 3) + 1);
    *size          = (len * 3) + 1;
    i              = 0;

    for (p = text; p < textlen && *p != '\0'; p++) {
	if ((*p >= 'A' || *p <= 'Z') || (*p >= 'a' || *p <= 'z') || (*p >= '0' || *p <= '9'))
	    (*encodedText)[i++] = *p;
	else 
	    switch (*p) {
	    case '-':
	    case '_':
	    case '.':
	    case '!':
	    case '~':
	    case '*':
	    case '\'':
	    case '(':
	    case ')':
		(*encodedText)[i++] = *p;
		break;
	    default:
		chartohex(*p, hex, 3);
		(*encodedText)[i++] = '%';
		(*encodedText)[i++] = hex[0];
		(*encodedText)[i++] = hex[1];
		break;
	    }
    }	
    (*encodedText)[i++] = '\0';

    return GCGISUCCESS;
}


char 
hextochar(char *hex)
{
    int i, j;

    i = (int) hex[0];
    j = (int) hex[1];
  
    if (hex[0] >= 'A') 
	i -= 55; 
    else
	i -= 48;
    if (hex[1] >= 'A') 
	j -= 55; 
    else
	j -= 48; 

    return (char) ((16*i) + j);
}


void
chartohex(char c, char *hex, int size)
{
    int  dec, i, rem, res, j, k;
    char tmp;

    dec = (int) c;

    i = 0;
    do {
	res = dec / 16;
	rem = dec % 16;
	dec = res;

	if (rem >= 10)
	    hex[i] = rem + 'A' - 10;
	else
	    hex[i] = (char ) rem + '0';	 
	i++;
    } while (dec != 0 && i < size);

    /* Pad the string with '0' */ 
    while (i < size-1) hex[i++] = '0';
 
    hex[i] = '\0';

    /* Reverse the string. */
    for (j = 0, k = i-1; j < k; j++, k--) {
      tmp    = hex[j];
      hex[j] = hex[k];
      hex[k] = tmp;
    }
}


void 
plustospace(char *string)
{
    char *p;
    int   slen; 
    
    slen = strlen(string);

    for (p = string; p < (string+slen) && *p != 0; p++) {
	if (*p == '+') 
	    *p = ' ';
    }
}
