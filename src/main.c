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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cmdline/argopt.h"

int main(int argc, char **argv)
{
	/* current argv index */
	int index = 0;

	/* options */
	int tolerance;
	char *left_host = NULL;
	char *right_host = NULL;

	int c;
	char *parameter;
	char *endptr = NULL;
	struct option_t options[] = {
		{"left_host", "-left_host <PARAMETER> Left host name in name:port format", NEED_PARAM, NULL, 'l'},
		{"right_host", "-right_host <PARAMETER> Right host name in name:port format", NEED_PARAM, NULL, 'r'},
		{"tolerance", "-tolerance <PARAMETER> Maximum tolarable difference for equality. Must be an integer.", NEED_PARAM, NULL, 't'},
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
		default:
			printf ("?? get_option() returned character code 0%o ??\n", c);
		}
		if (parameter)
			free(parameter);
	}
opt_done:
	if ((index) && (index < argc)) {
		printf ("non-option ARGV-elements: ");
		while (index < argc)
			printf("%s ", argv[index++]);
		printf ("\n");

		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
