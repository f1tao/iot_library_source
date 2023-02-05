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

#include "parse.h"

int 
numOccurances(char *string, char *pattern)
{
    char *pin;
    int patternLen;
    int count = 0;

    patternLen = strlen(pattern);

    for (pin=string; *pin != '\0'; pin++) {
	if (*pin == pattern[0]) {
	    if(!strncasecmp(pin,pattern,patternLen)) {
		pin += patternLen-1;
		count++;
	    }
	}
    } 

    return count;
}

int 
replacePat(char **string, char *pattern, char *replace, int *size)
{
    char *pin, *pout, *output;
    int  stringLen, patternLen, replaceLen;
    int  osize;
    int  count;

    pattern ? (patternLen = strlen(pattern)) : (patternLen = 0);
    replace ? (replaceLen = strlen(replace)) : (replaceLen = 0);
    *string ? (stringLen  = strlen(*string)) : (stringLen  = 0);

    //DEBUG("START LEN: %d, START SIZE: %d",strlen(*string), *size);
    
    /* Count up the number of occurances of the pattern to  *
     * determine how big to make the new string.            */
    count = numOccurances(*string, pattern);

    if (count < 1) {
	//DEBUG("Pattern does not occur");
	return 0;
    }
    
    if (replaceLen <= patternLen)
	osize  = stringLen + 1;
    else 
	osize  = stringLen + ((replaceLen - patternLen) * count) + 1;
    
    //DEBUG("OUTPUT SIZE: %d", osize);

    output = XMALLOC(char, osize);
    memset(output, 0, osize);

    count = 0; 
    pin   = *string; 
    pout  = output;

    while (*pin != '\0') {
	if (*pin == pattern[0] && (strncasecmp(pin, pattern, patternLen) == 0)) {
	    strncpy(pout, replace, replaceLen);
	    pin  += patternLen;
	    pout += replaceLen; 
	    count++;
	}
	else {
	    *pout = *pin;
	    pout++; 
	    pin++;
	}
    }

    *pout = '\0';

    if (osize > *size) {
	*size = osize;
	*string = XREALLOC(char, *string, *size);
    }

    strncpy(*string, output, pout - output);
    (*string)[pout - output] = '\0';

    XFREE(output);

    return count;
}


int 
removePat(char *string, char *pattern, int stringLen)
{
  char *pin, *pout, *out;
  int   patternLen;
  int   count;

  count = 0;
  pattern ? (patternLen = strlen(pattern)) : (patternLen = 0);

  out = XMALLOC(char, stringLen + 1);

  for (pin = string, pout = out; *pin != '\0'; pout++, pin++)  {     
      if (*pin == pattern[0] && (strncasecmp(pin, pattern, patternLen) == 0)) {
	  pin += patternLen;
	  *pout = *pin;
	  count++; 
      }
      else {
	  *pout = *pin;
      }
  } 

  *pout = '\0';

  if (count)
      strcpy(string, out);
  
  XFREE(out);

  return count;
}


int 
regexPat(char *string, char *pattern, int stringLen)
{
    int         res, outLen, limit;
    int         count = 0;
    int         nmatch = 1;
    char       *pout, *sptr, *eptr;
    regex_t    *rx;
    regmatch_t  rm[1];
    
    /* Calculate a limit to ensure we stop regex */
    limit = floor(stringLen / 5);
    
    rx = XMALLOC(regex_t, 1);
    res = regcomp(rx, pattern, REG_ICASE);  
    if (res != 0) 
	return -1;
    
    res = regexec(rx, string, nmatch, rm, 0);
    
    if (res == REG_NOMATCH)     /* Didn't find the pattern. */      
	return 0;
    
    else if (res == REG_ESPACE) /* Regex ran out of memory. */
	return -1;
    
    pout = XMALLOC(char, stringLen);
    
    count++;
    sptr = string + rm[0].rm_so;
    eptr = string + rm[0].rm_eo;
    strncpy(pout, string, sptr-string);
    *(pout+(sptr-string)) = '\0';
    
    strcat(pout, eptr);
    outLen = strlen(pout);
    strcpy(string, pout);
    *pout = '\0';
    
    /* Loop until we no longer find the pattern (or hit the safety limit) */
    while (((res = regexec(rx,string,nmatch,rm,0)) != REG_NOMATCH) && (count < limit)) {
	if (res == REG_ESPACE) /* Regex ran out of memory. */
	    return -1;
	
	count++;
	sptr = string + rm[0].rm_so;        /* Beginning of pattern.       */
	eptr = string + rm[0].rm_eo;        /* End of pattern.             */
	strncpy(pout, string, sptr-string); /* Copy the first half.        */
	*(pout+(sptr-string)) = '\0';       /* Zero out the partial string *
					     * so strcat works properly.   */
	strcat(pout, eptr);                 /* Add on the second half.     */
	outLen = strlen(pout);            
	strcpy(string, pout);               /* Copy back to our original string */
	*pout = '\0';
    }
    
    regfree(rx);
    XFREE(rx);
    
    XFREE(pout);
    return count;
}


