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
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include "network/network.h"
#include "marshal/marshal.h"
#include "ipc.h"

/*
 * Hints to the compiler whether an expression is likely to be true or
 * not
 */
#define LIKELY(expr__) (__builtin_expect(((expr__) ? 1 : 0), 1))
#define UNLIKELY(expr__) (__builtin_expect(((expr__) ? 1 : 0), 0))

int
recv_result(int socket,
            const size_t buflen,
            void * const buf,
            size_t *count,
	    time_t timeout)
{
        ipcdata_t pos = buf;
        uint32_t rem;
        ssize_t packet_size;
        ssize_t recv_cnt_acc = 0; // accumulated receive count
        ssize_t recv_cnt = 0;

        if (!buf || !buflen) {
                fprintf(stdout, "NULL buffer");
                return 0;
        }

        if (!set_recv_timeout(socket, timeout)) {
                fprintf(stdout, "could not set timeout");
                return 0;
        }

        do {
                recv_cnt = recvfrom(socket, (void*)((uint8_t*)pos + recv_cnt_acc), buflen - recv_cnt_acc, MSG_WAITALL, NULL, NULL);
                switch (recv_cnt) {
                case -1:
                        fprintf(stdout, "error: %s", strerror(errno));
                        return 0;
                case 0:
                        fprintf(stdout, "peer disconnected");
                        return 0;
                default:
			/*
			 * Now we set a timeout of 5 seconds. Continue
			 * transmitting or fail within that limit.
			 *
			 * Using UNLIKELY to tell the compiler that we
			 * only think this will happen once in a while.
			 */
                        if UNLIKELY(!recv_cnt_acc) {
                                if (!set_recv_timeout(socket, timeout)) {
                                        fprintf(stdout, "could not set timeout");
                                        return 0;
                                }
                        }
                        break;
                }
                recv_cnt_acc += recv_cnt;
                if ((ssize_t)IPC_HEADER_SIZE <= recv_cnt_acc)
                        break;
        } while (1);

        // infer command length and get the rest
        rem = ipcdata_get_datalen(pos);
        packet_size = rem + IPC_HEADER_SIZE;
        if (buflen < packet_size) {
                fprintf(stdout, "buffer too small. Required %d, available %d", packet_size, buflen);
                return 0;
        }
        while UNLIKELY(recv_cnt_acc < packet_size) {
                recv_cnt = recvfrom(socket, (void*)((uint8_t*)pos + recv_cnt_acc), buflen - recv_cnt_acc, MSG_WAITALL, NULL, NULL);
                switch (recv_cnt) {
                case -1:
                        fprintf(stdout, "error: %s", strerror(errno));
                        return 0;
                case 0:
                        fprintf(stdout, "peer disconnected");
                        return 0;
                default:
                        break;
                }
                recv_cnt_acc += recv_cnt;
        }
        *count = recv_cnt_acc;

        return 1;
}



uint32_t
send_cmd(int socket,
         const char * const format,
         ...)
{
        uint32_t retv;
        va_list ap;

        va_start(ap, format);
        retv = vsend_cmd(socket, format, ap);
        va_end(ap);

        return retv;
}

uint32_t
vsend_cmd(int socket,
          const char * const format,
          va_list ap)
{
        static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        static uint8_t buf[IPC_BUFFER_SIZE];
        uint32_t len;
        uint8_t *pos;
        va_list ap2;

        va_copy(ap2, ap);
        len = vmarshal_size(format, ap2);
        va_end(ap2);
        if ((IPC_BUFFER_SIZE - IPC_HEADER_SIZE) < len) {
                errno = EINVAL;
                return 0;
        }

        while (pthread_mutex_trylock(&mutex))
                usleep(1);

        if (len) {
                pos = buf + IPC_HEADER_SIZE;
                if (!vmarshal(pos, len, &len, format, ap)) {
                        goto error;
                }
        }
        len += IPC_HEADER_SIZE;
        if (!send_all(socket, buf, len)) {
                goto error;
        }
        goto exit;

error:
        len = 0;

exit:
        pthread_mutex_unlock(&mutex);

        return len;
}
