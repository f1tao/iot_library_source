INCLUDES           = -I$(top_builddir) -I$(top_srcdir)
AM_CFLAGS          = -Wall

lib_LTLIBRARIES    = libgcgi.la

libgcgi_la_SOURCES = 	\
        gcgi.c      	\
        gcgi.h      	\
	gcgi_private.h  \
	code.c          \
	crypt.c         \
	crypt.h         \
        mime.c      	\
        mime.h      	\
        common.c    	\
        common.h    	\
        parse.c     	\
        parse.h     	\
	crlfstringbuf.c \
	crlfstringbuf.h

include_HEADERS    = gcgi.h
