/* -*-mode:c; c-style:k&r; c-basic-offset:4; -*- */
/*
 * GCGILibrary, implementing NCSA'a Common Gateway Interface and RFC2338.
 * Copyright (C) 2001-2002 Julian Catchen, julian@catchen.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CRYPT_H__
#define __CRYPT_H__

unsigned char *generateKey();
void           encryptString(char *text, int size, unsigned char *key, char **ciphertext, int *ctlen);
void           decryptString(char *ciphertext, int ctlen, unsigned char *key, char **text, int *size);
void           generateStringHMAC(char *text, int size, unsigned char **hmac, int *hlen);
int            writeKeyToFile(unsigned char *key, char *path);
int            readKeyFromFile(char *path, unsigned char **key);
void           printHexString(FILE *output, unsigned char *s, int len);

#endif /* __CRYPT_H__ */
