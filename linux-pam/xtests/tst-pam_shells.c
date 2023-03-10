/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, and the entire permission notice in its entirety,
 *    including the disclaimer of warranties.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * ALTERNATIVELY, this product may be distributed under the terms of
 * the GNU Public License, in which case the provisions of the GPL are
 * required INSTEAD OF the above restrictions.  (This clause is
 * necessary due to a potential bad interaction between the GPL and
 * the restrictions contained in a BSD-style copyright.)
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
  test case:

  shells.conf:
  /bin/testbash

*/

#include "test_assert.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <security/pam_appl.h>

static struct pam_conv conv;

int
main(void)
{
  pam_handle_t *pamh = NULL;
  int retval;

  // /bin/testbash is defined in shell definition file(s)
  ASSERT_EQ(PAM_SUCCESS, pam_start("tst-pam_shells", "tstpamshells", &conv, &pamh));
  ASSERT_EQ(PAM_SUCCESS, retval=pam_authenticate (pamh, 0));
  ASSERT_EQ(PAM_SUCCESS, pam_end (pamh,retval));

  // /bin/testnoshell is not defined in shell definition file(s)
  ASSERT_EQ(PAM_SUCCESS, pam_start("tst-pam_shells", "tstnoshell", &conv, &pamh));
  ASSERT_EQ(PAM_AUTH_ERR, retval=pam_authenticate (pamh, 0));
  ASSERT_EQ(PAM_SUCCESS, pam_end (pamh,retval));

  return 0;
}
