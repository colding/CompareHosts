/*
 *    Copyright (C) 2013, Jules Colding <jcolding@gmail.com>.
 *
 *    All Rights Reserved.
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 *     (1) Redistributions of source code must retain the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer.
 * 
 *     (2) Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *     
 *     (3) Neither the name of the copyright holder nor the names of
 *     its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written
 *     permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
    #include "ac_config.h"
#endif
#ifdef  __linux__
    #include <stdarg.h>
    #include <string.h>
#endif
#include <errno.h>
#include <sys/time.h>
#include "network.h"

int
send_all(int sock,
         const uint8_t * const buf,
         const int len)
{
        int total = 0;
        int bytesleft = len;
        int n;

        while (total < len) {
                n = send(sock, buf + total, bytesleft, 0);
                if (-1 == n) {
                        return 0;
                }
                total += n;
                bytesleft -= n;
        }

        return 1;
}

int
set_recv_timeout(int sock,
                 const time_t time_out)
{
        struct timeval t;

        t.tv_sec = time_out;
        t.tv_usec = 0;

        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(struct timeval))) {
                return 0;
        }

        return 1;
}

int
set_send_timeout(int sock,
                 const time_t time_out)
{
        struct timeval t;

        t.tv_sec = time_out;
        t.tv_usec = 0;

        if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &t, sizeof(struct timeval))) {
                return 0;
        }

        return 1;
}

int
set_min_recv_sice(int sock,
                  const int ipc_header_size)
{
        if (setsockopt(sock, SOL_SOCKET, SO_RCVLOWAT, &ipc_header_size, sizeof(ipc_header_size))) {
                return 0;
        }

        return 1;
}

int
set_non_blocking(int sock)
{
	int flags;

	flags = fcntl(sock, F_GETFL, 0);
	if (-1 == flags) {
		return 0;
	}

	flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
	if (-1 == flags) {
		return 0;
	}

	return 1;
}

int
set_blocking(int sock)
{
	int flags;

	flags = fcntl(sock, F_GETFL, 0);
	if (-1 == flags) {
		return 0;
	}
	flags &= ~O_NONBLOCK;

	flags = fcntl(sock, F_SETFL, flags);
	if (-1 == flags) {
		return 0;
	}

	return 1;
}
