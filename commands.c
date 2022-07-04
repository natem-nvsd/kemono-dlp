/* (c) 2022, by Nate Morrison
 * commands.c: The code for executing commands is here.
  * Copyright (c) 2022 by Nate Morrison.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "kemono-dlp.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

static void write_cookie(bool cookie_type, char *path) {
		char *home = getenv("HOME");
		char *cookie_path;
		FILE *cookie_file;
		FILE *input_file;
		char *cookie;
		size_t input_size;

		/* Read the cookie from input */
		input_file = fopen(path, "r");
		if (input_file == NULL) {
			fprintf(stderr, "%s: No such file or directory.\n", path);
			exit(ENOENT);
		}

		fseek(input_file, 0, SEEK_END);
		input_size = ftell(input_file);
		rewind(input_file);

		cookie = (char *) malloc(input_size);
		fread(cookie, sizeof(char), input_size, input_file);

		/* Create the file to write to */
		cookie_path = (char *) malloc(strlen(home) + 30);
		if (cookie_type == KEMONO_COOKIE)
			sprintf(cookie_path, "%s/.config/kemono-dlp/kemono-cookies.txt", home);
		else
			sprintf(cookie_path, "%s/.config/kemono-dlp/coomer-cookies.txt", home);

		cookie_file = fopen(cookie_path, "w");
		fputs(cookie, cookie_file);

		free(cookie);
		fclose(cookie_file);
		fclose(input_file);
}

extern void get_cmd(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "%s: %s: Too few arguments.\n", argv[0], argv[1]);
		exit(1);
	}

	if (argc > 3) {
		fprintf(stderr, "%s: %s: Too many arguments.\n", argv[0], argv[1]);
		exit(E2BIG);
	}

	/** Check arguments **/
	if (strcmp(argv[2], "useragent") == 0) {
		printf("\"%s\"\n", get_useragent());
		exit(0);
	}

	if (strcmp(argv[2], "kemono-cookies") == 0) {
		printf("\"%s\"\n", get_cookie_path(KEMONO_COOKIE));
		exit(0);
	}

	if (strcmp(argv[2], "kemono-cookie") == 0) {
		printf("%s: get: kemono-cookie: Did you mean '%s get kemono-cookies'?\n", argv[0], argv[0]);
		exit(EINVAL);
	}

	if (strcmp(argv[2], "coomer-cookies") == 0) {
		printf("\"%s\"\n", get_cookie_path(COOMER_COOKIE));
		exit(0);
	}

	if (strcmp(argv[2], "coomer-cookie") == 0) {
		printf("%s: get: coomer-cookie: Did you mean '%s get coomer-cookies'?\n", argv[0], argv[0]);
		exit(EINVAL);
	}

	fprintf(stderr, "%s: get: %s: Invalid argument.\n", argv[0], argv[2]);
	exit(EINVAL);
}

extern void set_cmd(int argc, char **argv) {
	if (argc < 4) {
		fprintf(stderr, "%s: %s: Too few arguments.\n", argv[0], argv[1]);
		exit(1);
	}

	if (argc > 4) {
		fprintf(stderr, "%s: %s: Too many arguments.\n", argv[0], argv[1]);
		exit(E2BIG);
	}

	/** Check arguments **/
	if (strcmp(argv[2], "useragent") == 0) {
		// THis was the first command written, so I would not forget how bad kemono-dl was at weird useragents.
		char *home = getenv("HOME");
		char *ua_path;
		FILE *ua_file;

		ua_path = (char *) malloc(strlen(home) + 30);
		sprintf(ua_path, "%s/.config/kemono-dlp/useragent", home);

		ua_file = fopen(ua_path, "w");
		fprintf(ua_file, "%s", argv[3]);
		fclose(ua_file);

		exit(0);
	}

	if (strcmp(argv[2], "kemono-cookies") == 0) {
		write_cookie(KEMONO_COOKIE, argv[3]);
		exit(0);
	}

	if (strcmp(argv[2], "coomer-cookie") == 0) {
		printf("%s: set: coomer-cookie: Did you mean '%s set coomer-cookies %s'?\n", argv[0], argv[0], argv[3]);
		exit(EINVAL);
	}

	fprintf(stderr, "%s: set: %s: Invalid argument.\n", argv[0], argv[2]);
	exit(EINVAL);
}
