#include <stdlib.h>
#include <stdio.h>
#include "gcgi.h"

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
  MimeEncoding  enc;
  MimeType      type;
  char         *file, *fname, *subtype; 
  int           len, truncated;

  gcgiDebug("./gcgi-env", "./file-upload-example-unix.txt");
  
  /* Set Limits: field limit, query limit */
  gcgiSetLimits(1048576, 0);

  if (initCgi() < 0) {
    fprintf(stderr, "Unable to parse CGI query.\n");
    return -1;
  }
  
  //gcgiSaveEnvVariables("/tmp/gcgi-env");

  gcgiFieldSize("uploaded_file", &len);
  file = (char *) malloc(len);

  gcgiFetchData("uploaded_file", file, len, &type, &subtype, &enc, &fname, &truncated);

  if (truncated) {
    fprintf(gcgiOut, "Content-Type: text/plain\r\n\r\n");
    fprintf(gcgiOut, "File size has exceeded limit.  Pick something smaller.\n");
  }
  else {
    fprintf(gcgiOut,"Content-Type: %s/%s\r\n", strTypes[type], subtype);
    fprintf(gcgiOut,"Content-Disposition: attachment; filename=\"%s\"\r\n", fname);
    fprintf(gcgiOut,"Content-Length: %d\r\n\r\n", len);

    fprintf(stderr,"Content-Type: %s/%s\r\n", strTypes[type], subtype);
    fprintf(stderr,"Content-Disposition: attachment; filename=\"%s\"\r\n", fname);
    fprintf(stderr,"Content-Length: %d\r\n\r\n", len);

    fwrite(file, sizeof(char), len, gcgiOut);
  }

  free(file);
  free(subtype);
  free(fname);

  freeCgi();

  return 0;
}
