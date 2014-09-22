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
    #include <stdarg.h>
#endif
#include <stdlib.h>

/*
 * Functions to manipulate the generic Mercury data structure which is
 * used for IPC over network sockets. It consist of an IPC header plus
 * an optional data segment. The on-the-wire layout is:
 *
 * 4 bytes | 4 bytes | (0 <= n) bytes
 * COMMAND   LENGTH    <DATA>
 *
 * COMMAND: A uint32_t value. In big endian byte order.  Part of the
 *          IPC header.
 *
 * LENGTH: A uint32_t specifying the length in bytes of the following
 *         data array. In big endian byte order. May be 0 (zero). Part
 *         of the IPC header.
 *
 * DATA: An array of uint8_t. The layout of this array is determined
 *       by the value of COMMAND. This array is non-present if LENGTH
 *       is 0 (zero). All encoded numbers are in big endian byte
 *       order. All encoded strings are UTF-8.
 */

/*
 * So that we can pass IPC data directly to the network
 * functions. Furthermore, it is better that we can see that it is IPC
 * data, not just some random memory block. IPC data is structured
 * data, unlike void* data which is simply a block of memory without
 * inherent internal structure.
 *
 * It would be nice if we could make this a struct, or just a simple
 * typedef, but then we would loose const flexibility.
 */
#define ipcdata_t void*
#define IPC_DATALENGTH_OFFSET (sizeof(uint32_t))     // offset of LENGTH
#define IPC_RETURN_VALUE_OFFSET (2*sizeof(uint32_t)) // offset of IPC_ReturnValue
#define IPC_RETURN_DATA_OFFSET (3*sizeof(uint32_t))  // data following IPC_ReturnValue
#define IPC_HEADER_SIZE (2*sizeof(uint32_t)) // size of COMMAND + LENGTH fields
#define IPC_RESULT_SIZE (3*sizeof(uint32_t)) // minimum size of CMD_RESULT data
#define IPC_BUFFER_SIZE (8*1024) // constant size of receiving data buffer

/*
 * Will receive an IPC result code and data. Returns true (1) if
 * successful and false (0) if any error occurred. buf_len is the size of
 * buf in bytes and *count is the number of bytes received into buf,
 * including the return code.
 */
extern int
recv_result(int socket,
            const size_t buflen,
            void * const buf,
	    size_t *count,
	    time_t timeout);

/*
 * Will ensure that the command "cmd" is send over "sock".
 *
 * send_command() returns the number of bytes send unless an error
 * occurred, in which case 0 is returned.
 */
extern uint32_t
vsend_cmd(int sock,
          const char * const format,
          va_list ap);

extern uint32_t
send_cmd(int sock,
         const char * const format,
         ...);


