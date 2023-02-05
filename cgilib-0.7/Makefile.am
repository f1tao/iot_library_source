
#   Copyright (c) 1998,9,2001,7,8 by Martin Schulze <joey@infodrom.org>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software Foundation
#   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

lib_LTLIBRARIES = libcgi.la

libcgi_la_SOURCES = cgi.c cookies.c aux.c
libcgi_la_LDFLAGS = -version-info @SO_VERSION@

bin_PROGRAMS = cgitest jumpto

cgitest_SOURCES = cgitest.c
cgitest_LDADD = libcgi.la

jumpto_SOURCES = cgitest.c
jumpto_LDADD = libcgi.la

include_HEADERS = aux.h cgi.h

man_MANS =  cgi.5 cgiDebug.3 cgiEscape.3 cgiFree.3 cgiFreeList.3 cgiGetCookie.3 cgiGetCookies.3 cgiGetFile.3 cgiGetFiles.3 cgiGetValue.3 cgiGetVariables.3 cgiHeader.3 cgiInit.3 cgiRedirect.3 cgiSetHeader.3 cgiSetType.3
