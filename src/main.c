/*
 *    Copyright (C) 2014 Jules Colding <jcolding@gmail.com>.
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

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "cmdline/argopt.h"
#include "network/net_interfaces.h"
#include "ipc/ipc.h"

/*
 * FIXME
 */
static int
parse_reply(const size_t buflen,
	    char *buf, 
	    uint64_t *timestamp)
{
	return 1;
}

/*
 * FIXME
 */
static int
compose_request(char buf[IPC_BUFFER_SIZE])
{
	return 1;
}

/*
 * HOST:PORT
 */
static int
split_address(char * const addr,
	      char **hostname,
	      unsigned int *port)
{
	if (!port || !addr)
		return 0;
	*hostname = addr;

	char *tmp = *hostname;
	while (*tmp && (':' != *tmp))
		++tmp;

	if (!*tmp)
		return 0;

	*tmp = '\0';
	++tmp;

	if (!*tmp)
		return 0;

	char *endptr = NULL;
	*port = strtol(tmp, &endptr, 10);
	if (!*port || *endptr != '\0')
		return 0;

	if (UINT16_MAX < *port)
		return 0;

	return 1;
}

static int
get_latest_update(const int timeout, int socket, uint64_t *timestamp)
{
	char buf[IPC_BUFFER_SIZE] = { '\0' };
	if (!compose_request(buf)) {
		fprintf(stdout, "Could not compose request\n");
		return 0;
	}
		
	if (!send_cmd(socket, "%s", buf)) {
		fprintf(stdout, "Could not send request\n");
		return 0;
	}

	size_t count;
	if (!recv_result(socket, sizeof(buf), buf, &count, timeout)) {
		fprintf(stdout, "Could not get reply\n");
		return 0;
	}

	return parse_reply(count, buf, timestamp);
}

int main(int argc, char **argv)
{
	/* current argv index */
	int index = 0;

	/* options */
	int timeout = 0;
	int tolerance = 0;
	char *left_host = NULL;
	char *right_host = NULL;

	int c;
	char *parameter;
	char *endptr = NULL;
	struct option_t options[] = {
		{"left_host", "-left_host <PARAMETER> Left host name in name:port format", NEED_PARAM, NULL, 'l'},
		{"right_host", "-right_host <PARAMETER> Right host name in name:port format", NEED_PARAM, NULL, 'r'},
		{"tolerance", "-tolerance <PARAMETER> Maximum tolarable difference for equality. Must be an integer.", NEED_PARAM, NULL, 't'},
		{"timeout", "-timeout <PARAMETER> Send and recieve timeout. Must be an integer.", NEED_PARAM, NULL, 'w'},
		{0, 0, (enum need_param_t)0, 0, 0}
	};

	/* get options */
	while (1) {
		c = argopt(argc,
			   argv,
			   options,
			   &index,
			   &parameter);

		switch (c) {
		case ARGOPT_OPTION_FOUND :
			fprintf(stdout, "Option found:\t\t%s (*flag is not NULL)\n", argv[index]);
			break;
		case ARGOPT_AMBIGIOUS_OPTION :
			argopt_completions(stdout,
					   "Ambigious option found. Possible completions:",
					   ++argv[index],
					   options);
			break;
		case ARGOPT_UNKNOWN_OPTION :
			fprintf(stdout, "Unknown option found:\t%s\n", argv[index]);
			argopt_help(stdout,
				    "Unknown option found",
				    argv[0],
				    options);
			break;
		case ARGOPT_NOT_OPTION :
			fprintf(stdout, "Bad or malformed option found:\t%s\n", argv[index]);
			break;
		case ARGOPT_MISSING_PARAM :
			fprintf(stdout, "Option missing parameter:\t%s\n", argv[index]);
			break;
		case ARGOPT_DONE :
			goto opt_done;
		case 0 :
			fprintf(stdout, "Option found:\t\t%s\n", argv[index]);
			break;
		case 'l' :
			left_host = strdup(parameter);
			fprintf(stdout, "Left host is is:\t\"%s\"\n", parameter);
			break;
		case 'r' :
			right_host = strdup(parameter);
			fprintf(stdout, "Right host is is:\t\"%s\"\n", parameter);
			break;
		case 't' :
			tolerance = strtol(parameter, &endptr, 10);
			if (*endptr != '\0') {
				fprintf(stdout, "Tolerance parameter is not valid:\t%s\n", parameter);
				exit(EXIT_FAILURE);
			}
			fprintf(stdout, "Tolerance parameter is:\t\"%s\"\n", parameter);
			break;
		case 'w' :
			timeout = strtol(parameter, &endptr, 10);
			if (*endptr != '\0') {
				fprintf(stdout, "Timeout parameter is not valid:\t%s\n", parameter);
				exit(EXIT_FAILURE);
			}
			fprintf(stdout, "Timeout parameter is:\t\"%s\"\n", parameter);
			break;
		default:
			printf ("?? get_option() returned character code 0%o ??\n", c);
		}
		if (parameter)
			free(parameter);
	}
opt_done:
	if ((index) && (index < argc)) {
		fprintf(stdout, "non-option ARGV-elements: ");
		while (index < argc)
			fprintf(stdout, "%s ", argv[index++]);
		fprintf(stdout, "\n");

		exit(EXIT_FAILURE);
	}

	/*
	 * Parse hostnames
	 */
	unsigned int left_port;
	char *left_hostname = NULL;
	if (!split_address(left_host, &left_hostname, &left_port)) {
		fprintf(stdout, "Left hostname incorrectly formatted\n");
		exit(EXIT_FAILURE);
	}
	unsigned int right_port;
	char *right_hostname = NULL;
	if (!split_address(right_host, &right_hostname, &right_port)) {
		fprintf(stdout, "Right hostname incorrectly formatted\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * Get update time from left host
	 */
	int left_socket;
	left_socket = connect_to_listening_socket(left_hostname, left_port, PF_INET, SOCK_STREAM, 30);
	if (-1 == left_socket) {
		fprintf(stdout, "Could not connect to left host\n");
		exit(EXIT_FAILURE);
	}
	uint64_t left_time;
	if (!get_latest_update(timeout, left_socket, &left_time)) {
		fprintf(stdout, "Could not get latest timestamp from left host\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * Get update time from right host
	 */
	int right_socket;
	right_socket = connect_to_listening_socket(right_hostname, right_port, PF_INET, SOCK_STREAM, 30);
	if (-1 == right_socket) {
		fprintf(stdout, "Could not connect to right host\n");
		exit(EXIT_FAILURE);
	}
	uint64_t right_time;
	if (!get_latest_update(timeout, right_socket, &right_time)) {
		fprintf(stdout, "Could not get latest timestamp from right host\n");
		exit(EXIT_FAILURE);
	}

	/*
	 * Compare the hosts
	 */
	if (tolerance < (left_time - right_time))
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
}
