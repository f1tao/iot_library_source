#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

int main (int argc, char *argv[], char *envp[]) 
{
    FILE  *output;
    int    clen;
    size_t read, write;
    off_t  offset;
    char  *querystring, *e;

    if ((e = getenv("CONTENT_LENGTH")) == NULL) {
      fprintf(stderr, "Unable to get content length.");
      return -1;
    }

    clen = strtol(e, NULL, 10);
    /* Content Length was out of range */
    if (errno == ERANGE) {
      fprintf(stderr, "Unable to convert Content-Length to integer.\n");
        return -1;
    }

    read           = 0;
    offset         = 0;
    querystring    = (char *) malloc(clen+1);
    memset(querystring, 0, clen + 1);

    /* Read in the query from STDIN */
    do {
        read = fread(querystring + offset, sizeof(char), clen - offset, stdin);
        if (ferror(stdin)) 
            return -1;
        else if (read <= 0)
            continue;
        offset += read;
    } while (offset < clen && read > 0);

    if ((output = fopen("./output", "w")) == NULL)
      fprintf(stderr, "Unable to open output!");

    write = fwrite(querystring, sizeof(char), clen, output);
    fclose(output);

    fprintf(stdout, "Content-Type: text/plain\r\n\r\nDone.\n");

    return 0;
}
