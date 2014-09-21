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
#include <pthread.h>
#include "marshal/marshal.h"
#include "ipc.h"

int
recv_result(int socket,
            void * const buf,
            const uint32_t buf_len,
            uint32_t *count)
{
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
