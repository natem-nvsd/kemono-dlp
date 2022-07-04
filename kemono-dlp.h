/* kemono-dlp.h
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

#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <stdbool.h>
#include <stdio.h>

/** Macros **/
#define KEMONO		0
#define KEMONO_COOKIE	0
#define COOMER		1
#define COOMER_COOKIE	1
#define ARTIST		false
#define POST		true
#define DATE		6
#define VERSION		"0.1.0"

/* Services */
#define DISCORD		0
#define DLSITE		1
#define FANBOX		2
#define FANTIA		3
#define GUMROAD		4
#define PATREON		5
#define SUBSTAR		6


/** Data Types **/
struct node {
	int id;
	char *data;
	struct node *next;
};


/** Functions **/
extern bool check_link(char *url);
extern void do_artist(char *url);
extern void do_post(char *url, char *artist);
extern void download_delegate(void);
extern void fetch_artist_json(bool site);
extern char *get_artist_from_url(char *url);
extern void get_cmd(int argc, char **argv);
extern char *get_config_path(void);
extern bool get_site_from_url(char *url);
extern char *get_useragent(void);
extern char *get_cookie_path(bool cookie_type);
extern char *get_cookie_data(bool cookie_type);
extern struct node *json_parse(bool type, char *json);
extern struct node *link_merge(char **src0, char **src1);
extern struct node *link_parse(char *data);
extern void set_cmd(int argc, char **argv);

/* Linked List Functions */
extern struct node *new_linked_list(void);
extern void add_node(struct node *list, char *data);
extern char *get_data_from_node(struct node *list, int index);
extern size_t get_linked_list_size(struct node *list);
extern size_t count_nodes(struct node *list);


/** Variables **/
extern bool arg_ua;
extern bool banner;
extern bool coomer_fav_posts;
extern bool coomer_fav_users;
extern bool commentary;
extern bool dms;
extern bool extract_links;
extern bool icon;
extern bool json;
extern bool kemono_fav_users;
extern bool kemono_fav_posts;
extern bool links_from_file;
extern bool links_from_str;
extern bool nopart;
extern bool overwrite;
extern bool prefer_kemono;
extern bool quiet;
extern bool simulate;
extern bool skip_json;
extern bool user_outdir;
extern bool verbose;

extern char *arg_useragent;
extern char *cwd;
extern char date_after[DATE];
extern char date_before[DATE];
extern char *directory_pattern;
extern char *exclude_types;
extern char *file_pattern;
extern char *include_types;
extern char *inline_pattern;
extern char *links_str;
extern char *links_path;
extern char *outdir;
extern char updated_after[DATE];
extern char updated_before[DATE];
extern char *prog;

extern double min_size;
extern double max_size;
extern int retry;
extern unsigned int posts_offset;


#endif
