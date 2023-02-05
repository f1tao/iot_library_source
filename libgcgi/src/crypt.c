/* -*-mode:c; c-style:k&r; c-basic-offset:4; -*- */
/*
 * cgiPop, a web-based POP email client.
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

#include "common.h"

#ifdef USE_SSL

#include <openssl/evp.h>
#include "crypt.h"


unsigned char *
generateKey()
{
    FILE *rng;
    int   num;
    static unsigned char *key;
  
    if ((rng = fopen("/dev/random", "r")) == NULL)
	return NULL;

    key = XMALLOC(unsigned char, EVP_MAX_KEY_LENGTH);
    num = 0;
    while (num < EVP_MAX_KEY_LENGTH) {
	num += fread(&key[num], 1, EVP_MAX_KEY_LENGTH - num, rng);
    }
    
    fclose(rng);

    return key;
}


void 
encryptString(char *text, int size, unsigned char *key, char **ciphertext, int *ctlen)
{
    EVP_CIPHER_CTX  ctx;
    EVP_CIPHER     *cipher;
    int             len;
    char            ivec[EVP_MAX_IV_LENGTH] = {0};

    cipher = EVP_bf_cbc();
    EVP_EncryptInit(&ctx, cipher, key, ivec);

    /* Allocate ciphertext and include enough length for padding */
    *ciphertext = XMALLOC(unsigned char, size + EVP_MAX_KEY_LENGTH + 1);

    EVP_EncryptUpdate(&ctx, *ciphertext, &len, text, size);
    *ctlen  = len;
    EVP_EncryptFinal(&ctx, (*ciphertext) + len, &len);
    *ctlen += len;
}


void 
decryptString(char *ciphertext, int ctlen, unsigned char *key, char **text, int *size)
{
    EVP_CIPHER_CTX  ctx;
    EVP_CIPHER     *cipher;
    int             len;
    char            ivec[EVP_MAX_IV_LENGTH] = {0};

    cipher = EVP_bf_cbc();
    EVP_DecryptInit(&ctx, cipher, key, ivec);

    /* Allocate ciphertext and include enough length for padding */
    *text = XMALLOC(unsigned char, ctlen);
    
    EVP_DecryptUpdate(&ctx, *text, &len, ciphertext, ctlen);
    *size  = len;
    EVP_DecryptFinal(&ctx, (*text) + len, &len);
    *size += len;
}


void 
generateStringHMAC(char *text, int size, unsigned char **hmac, int *hlen)
{
    EVP_MD_CTX ctx;

    *hmac = XMALLOC(unsigned char, EVP_MAX_MD_SIZE);

    EVP_DigestInit(&ctx, EVP_sha1());
    EVP_DigestUpdate(&ctx, text, size);
    EVP_DigestFinal(&ctx, *hmac, hlen);
}


int
writeKeyToFile(unsigned char *key, char *path)
{
    FILE   *out;
    mode_t  old_mode;

    old_mode = umask(077);

    if ((out = fopen(path, "w")) == NULL)
	return -1;

    umask(old_mode);

    if (fwrite(key, sizeof(unsigned char), EVP_MAX_KEY_LENGTH, out) < EVP_MAX_KEY_LENGTH)
	return -1;

    fclose(out);

    return 0;
}

int
readKeyFromFile(char *path, unsigned char **key)
{
    FILE   *in;
    size_t  read;
    off_t   offset;

    if ((in = fopen(path, "r")) == NULL)
	return -1;
    
    read   = 0;
    offset = 0;
    *key   = XMALLOC(unsigned char, EVP_MAX_KEY_LENGTH);
    memset(*key, 0, EVP_MAX_KEY_LENGTH);

    /* Read in the query from STDIN */
    do {
        read = fread(*key + offset, sizeof(unsigned char), EVP_MAX_KEY_LENGTH - offset, in);
        if (ferror(in)) 
            return -1;
        else if (read <= 0)
            continue;
        offset += read;
    } while (offset < EVP_MAX_KEY_LENGTH && read > 0);

    fclose(in);

    return 0;
}


void 
printHexString(FILE *output, unsigned char *s, int len)
{
    int i;

    for (i = 0; i < len; i++)
	fprintf(output, "%02x", s[i]);
}

#endif /* USE_SSL */
