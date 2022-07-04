/* (c) 2022, by Nate Morrison
 * getters.c: The code in the file is for getting data like file paths, cookie files, and more.
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

extern char *get_config_path(void) {
	char *home = getenv("HOME");
	char *path;

	path = (char *) malloc(strlen(home) + 21);
	sprintf(path, "%s/.config/kemono-dlp/", home);

	return path;
}

extern char *get_useragent(void) {
	char *home = getenv("HOME");
	char *ua;
	char *ua_path;
	FILE *ua_file;
	size_t ua_size;

	ua_path = (char *) malloc(strlen(home) + 30);
	sprintf(ua_path, "%s/.config/kemono-dlp/useragent", home);

	ua_file = fopen(ua_path, "r");
	if (ua_file == NULL) {
		fprintf(stderr, "%s: No such file or directory.\n", ua_path);
		exit(ENOENT);
	}

	fseek(ua_file, 0, SEEK_END);
	ua_size = ftell(ua_file);
	rewind(ua_file);

	ua = (char *) malloc(ua_size);
	fread(ua, sizeof(char), ua_size, ua_file);

	fclose(ua_file);
	return ua;
}

extern char *get_cookie_path(bool cookie_type) {
	char *home = getenv("HOME");
	char *cookie_path;

	cookie_path = (char *) malloc(strlen(home) + 39);
	if (cookie_type == 0)
		sprintf(cookie_path, "%s/.config/kemono-dlp/kemono-cookies.txt", home);
	else
		sprintf(cookie_path, "%s/.config/kemono-dlp/coomer-cookies.txt", home);

	return cookie_path;
}

extern char *get_cookie_data(bool cookie_type) {
	char *home = getenv("HOME");
	char *cookie;
	char *cookie_path;
	FILE *cookie_file;
	size_t cookie_size;

	cookie_path = (char *) malloc(strlen(home) + 39);
	if (cookie_type == 0)
		sprintf(cookie_path, "%s/.config/kemono-dlp/kemono-cookies.txt", home);
	else
		sprintf(cookie_path, "%s/.config/kemono-dlp/coomer-cookies.txt", home);

	cookie_file = fopen(cookie_path, "r");
	if (cookie_file == NULL) {
		fprintf(stderr, "%s: No such file or directory.\n", cookie_path);
		exit(ENOENT);
	}

	fseek(cookie_file, 0, SEEK_END);
	cookie_size = ftell(cookie_file);
	rewind(cookie_file);

	cookie = (char *) malloc(cookie_size);
	fread(cookie, sizeof(char), cookie_size, cookie_file);

	fclose(cookie_file);
	return cookie;
}
