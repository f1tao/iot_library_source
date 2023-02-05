#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "gcgi.h"

#define SETCOOKIES  1
#define READCOOKIES 2
#define STOREHEADERS 3

static const char* const strTypes[] = {
    "text", 
    "image", 
    "audio", 
    "video", 
    "application", 
    "multipart", 
    "message", 
    "unknown"
};

static const char* const strEncodings[] = {
    "7bit", 
    "8bit", 
    "binary", 
    "quoted-printable", 
    "base64", 
    "unknown"
};

int main (int argc, char *argv[], char *envp[]) 
{
  int             caseAct, i;
  char          **cookies, *name, *value;

  /* Set Limits: field limit, query limit */
  gcgiSetLimits(1048576, 0);

  //gcgiDebug("./gcgi-env", "./mimedata2");

  if (initCgi() < 0) {
    fprintf(stderr, "Unable to parse CGI query.\n");
    return -1;
  }
  
  gcgiFetchInteger("caseAct", &caseAct, -1);

  cookies = NULL;

  if (caseAct == SETCOOKIES) {

#ifdef USE_SSL
    gcgiGenerateKey(&key);
    gcgiWriteKeyToFile(key, "/tmp/gcgiCookieTest.key");
#endif

    gcgiSendContentType("text/html", NULL, "ISO-8859-1", COOKIE);

#ifdef USE_SSL
    gcgiSendEncryptedCookie("ticket1", "ThisIsTheFirstCookie", "/", "topeka.shingletowngap.org", NULL, 0, key, COOKIE);
    gcgiSendEncryptedCookie("ticket2", "ThisIsTheSecondCookie", "/", "topeka.shingletowngap.org", NULL, 0, key, LAST);
#else
    gcgiSendCookie("ticket1", "ThisIsTheFirstCookie", "/", "topeka.shingletowngap.org", NULL, 0, COOKIE);
    gcgiSendCookie("ticket2", "ThisIsTheSecondCookie", "/", "topeka.shingletowngap.org", NULL, 0, LAST);
#endif

  }
  else if (caseAct == READCOOKIES) {
#ifdef USE_SSL
    gcgiReadKeyFromFile("/tmp/gcgiCookieTest.key", &key);
    unlink("/tmp/gcgiCookieTest.key");
#endif
    gcgiSendContentType("text/html", NULL, "ISO-8859-1", LAST);
    gcgiFetchCookies(&cookies);
  }
  else if (caseAct == STOREHEADERS) {
    gcgiSaveEnvVariables("./gcgiCookieTest-output.txt");
    gcgiSendContentType("text/html", NULL, "ISO-8859-1", LAST);
  }

  fprintf(gcgiOut, 
	  "<html>\n"
	  "<head><title>gcgiCookieTest: Results</title></head>\n"
	  "<body>\n"
	  "<h1>Transaction completed successfully.</h1>\n");

  if (cookies != NULL) {
    for (i = 0; cookies[i] != '\0'; i++) {
#ifdef USE_SSL      
      gcgiParseEncryptedCookie(cookies[i], key, &name, &value);
#else
      gcgiParseCookie(cookies[i], &name, &value);
#endif

      fprintf(gcgiOut, 
	      "Cookie #%d: %s<br>\n"
	      "<b>Decoded and Parsed Cookie:</b> Name: %s, Value: %s, Size: %d<br>\n",
	      i+1, cookies[i], name, value, strlen(value));

      free(name);
      free(value);
    }

    gcgiFreeCookies(cookies);
  }


  fprintf(gcgiOut,
	  "</body>\n"
	  "</html>\n");

  freeCgi();

#ifdef USE_SSL
  free(key);
#endif

  return 0;
}
