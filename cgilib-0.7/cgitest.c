/*
    cgitest.c - Testprogram for cgi.o
    Copyright (c) 1998,9,2007,8 by Martin Schulze <joey@infodrom.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * Compile with: cc -o cgitest cgitest.c -lcgi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cgi.h>

#define HTML_CSS "<style type=\"text/css\">\n\
body { background-color: #afafff; }\n\
.formbox { border: 1px solid black; background: #dedede; padding: 2px; margin: 5px; }\n\
a:link, a:visited { text-decoration: none; }\n\
.footer { border-top: 1px solid black; font-size: 80%%; }\n\
</style>\n"

#define HTML_HEAD "<html>\n<head><title>cgilib forms</title></head>\n<body>\n"
#define HTML_FOOT "<div class=\"footer\">\
<a href=\"http://www.infodrom.org/\">Infodrom Oldenburg</a>&nbsp;&nbsp;&nbsp;&nbsp;Project \
<a href=\"http://www.infodrom.org/projects/cgilib/\">cgilib</a>\n</div></body></html>"

#define HTML_FORMCONTENT "<table>\
<tr><td valign=\"top\">Input</td><td><input name=string size=50></td></tr>\n\
<tr><td valign=\"top\">Select</td><td><select name=select multiple>\n\
<option>Nr. 1\n<option>Nr. 2\n<option>Nr. 3\n<option>Nr. 4\n</select></td></tr>\n\
<tr><td valign=\"top\">Radio</td><td><input type=\"radio\" name=\"radio\" value=\"yes\"> yes \
<input type=\"radio\" name=\"radio\" value=\"no\"> no</td></tr>\n\
<tr><td valign=\"top\">Text</td><td><textarea name=text cols=50>\n</textarea></td></tr>\n"

#define HTML_SUBMIT "</table><center><input type=submit value=Submit> <input type=reset value=Reset></center>\n"

s_cgi *cgi;

void print_form()
{
    printf ("<h1>cgilib Test-Forms</h1>\n");

    printf ("<div class=\"formbox\">");
    printf ("<b>GET, display selected</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/insertdata\" method=GET>\n");
    printf (HTML_FORMCONTENT HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>POST, display selected</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/insertdata\" method=post>\n");
    printf (HTML_FORMCONTENT HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>POST, multipart, display selected</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/insertdata\" method=POST enctype=\"multipart/form-data\">\n");
    printf (HTML_FORMCONTENT HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>GET, display everything</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/listall\" method=GET>\n");
    printf (HTML_FORMCONTENT HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>POST, display everything</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/listall\" method=POST>\n");
    printf (HTML_FORMCONTENT HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>POST, multipart, display everything</b><br>\n");
    printf ("<form action=\"/cgi-bin/cgitest/listall\" method=post enctype=\"multipart/form-data\">\n");
    printf (HTML_FORMCONTENT);
    printf ("<tr><td>File</td><td><input name=file type=file></td></tr>");
    printf (HTML_SUBMIT);
    printf ("</form>\n");
    printf ("</div>");

    printf ("<div class=\"formbox\">");
    printf ("<b>Misc</b>\n");
    printf ("<br><a href=\"/cgi-bin/cgitest/redirect\">Redirect</a>\n");
    printf ("<br><a href=\"/cgi-bin/cgitest/listall?var=value&var2=val2;var3=val3\">List Everything</a>\n");
    printf ("<br><a href=\"/cgi-bin/cgitest/setcookie\">Set Cookie</a>\n");
    printf ("</div>");
}

void eval_cgi()
{
    printf ("<h1>Results</h1>\n\n");
    printf ("<div class=\"formbox\">");
    printf ("<b>string</b>: %s<p>\n", cgiGetValue(cgi, "string"));
    printf ("<b>radio</b>: %s<p>\n", cgiGetValue(cgi, "radio"));
    printf ("<b>text</b>: %s<p>\n", cgiGetValue(cgi, "text"));
    printf ("<b>select</b>: %s<p>\n", cgiGetValue(cgi, "select"));
    printf ("</div>");
}

void listall (char **env)
{
  char **vars;
  char *val;
  char *tmp;
  s_cookie *cookie;
  s_file *file;
  int i;

  printf ("<h3>Environment Variables</h3>\n<pre>\n");
  for (i=0; env[i]; i++) {
      tmp = cgiEscape (env[i]);
      if (tmp) {
	  printf ("%s\n", tmp);
	  free (tmp);
      }
  }
  
  printf ("</pre>\n<h3>CGI Variables</h3>\n<pre>\n");

  vars = cgiGetVariables (cgi);
  if (vars) {
      for (i=0; vars[i] != NULL; i++) {
	  val = cgiGetValue (cgi, vars[i]);
	  if (val) {
	      tmp = cgiEscape (val);
	      if (tmp) {
		  printf ("%s=%s\n", vars[i], tmp);
		  free (tmp);
	      } else
		  printf ("%s=...\n", vars[i]);
	  } else
	      printf ("%s=\n", vars[i]);
      }
      cgiFreeList (vars);
  } else
      printf ("No variables transmitted.\n");

  printf ("</pre>\n<h3>Cookies</h3>\n<pre>\n");

  vars = cgiGetCookies (cgi);
  if (vars) {
      for (i=0; vars[i] != NULL; i++) {
	  cookie = cgiGetCookie (cgi, vars[i]);
	  if (cookie) {
	      tmp = cgiEscape (cookie->value);
	      if (tmp) {
		  printf ("%s=%s\n", vars[i], tmp);
		  free (tmp);
	      } else
		  printf ("%s=...\n", vars[i]);
	  } else
	      printf ("%s=\n", vars[i]);
      }
      cgiFreeList (vars);
  } else
      printf ("No cookies transmitted.\n");

  printf ("</pre>\n<h3>Files</h3>\n<pre>\n");

  vars = cgiGetFiles (cgi);
  if (vars) {
      for (i=0; vars[i] != NULL; i++) {
	  file = cgiGetFile (cgi, vars[i]);
	  if (file) {
	      tmp = cgiEscape (file->filename);
	      printf ("%s=%s (%s", vars[i], tmp, file->tmpfile);
	      free (tmp);
	      if (file->type) {
		  tmp = cgiEscape (file->type);
		  printf (", %s)\n", tmp);
		  free (tmp);
	      } else
		  printf (")\n");
	  }
      }
      cgiFreeList (vars);
  } else
      printf ("No files transmitted.\n");

  printf ("</pre>\n");
}

int main (int argc, char **argv, char **env)
{
    char *path_info = NULL;

    cgiDebug(0, 0);
    cgi = cgiInit();

    path_info = getenv("PATH_INFO");
    if (path_info) {
	if (!strcmp(path_info, "/redirect")) {
	    cgiRedirect("http://www.infodrom.org/");
	    exit (0);
	} else if (!strcmp(path_info, "/setcookie")) {
	    cgiSetHeader ("Set-Cookie", "Version=1; Library=cgilib; Path=/");
            cgiHeader();
	    printf (HTML_CSS HTML_HEAD);
	    printf ("<h3>Cookie Library set</h3>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest\">Test</a><p>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/redirect\">Redirect</a><p>\n");
	    printf ("<p><br><p><br><a href=\"/cgi-bin/cgitest/listall\">List Everything</a><p>\n");
	} else if (!strcmp(path_info, "/listall")) {
            cgiHeader();
	    printf (HTML_CSS HTML_HEAD);
	    listall (env);
	} else {
	    cgiHeader();
	    printf (HTML_CSS HTML_HEAD);
	    if (strlen (path_info))
		printf ("path_info: %s<br>\n", path_info);
	    if (!strcmp(path_info, "/insertdata")) {
		eval_cgi();
	    } else
		print_form();
	}
    } else {
	cgiHeader();
	printf (HTML_CSS HTML_HEAD);
	print_form();
    }

    printf (HTML_FOOT);
    cgiFree (cgi);
    return 0;
}

/*
 * Local variables:
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  tab-width: 8
 * End:
 */