void 
insertString(char **string, char *insert, int offset, int *size)
{
  char *p, *len, *tmp;
  int  insertLen, stringLen, tmpLen;

  len = *string + offset; 
  stringLen = strlen(*string);
  insertLen = strlen(insert);

  tmp = XMALLOC(char, (stringLen+insertLen+1));
  for (p = *string; p < len; p++);

  strncpy(tmp, *string, (p - *string));
  tmp[(p - *string)] = '\0';
  strcat(tmp,insert);
  strcat(tmp,p);
  tmpLen = strlen(tmp);
  
  if (tmpLen >= *size) {
    *size = tmpLen * 2;    /* Double the string size */  
    *string = XREALLOC(char, *string, (*size));
  }

  strcpy(*string,tmp);

  XFREE(tmp);
}


/* Return a NULL-terminated array of strings. */
int
tokenizeString(char *string, int size, char ***output)
{
    char  *start, *end;
    char  *p, *q;
    int   i;

    end = string + size;
    
    /* Remove spaces from end of string */
    for (p = string; p < end && *p != '\0'; p++);
    for (p--; isspace((int)*p); p--);
    end = p + 1;

    /* Remove spaces from beginning of string */
    for (p = string; p < end && isspace((int)*p); p++);
    start = p;

    /* If start == end, then we have an empty string. */
    if (start == end) {
	*output = XMALLOC(char *, 1);
	(*output)[0] = '\0';
	return 0;
    }

    /* Count number of tokens */
    for (p = end-1, i = 0; p > start; p--) {
	/* Treat quoted text as opaque, do not count spaces. */
	if (*p == '\'' || *p == '"') {                        
	    for (p--; *p != '\'' && *p != '"' && p > start; p--);
	}                                                     
	if (isspace((int)*p) && !isspace((int)*(p-1))) {
	    i++;                        
	}
    }
    
    /* Number of tokens equals spaces + 1 */
    i++;
    
    *output = XMALLOC(char *, i + 1);
    
    (*output)[i] = '\0';

    /* Copy tokens to the char array */
    for (i = 0, p = q = start; p < end && q < end; p = q, i++) {
	for (q = p; !isspace((int)*q) && q < end; q++) {
	    
	    /* If we hit a quote, forward to the end of the quote *
	     * i.e. treat quoted text as an opaque token.         */
	    if (*q == '\'' || *q == '"') {
		for (q++; *q != '\'' && *q != '"' && q < end; q++);
	    }
	}      
	
	/* Copy the token to the string array */
	(*output)[i] = XMALLOC(char, (q - p + 1));
	strncpy((*output)[i], p, q-p);
	(*output)[i][q-p] = '\0';
	
	/* Forward past any extra spaces between tokens */
	for (q++; isspace((int)*q) && q < end; q++);
    }
    
    return 0;
}


