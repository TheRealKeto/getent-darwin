/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2010 Ed Schouten <ed@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <utmpx.h>
#include "utxdb.h"

#include "utmpx-defines.h"
#define rounddown(x, y) (((x) / (y)) * (y))

#ifdef __NO_TLS
static FILE *uf = NULL;
static int udb;
#else
static _Thread_local FILE *uf = NULL;
static _Thread_local int udb;
#endif

int
setutxdb(int db, const char *file)
{
	struct stat sb;

	switch (db) {
	case UTXDB_ACTIVE:
		if (file == NULL)
			file = _PATH_UTX_ACTIVE;
		break;
	case UTXDB_LASTLOGIN:
		if (file == NULL)
			file = _PATH_UTX_LASTLOGIN;
		break;
	case UTXDB_LOG:
		if (file == NULL)
			file = _PATH_UTX_LOG;
		break;
	default:
		errno = EINVAL;
		return (-1);
	}

	if (uf != NULL)
		fclose(uf);
	uf = fopen(file, "re");
	if (uf == NULL)
		return (-1);

	if (db != UTXDB_LOG) {
		/* Safety check: never use broken files. */
		if (fstat(fileno(uf), &sb) != -1 &&
		    sb.st_size % sizeof(struct futx) != 0) {
			fclose(uf);
			uf = NULL;
			errno = EFTYPE;
			return (-1);
		}
		/* Prevent reading of partial records. */
		(void)setvbuf(uf, NULL, _IOFBF,
		    rounddown(BUFSIZ, sizeof(struct futx)));
	}

	udb = db;
	return (0);
}
