/* download.c
 * Low-level functions for fetching and writing site data.
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
#include <curl/curl.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Site, Service, User ID, post
static void download(bool site, char *service, char *id, char *name, char *url) {
	FILE *file;
	CURL *curl;
	char err_msg[CURL_ERROR_SIZE];

	/** CURL stuff **/
	char *cookie = get_cookie_path(site);

	curl = curl_easy_init();
	curl_easy_setopt(url, CURLOPT_COOKIE, cookie);
	curl_easy_setopt(url, CURLOPT_FOLLOWLOCATION, 1L);	// Follow redirects
	curl_easy_setopt(url, CURLOPT_ERRORBUFFER, err_msg);
	curl_easy_setopt(url, CURLOPT_WRITEFUNCTION, fwrite);

	if (verbose == true)
		curl_easy_setopt(url, CURLOPT_VERBOSE, 1L);
	curl_easy_perform(url);

	/** Download **/
	file = fopen(name, "w");
	curl_easy_setopt(url, CURLOPT_URL, name);
	curl_easy_setopt(url, CURLOPT_WRITEDATA, file);
	fclose(file);
	file = NULL;
}

/* do_artist():
 * 1. Get the artist's JSON file
 * 2. Get List of post URLs
 * 3. Pass URLs to do_post() 
 */
extern void do_artist(char *url) {
	struct node *data;
	CURL *curl;
	char err_msg[CURL_ERROR_SIZE];
	char *json = NULL;
	int iter;
	char *artist;
	char *id;

	curl = curl_easy_init();
	curl_easy_setopt(url, CURLOPT_COOKIE, get_cookie_data(get_site_from_url(url)));
	curl_easy_setopt(url, CURLOPT_ERRORBUFFER, err_msg);
	curl_easy_setopt(url, CURLOPT_WRITEFUNCTION, fwrite);
	if (verbose == true)
		curl_easy_setopt(url, CURLOPT_VERBOSE, 1L);
	
	curl_easy_setopt(url, CURLOPT_WRITEDATA, json);

	data = json_parse(ARTIST, json);
	for (iter = 4; iter < get_linked_list_size(data); iter++)
		do_post(get_data_from_node(data, iter), artist);
}

/* do_post():
 * 1. Get the post's JSON file
 * 2. Get list of post's attachments
 * 3. Download attachments and/or commentary
 */
extern void do_post(char *url, char *artist) {
	DIR *post_dir;
	FILE *commentary_file;
	char *dirpath;
	char *json;
	CURL *curl;
	char *cookie;
	char err_msg[CURL_ERROR_SIZE];
	size_t iter;
	struct node *data;
	bool site;

	site = get_site_from_url(url);
	cookie = get_cookie_path(site);

	curl = curl_easy_init();
	curl_easy_setopt(url, CURLOPT_COOKIE, cookie);
	curl_easy_setopt(url, CURLOPT_ERRORBUFFER, err_msg);
	curl_easy_setopt(url, CURLOPT_WRITEFUNCTION, fwrite);
	if (verbose == true)
		curl_easy_setopt(url, CURLOPT_VERBOSE, 1L);
	
	curl_easy_setopt(url, CURLOPT_WRITEDATA, json);
	curl_easy_perform(url);

	data = json_parse(POST, json);
	char *service = get_data_from_node(data, 0);
	char *id = get_data_from_node(data, 1);
	char *name = get_data_from_node(data, 2);
	char *commentary = get_data_from_node(data, 4);

	if (user_outdir == true) {
		// ...
	} else {
		// fmt: ""
		dirpath = (char *) malloc(strlen(artist) + strlen(id) + 3);
		sprintf(dirpath, "%s [%s]", artist, id);

		post_dir = opendir(dirpath);
		chdir(dirpath);
	}


	for (iter = 5; iter < sizeof(data); iter++)
		download(site, service, id, name, get_data_from_node(data, iter));

	chdir(cwd);
	closedir(post_dir);
}

extern void download_delegate(void) {
	struct node *links;	// [Link][Name]
	struct node *file_links;
	struct node *str_links;
	int iter;

	size_t link_count;
	size_t str_count;
	size_t file_count;

	/* Links from file */
	if (links_from_file == true) {
		FILE *file;
		char *data;
		size_t file_size;

		file = fopen(links_path, "r");
		fseek(file, 0L, SEEK_END);
		file_size = ftell(file);
		rewind(file);

		data = (char *) malloc(file_size);
		fread((char *) data, sizeof(char), file_size, file);
		file_links = link_parse(data);

		free(data);
		fclose(file);
		data = NULL;
	}

	/* Links from string */
	if (links_from_str == true) {
		str_links = link_parse(links_str);
	}

	/* Merge */
	if (links_from_file == true && links_from_str == true) {
		for (iter = 0; iter < str_count; iter++)
			add_node(links, get_data_from_node(str_links, iter));

		for (iter = 0; iter < file_count; iter++)
			add_node(links, get_data_from_node(file_links, iter));
	}

	for (iter = 0; iter < sizeof(links); iter++) {
		if (check_link(get_data_from_node(links, iter)) == ARTIST) {
			do_artist(get_data_from_node(links, iter));
		} else {
			char *url = get_data_from_node(links, iter);
			char *artist = get_artist_from_url(url);

			do_post(url, artist);
		}
	}
}

extern char *get_artist_from_url(char *url) {
	char *artist = NULL;

	return artist;
}