int
tokenizeURLString(char *string, int size, char ***output)
{
    char  *start, *end;
    char  *p, *q;
    int   i;

    end = string + size;
    
    /* Remove spaces from end of string */
    for (p = string; p < end && *p != '\0'; p++);
    for (p--; isspace(*p); p--);
    end = p + 1;

    /* Remove spaces from beginning of string */
    for (p = string; p < end && isspace(*p); p++);
    start = p;

    /* If start == end, then we have an empty string. */
    if (start == end) {
	*output = XMALLOC(char *, 1);
	(*output)[0] = '\0';
	return 0;
    }

    /* Count number of tokens */
    for (p = end-1, i = 0; p > start; p--) {
	if (*p == '&') 
	    i++;
    }
    
    /* Number of tokens equals spaces + 1 */
    i++;
    
    *output = XMALLOC(char *, i + 1);
    
    (*output)[i] = '\0';

    /* Copy tokens to the char array */
    for (i = 0, p = q = start; p < end && q < end; p = q, i++) {

	for (q = p; *q != '&' && q < end; q++);     
	
	/* Copy the token to the string array */
	(*output)[i] = XMALLOC(char, (q - p + 1));
	strncpy((*output)[i], p, q-p);
	(*output)[i][q-p] = '\0';

	if (q < end) q++;
    }
    
    return 0;
}


/* Free a NULL-terminated array of character strings. */
int
freeStringArray(char **string)
{
    int i = 0;

    if (string == NULL)
	return 0;

    while (string[i] != '\0') {
	XFREE(string[i]);
	i++;
    }

    XFREE(string);

    return 0;
}


/* Parse a token, such as 'charset=ISO-8859-1' *
 * or 'filename="pic17958.pcx"' or 'inline;'   */
int
parseToken(char *token, char **property, char **value)
{
    char *start, *end;
    char *p, *q;
    int   tlen;

    token? (tlen = strlen(token)) : (tlen = 0);
    end = token + tlen;

    /* Return if string is empty or NULL. */
    if (tlen == 0) {
	*property = NULL;
	*value    = NULL;
	return 0;
    }

    /* Remove spaces from end of string */
    for (p = token; p < end && *p != '\0'; p++);
    for (p--; isspace((int)*p); p--);
    end = p + 1;

    /* Remove spaces from beginning of string */
    for (p = token; p < end && isspace((int)*p); p++);
    start = p;

    /* Parse property. If no '=' found then treat *
     * the entire token as the value.             */
    for (p = start; *p != '=' && p < end; p++);
    if (p == end) {
	(*property) = NULL;
	p = start;
    }
    else {
	(*property) = XMALLOC(char, p - start + 1);
	strncpy(*property, start, p - start);
	(*property)[p-start] = '\0';
    }
    
    /* Parse the value */
    (p == start)? (q = p) : (q = p + 1);
    for (; p < end; p++);
    
    /* Remove semi-colon if it is at the end of the value. */
    if (*(p-1) == ';')
	p--;

    /* Remove quotes if they surround the value */
    if (*q == '"' || *q =='\'')
	q++;
    if (*(p-1) == '"' || *(p-1) =='\'')
	p--;
    (*value) = XMALLOC(char, p - q + 1);
    strncpy(*value, q, p - q);
    (*value)[p-q] = '\0';
    
    return 0;
}


/* Read a line in from the file stream and check to make  *
 * sure that the entire string was read. The string is    *
 * terminated by "\r\n" (CRLF). If the character array is *
 * too short, make it bigger.                             */
int
readCRLFLine(FILE *input, char **line, int *size)
{
    off_t pos;
    char  buf[256];
    int   blen, llen;

    pos = ftell(input);
    memset(*line, 0, *size);   
    llen = 0;

    /* Make sure we read the entire line. */
    do {	    
	fgets(buf, sizeof(buf), input);
	blen = strlen(buf);
	if (blen + llen <= (*size) - 1) {
	    strcat(*line, buf);
	    llen += blen;
	}
	else {
	    *size *= 2;
	    llen  += blen;
	    *line  = XREALLOC(char, *line, *size);
	    strcat(*line, buf);
	}
    } while (buf[blen - 1] != '\n'&& !feof(input));
    
    return 0;
}
