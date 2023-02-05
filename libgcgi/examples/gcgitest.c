#include <stdlib.h>
#include <stdio.h>
#include "gcgi.h"
#include "common.h"

FILE *out;
int main (int argc, char *argv[], char *envp[]) {

  char tstring[128], qstring[128], *qs, **bands, tf1[256], tf2[256], mulstr[128], **mmulstr;
  int i, j, k, l, m, *n, o;
  double d;
  char *morebands[] = {"phish", "wsp", "moe." };
  char *colors[]    = {"red", "green", "blue" };

  //gcgiDebug("../examples/urlencodedenv", "../examples/urlencodedquery");

  //out = fopen("./buglog.txt","a");
  
  if (initCgi() < 0)
    return -1;
  
  //gcgiSaveEnvVariables();

  gcgiNumFormFields(&k);
  
  gcgiNumFields("band",&l);

  gcgiFetchString("string", tstring, 128);

  gcgiFetchStringNoNewLines("nonewlines", qstring, 128);

  gcgiFetchInteger("integer", &i, -1);

  gcgiFetchDouble("double", &d, -1.0);

  gcgiFetchCheckbox("choice", &j);

  gcgiFetchMultipleString("band", &bands);

  n = XMALLOC(int, 3);

  gcgiFetchMultipleCheckbox("band", morebands, 3, &n);

  gcgiFetchSelectIndex("radio", colors, 3, &o, -1);

  gcgiFetchString("tf1", tf1, 255);

  gcgiFetchStringNoNewLines("tf2", tf2, 255);

  qs = gcgiFetchEnvVar(gcgiQueryString);

  fprintf(gcgiOut,"Content-Type: Text/HTML\n\n");

  fprintf(gcgiOut,
	  "<html>\n"
	  "<head>\n"
	  "    <title>GCGI Test Results</title>\n"
	  "</head>\n\n"
	  "<body>\n"
	  "    <p><b>Total number of form fields:</b> %d</p>\n"
	  "    <p><b>Text String:</b>  %s</p>\n"
	  "    <p><b>Text String (no new lines):</b>  %s</p>\n"
	  "    <p><b>Integer:</b>      %d</p>\n"
	  "    <p><b>Double:</b>       %f</p>\n"
	  "    <p><b>Single Checkbox:</b> %s</p>\n"
	  "    <p><b>Number of checkboxes for \"band\":</b> %d\n"
	  "    <p><b>Bands:</b><ol>\n",
	  k, tstring, qstring, i, d, j? "checked":"unchecked", l);

  if (bands != NULL)
    for (m = 0; bands[m] != NULL; m++) {
      fprintf(gcgiOut,"<li>%s</li>\n",bands[m]);
    }

  fprintf(gcgiOut,
	  "</ol></p>\n"
	  "<p><b>Radio Button Selected Index:</b> %d"
	  "<p><b>Bands that were checked:</b><ol> ",o);

  for (m = 0; m < 3; m++)
    fprintf(gcgiOut,"<li><b>%s:</b> %s</li>",morebands[m], n[m]? "checked":"unchecked");

  fprintf(gcgiOut,"</ol>\n<p><b>Fetch Multiple Strings one at a time:</b>\n<ol>\n");
  while (gcgiFetchStringNext("mulstr", mulstr, 127) != GCGIFIELDNOTFOUND)
    fprintf(gcgiOut,"<li>%s</li>", mulstr);

  gcgiResetMultipleField("mulstr");

  gcgiFetchMultipleString("mulstr", &mmulstr);

  fprintf(gcgiOut, "</ol>\n<p><b>Fetch Multiple Strings all at once:</b>\n<ol>\n");
  for (m = 0; mmulstr[m] != NULL; m++) {
    fprintf(gcgiOut,"<li>%s</li>\n", mmulstr[m]);
  }
  fprintf(gcgiOut, "</ol>\n</p>\n");

  gcgiFreeMultipleString(mmulstr);

  fprintf(gcgiOut,
	  "<p><b>Text Field One:</b><br><pre>%s</pre><br>\n"
	  "<p><b>Text Field Two (No Newlines):</b><br><pre>%s</pre><br>\n",
	  tf1, tf2);

  fprintf(gcgiOut,
	  "</p>\n"
	  "<pre>\n"
	  "QUERY_STRING: %s\n",
	  qs);
  printQuery(gcgiOut);
  fprintf(gcgiOut,
	  "</pre>\n"
	  "</body>\n"
	  "</html>\n");
  
  gcgiFreeMultipleString(bands);
  XFREE(n);

  freeCgi();

  return 0;
}
