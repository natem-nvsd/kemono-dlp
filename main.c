/* kemono-dlp (c) 2022, by Nate Morrison
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
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

bool arg_ua;
bool artist;
bool banner;
bool coomer_fav_posts;
bool coomer_fav_users;
bool commentary;
bool dms;
bool extract_links;
bool icon;
bool json;
bool kemono_fav_users;
bool kemono_fav_posts;
bool links_from_file;
bool links_from_str;
bool nopart;
bool overwrite;
bool prefer_kemono;
bool quiet;
bool simulate;
bool skip_json;
bool user_outdir;
bool verbose;

char *arg_useragent;
char *cwd;
char date_after[DATE];
char date_before[DATE];
char *directory_pattern;
char *exclude_types;
char *file_pattern;
char *include_types;
char *inline_pattern;
char *links_str;
char *links_path;
char *outdir;
char updated_after[DATE];
char updated_before[DATE];
char *prog;

double min_size;
double max_size;
int retry;

static void print_help(void) {
	exit(0);
}

int main(int argc, char **argv) {
	int args;
	short status;
	DIR *dir;
	char *user = getenv("HOME");
	char *path;
	size_t cwd_size;

	prog = (char *) malloc(strlen(argv[0]));
	strcpy(prog, argv[0]);

	getcwd(NULL, cwd_size);
	cwd = (char *) malloc(cwd_size);

	/** Required directories **/
	/* Config path */
	path = (char *) malloc(strlen(user) + 9);
	sprintf(path, "%s/.config", user);
	dir = opendir(path);
	if (dir)
		closedir(dir);
	else if (ENOENT == errno)
		mkdir(path, S_IRWXU);
	free(path);

	/* Kemono Config */
	path = (char *) malloc(strlen(user) + 20);
	dir = opendir(path);
	sprintf(path, "%s/.config/kemono-dlp", user);
	if (dir)
		closedir(dir);
	else if (ENOENT == errno)
		mkdir(path, S_IRWXU);
	free(path);


	/** Arguments **/
	switch (argc) {
		case 1:
			print_help();

		case 2: {
			if (strlen(argv[1]) == 2) {
				if ((argv[1])[0] == '-')
					break;
			}

			if (strcmp(argv[1], "get") == 0) {
				fprintf(stderr, "%s: %s: Too few arguments.\n", argv[0], argv[1]);
				exit(EINVAL);
			}

			if (strcmp(argv[1], "set") == 0) {
				fprintf(stderr, "%s: %s: Too few arguments.\n", argv[0], argv[1]);
				exit(EINVAL);
			}

			fprintf(stderr, "%s: %s: Invalid command.\n", argv[0], argv[1]);
			exit(EINVAL);
		}

		default: {
			if (strcmp(argv[1], "get") == 0)
				get_cmd(argc, argv);

			if (strcmp(argv[1], "set") == 0)
				set_cmd(argc, argv);

			/*
			fprintf(stderr, "%s: %s: Invalid command.\n", argv[0], argv[1]);
			exit(EINVAL);
			*/
		}
	}

	// F, n, N, Q, R, S, t, T, w, W, y, Y, z, Z
	while ((args = getopt(argc, argv, "a:A:bB:cCd:DeEf:hHiI:jJkKl:L:m:M:o:OpPqrsu:U:vVx:X:")) != -1) {
		switch (args) {
			case 'a': {
				// User agent
				arg_ua = true;
				open(optarg, O_RDONLY, 0);
				arg_useragent = optarg;

				break;
			}

			case 'A': {
				// date after
				open(optarg, O_RDONLY, 0);
				if (strlen(optarg) > DATE) {
					fprintf(stderr, "%s: Invalid argument.\n", argv[0]);
					return(EINVAL);
				}

				strcpy(date_after, optarg);

				break;
			}

			case 'b': {
				//banner
				banner = true;

				break;
			}

			case 'B': {
				// date before
				open(optarg, O_RDONLY, 0);
				if (strlen(optarg) > DATE) {
					fprintf(stderr, "%s: Invalid argument.\n", argv[0]);
					return(EINVAL);
				}

				strcpy(date_before, optarg);

				break;
			}

			case 'c': {
				// coomer.party favourite posts
				coomer_fav_posts = true;

				break;
			}

			case 'C': {
				// coomer.party favourite users
				coomer_fav_users = true;

				break;
			}

			case 'd': {
				// directory name pattern
				open(optarg, O_RDONLY, 0);
				directory_pattern = (char *) malloc(strlen(optarg));
				strcpy(directory_pattern, optarg);

				break;
			}

			case 'D': {
				// dms
				dms = true;

				break;
			}

			case 'e': {
				// extract links
				extract_links = true;

				break;
			}

			case 'E': {
				printf("Copyright (c) 2022 by Nate Morrison.\n");
				printf("All rights reserved.\n\n");
				printf("Redistribution and use in source and binary forms, with or without\n");
				printf("modification, are permitted provided that the following conditions are met:\n\n");
				printf("1. Redistributions of source code must retain the above copyright notice, this\n");
				printf("   list of conditions and the following disclaimer.\n");
				printf("2. Redistributions in binary form must reproduce the above copyright notice,\n");
				printf("   this list of conditions and the following disclaimer in the documentation\n");
				printf("   and/or other materials provided with the distribution.\n\n");
				printf("THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n");
				printf("ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n");
				printf("WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n");
				printf("DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\n");
				printf("ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n");
				printf("(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n");
				printf("LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n");
				printf("ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n");
				printf("(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n");
				printf("SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n\n\n");
				printf("THIS SOFTWARE MAY ONLY BE USED TO DOWNLOAD ART FROM ARTISTS YOU ARE CURRENTLY\n");
				printf("SUPPORTING!\n");
				return 0;
			}

			case 'f': {
				// file name pattern
				open(optarg, O_RDONLY, 0);
				file_pattern = (char *) malloc(strlen(optarg));
				strcpy(file_pattern, optarg);

				break;
			}

			case 'h': {
				// help
				print_help();
			}

			case 'H': {
				// HTML commentary download
				commentary = true;

				break;
			}

			case 'i': {
				// icon
				icon = true;

				break;
			}

			case 'I': {
				// inline file name pattern
				open(optarg, O_RDONLY, 0);
				inline_pattern = (char *) malloc(strlen(optarg));
				strcpy(inline_pattern, optarg);

				break;
			}

			case 'j': {
				// json
				bool site;

				open(optarg, O_RDONLY, 0);
				inline_pattern = (char *) malloc(strlen(optarg));
				strcpy(inline_pattern, optarg);

				if ((strcmp(optarg, "kemono") == 0) | (strcmp(optarg, "coomer") == 0)) {
					if (strcmp(optarg, "kemono") == 0)
						site = KEMONO;
					else
						site = COOMER;

					//fetch_artist_json(site);
				} else {
					fprintf(stderr, "%s: Invalid argument.\n", argv[0]);
					return EINVAL;
				}

				break;
			}

			case 'J': {
				// Do not re-download JSON
				skip_json = true;
				break;
			}

			case 'k': {
				// kemono.party favourite posts
				kemono_fav_posts = true;

				break;
			}

			case 'K': {
				// kemono.party favourite users
				kemono_fav_users = true;

				break;
			}

			case 'l': {
				// links CSV
				open(optarg, O_RDONLY, 0);
				links_from_str = true;
				links_str = (char *) malloc(strlen(optarg));
				strcpy(links_str, optarg);

				break;
			}

			case 'L': {
				// links from file LSV
				open(optarg, O_RDONLY, 0);
				links_from_file = true;
				links_path = (char *) malloc(strlen(optarg));
				strcpy(links_path, optarg);

				break;
			}

			case 'm': {
				// minimum file size
				open(optarg, O_RDONLY, 0);
				min_size = atof(optarg);

				break;
			}

			case 'M': {
				// maximum file size
				open(optarg, O_RDONLY, 0);
				max_size = atof(optarg);

				break;
			}

			case 'o': {
				// output directory
				open(optarg, O_RDONLY, 0);
				outdir = (char *) malloc(strlen(optarg));
				strcpy(outdir, optarg);
				user_outdir = true;

				break;
			}

			case 'O': {
				// overwrite
				overwrite = true;

				break;
			}

			case 'p': {
				// no .part files
				nopart = true;

				break;
			}

			case 'P': {
				// no .part files
				prefer_kemono = true;

				break;
			}

			case 'q': {
				// quiet
				quiet = true;

				break;
			}

			case 'r': {
				// retry x times
				open(optarg, O_RDONLY, 0);
				retry = atoi(optarg);

				break;
			}

			case 's': {
				// simulate
				simulate = true;

				break;
			}

			case 'u': {
				// user updated before [date]
				open(optarg, O_RDONLY, 0);
				if (strlen(optarg) > DATE) {
					fprintf(stderr, "%s: -u: Dates must be in six character long strings.\n", argv[0]);
					return(EINVAL);
				}

				strcpy(updated_before, optarg);

				break;
			}

			case 'U': {
				// updated after [date]
				open(optarg, O_RDONLY, 0);
				if (strlen(optarg) > DATE) {
					fprintf(stderr, "%s: -u: Dates must be in six character long strings.\n", argv[0]);
					return(EINVAL);
				}

				strcpy(updated_after, optarg);

				break;
			}

			case 'v': {
				// verbose
				verbose = true;

				break;
			}

			case 'V': {
				// version
				printf("%s\n", VERSION);
				exit(0);
			}

			case 'x': {
				// eXclude [types]
				open(optarg, O_RDONLY, 0);
				exclude_types = (char *) malloc(strlen(optarg));
				strcpy(exclude_types, optarg);

				break;
			}

			case 'X': {
				// only include [types]
				open(optarg, O_RDONLY, 0);
				include_types = (char *) malloc(strlen(optarg));
				strcpy(include_types, optarg);

				break;
			}
		}
	}

	/** Do magick **/
	download_delegate();

	return status;
}
