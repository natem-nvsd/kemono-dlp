/* parser.c
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

#define PCRE2_CODE_UNIT_WIDTH 8

#include "kemono-dlp.h"
#include <errno.h>
#include <json-c/json.h>
#include <locale.h>
#include <pcre2.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Data is expected to be clean.
 * Site is either kemono or coomer
 */
extern bool get_site_from_url(char *url) {
	bool site;

	if ((strncmp(url, "https://kemono.party/", 21) == 0) | (strncmp(url, "https://coomer.party/", 21) == 0)) {
		if (strncmp(url, "https://kemono.party/", 21) == 0)
			site = KEMONO;
		else
			site = COOMER;
	} else {
		// exit()?
	}

	return site;
}

/* link_parse() is for re-organising URLs into a 2D array for quick downloading.
 * It also sorts URLs into different areas of the arry depending on the URL
 * (coomer.party or kemono.party).
 * Input data example:
https://kemono.party/fanbox/user/XXXXXXXX,https://kemono.party/fanbox/user/XXXXXXXX,https://kemono.party/fanbox/user/XXXXXXXX
https://kemono.party/fanbox/user/XXXXXXXX
https://kemono.party/fanbox/user/XXXXXXXX
https://kemono.party/fanbox/user/XXXXXXXX
 */
extern struct node *link_parse(char *data) {
	struct node *coomer_links;
	struct node *kemono_links;
	size_t coomer_size;
	size_t kemono_size;
	size_t total_size;
	int iter;

	coomer_size = get_linked_list_size(coomer_links);
	kemono_size = get_linked_list_size(kemono_links);
	total_size = (coomer_size + kemono_size);

	char *tmp[total_size];
	struct node *crap;

	/** Merge the two buffers **/
	if (prefer_kemono == true) {
		for (iter = 0; iter < kemono_size; iter++)
			add_node(crap, get_data_from_node(kemono_links, iter));
	
		for (iter = 0; iter < coomer_size; iter++)
			add_node(crap, get_data_from_node(coomer_links, iter));
	} else {
		for (iter = 0; iter < coomer_size; iter++)
			add_node(crap, get_data_from_node(coomer_links, iter));

		for (iter = 0; iter < kemono_size; iter++)
			add_node(crap, get_data_from_node(kemono_links, iter));
	}

	/** Parse links **/
	for (iter = 0; iter < total_size; iter++) {
		regex_t regex;
		int regres;

		/* Sanitise input */
		regres = regcomp(&regex, "s/,/\n/g", 0);
		regres = regcomp(&regex, tmp[iter], 0);
		regres = regexec(&regex, tmp[iter], 0, NULL, 0);

		/* Check if valid */
//		regres = regcomp(&regex, "^https://(kemono\.party|coomer\.party)/([^/]+)/user/([^/]+)($|/post/([^/]+)$)", 0);
		regres = regcomp(&regex, "^https:\\/\\/(kemono.party|coomer.party)/([^/]+)/user/([^/]+)($|/post/([^/]+)$)", 0);
		regres = regcomp(&regex, "/\\v^https:\\/\\/(coomer.party|kemono.party)\\/([^/]+)\\/user\\/([^/]+)\\/post\\/.*\n", 0);

		regres = regcomp(&regex, tmp[iter], 0);
		if (!regres) {
			strcpy(tmp[iter], tmp[iter]);
		} else if (regres == REG_NOMATCH) {
		}
	}

	/* Remove empty indexes */

	/** Finalise **/
	return crap;
}

extern struct node *json_parse(bool type, char *json) {
	size_t LIST_SIZE;
	json_object *root;
	struct node *list = new_linked_list();
	int iter;

	/* Post Data */
	json_object *id;
	json_object *name;
	json_object *service;

	if (type == ARTIST) {
		root = json_object_from_file(json);
		service = json_object_object_get(root, "service");

		add_node(list, (char *) json_object_get_string(service));
		add_node(list, (char *) json_object_get_string(id));
		add_node(list, (char *) json_object_get_string(name));

		LIST_SIZE = get_linked_list_size(list);
		for (iter = 3; iter < LIST_SIZE; iter++)
			add_node(list, NULL);
	} else {
		json_object *indexed;
		json_object *updated;
		json_object *id;
		json_object *title;
		json_object *date;
		json_object *commentary;

		add_node(list, (char *) json_object_get_string(service));
		add_node(list, (char *) json_object_get_string(id));
		add_node(list, (char *) json_object_get_string(title));
		add_node(list, (char *) json_object_get_string(date));
		add_node(list, (char *) json_object_get_string(commentary));

		LIST_SIZE = get_linked_list_size(list);
		for (iter = 5; iter < LIST_SIZE; iter++)
			add_node(list, NULL);
	}

	return list;
}

// Returns ARTIST or POST
extern bool check_link(char *url) {
	bool site;
	char *expression = "\v\\/([^/]+)\\/([^/]+)\\/";
	PCRE2_SPTR pattern;
	PCRE2_SPTR subject;
	PCRE2_SIZE erroffset;
	pcre2_code *regex;
	pcre2_match_data_8 *match;
	int errno;
	int rc;	// What is this short for?

	site = get_site_from_url(url);
	pattern = (PCRE2_SPTR) "\vuser\\/([^/]+)\\/([^/]+)\\/";
	regex = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errno, &erroffset, NULL);

	if (regex == NULL) {
		fprintf(stderr, "regex failed to compile at %lu, %s\n", erroffset, url);
		exit(1);
	}

	rc = pcre2_match(regex, (const unsigned char *) url, strlen(url), 0, 0, match, NULL);

	if (site == ARTIST) {
		return ARTIST;
	} else
		return POST;
}
