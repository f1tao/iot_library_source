INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src \
	   -I$(top_builddir) -I$(top_builddir)/src
AM_CFLAGS = -Wall

noinst_PROGRAMS = gcgiFileUploadTest gcgiFormTest gcgiCookieTest gcgiGetInput

gcgi_HTML = \
	formexample.html \
	fileupload.html \
	cookietest.html 

EXTRA_DIST = \
	mime-data-png \
	mime-data-png-big \
	mimedata2 \
	gcgi-env \
	urlencodedenv \
	urlencodedquery \
	mime-data-2parts \
	mime-data-gif-big \
	file-upload-example-unix.txt \
	file-upload-example-windows.txt \
	$(gcgi_HTML)

gcgiFileUploadTest_SOURCES      = gcgiFileUploadTest.c
gcgiFileUploadTest_LDADD        = $(top_builddir)/src/libgcgi.la
gcgiFileUploadTest_LDFLAGS      = -static

gcgiFormTest_SOURCES      = gcgitest.c
gcgiFormTest_LDADD        = $(top_builddir)/src/libgcgi.la
gcgiFormTest_LDFLAGS      = -static

gcgiCookieTest_SOURCES      = gcgiCookieTest.c
gcgiCookieTest_LDADD        = $(top_builddir)/src/libgcgi.la
gcgiCookieTest_LDFLAGS      = -static

gcgiGetInput_SOURCES= getinput.c
