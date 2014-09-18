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

#pragma once

#ifdef HAVE_CONFIG_H
    #include "ac_config.h"
#endif
#ifdef __linux__
    #include <string.h>
#endif

#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/socket.h>

/*
 * Will ensure that "len" bytes from "buf" is send over "socket".
 *
 * send_all() returns 1 (one) unless an error occurred, in which case
 * 0 (zero) is returned and the external value errno is set.
 */
extern int
send_all(int sock,
         const uint8_t * const buf,
         const int len);

/*
 * Sets the recv timeout value in seconds. Returns 1 (one) on success,
 * 0 (zero) otherwise.
 */
extern int
set_recv_timeout(int sock,
                 const time_t time_out);

/*
 * Sets the send timeout value in seconds. Returns 1 (one) on success,
 * 0 (zero) otherwise.
 */
extern int
set_send_timeout(int sock,
                 const time_t time_out);

/*
 * Sets the minimum recieve size in bytes. Returns 1 (one) on success,
 * 0 (zero) otherwise.
 */
extern int
set_min_recv_sice(int sock,
                  const int ipc_header_size);

/*
 * Makes the socket non-blocking,
 *
 * Returns 1 (one) unless an error occurred, in which case 0 (zero) is
 * returned and the external value errno is set.
 */
extern int
set_non_blocking(int sock);

/*
 * Makes the socket blocking,
 *
 * Returns 1 (0ne) unless an error occurred, in which case 0 (zero) is
 * returned and the external value errno is set.
 */
extern int
set_blocking(int sock);
