/*
 * University of Illinois/NCSA Open Source License
 *
 * Copyright © 2014 NCSA.  All rights reserved.
 *
 * Developed by:
 *
 * Storage Enabling Technologies (SET)
 *
 * Nation Center for Supercomputing Applications (NCSA)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the .Software.),
 * to deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 *    + Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *
 *    + Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimers in the
 *      documentation and/or other materials provided with the distribution.
 *
 *    + Neither the names of SET, NCSA
 *      nor the names of its contributors may be used to endorse or promote
 *      products derived from this Software without specific prior written
 *      permission.
 *
 * THE SOFTWARE IS PROVIDED .AS IS., WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
  DEALINGS WITH THE SOFTWARE.
 */

/*
 * System includes.
 */
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

/*
 * Globus includes.
 */
#include <globus_gridftp_server.h>

static char * _sharee = NULL;

void
globus_i_gfs_log_transfer(
    int                                 stripe_count,
    int                                 stream_count,
    struct timeval *                    start_gtd_time,
    struct timeval *                    end_gtd_time,
    char *                              dest_ip,
    globus_size_t                       blksize,
    globus_size_t                       tcp_bs,
    const char *                        fname,
    globus_off_t                        nbytes,
    int                                 code,
    char *                              volume,
    char *                              type,
    char *                              username,
    char *                              retrans)
{
	void * module = NULL;
	char * fname_with_quotes = NULL;
	char * extended_username = NULL;
	int    retval = 0;

	static int (*_real_globus_i_gfs_log_transfer) (int              stripe_count,
	                                               int              stream_count,
	                                               struct timeval * start_gtd_time,
	                                               struct timeval * end_gtd_time,
	                                               char *           dest_ip,
	                                               globus_size_t    blksize,
	                                               globus_size_t    tcp_bs,
	                                               const char *     fname,
	                                               globus_off_t     nbytes,
	                                               int              code,
	                                               char *           volume,
	                                               char *           type,
	                                               char *           username,
	                                               char *           retrans) = NULL;

	if (_real_globus_i_gfs_log_transfer == NULL)
	{
		module = dlopen("libglobus_gridftp_server.so.6", RTLD_LAZY|RTLD_LOCAL);

		if (!module)
			exit(1);

		/* Clear any previous error. */
		dlerror();

		_real_globus_i_gfs_log_transfer = dlsym(module, "globus_i_gfs_log_transfer");
		if (dlerror())
			exit(1);
	}

	/*
	 * Surround the file name with quotes.
	 */
	fname_with_quotes = calloc(strlen(fname) + 3, sizeof(char));
	snprintf(fname_with_quotes, strlen(fname) + 3, "\"%s\"", fname);

	/*
	 * Expand username to include SHARE=[0|1] and SHAREE=[_sharee|none]
	 */
	extended_username = calloc(strlen(username) + 
	                           strlen("SHARE=0") + 1 +
	                           strlen("SHAREE=") + 1 +
	                           (_sharee ? strlen(_sharee) : strlen("none")) + 1,
	                           sizeof(char));

	sprintf(extended_username, 
	        "%s SHARE=%d SHAREE=%s", 
	        username,
	        _sharee ? 1 : 0,
	        _sharee ? _sharee : "none");

	_real_globus_i_gfs_log_transfer(stripe_count,
	                                stream_count, 
	                                start_gtd_time,
	                                end_gtd_time,
	                                dest_ip,
	                                blksize,
	                                tcp_bs,
	                                fname_with_quotes,
	                                nbytes,
	                                code,
	                                volume,
	                                type,
	                                extended_username,
	                                NULL); /* Passing NULL until twinkie's DB parser is fixed. */

	free(fname_with_quotes);
	free(extended_username);
}

char *
globus_i_gfs_kv_getval(char * kvstring, char * key)
{
	char * value  = NULL;
	void * module = NULL;
	int    retval = 0;

	static char * (*_real_globus_i_gfs_kv_getval)(char * kvstring, char * key) = NULL;

	if (_real_globus_i_gfs_kv_getval == NULL)
	{
		module = dlopen("libglobus_gridftp_server.so.6", RTLD_LAZY|RTLD_LOCAL);

		if (!module)
			exit(1);

		/* Clear any previous error. */
		dlerror();

		_real_globus_i_gfs_kv_getval = dlsym(module, "globus_i_gfs_kv_getval");
		if (dlerror())
			exit(1);
	}

	value = _real_globus_i_gfs_kv_getval(kvstring, key);

	if (strcmp(key, "SHAREE") == 0 && value)
		_sharee = strdup(value);

	return value;
}
