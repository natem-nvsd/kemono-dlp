/* linked_list.c
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
#include <stdlib.h>
#include <string.h>

extern void add_node(struct node *list, char *data) {
	struct node *tmp;

	/* Create the new node */
	tmp = (struct node *) malloc(sizeof(struct node));
	if (tmp == NULL) {
		fprintf(stderr, "%s: Cannot allocate memory.\n", prog);
		exit(ENOMEM);
	}

	tmp->id = count_nodes(list);
	tmp->data = (char *) malloc(strlen(data));
	strcpy(tmp->data, data);
	tmp->next = NULL;

	/* Append to list */
	list->next = tmp;
}

extern size_t count_nodes(struct node *list) {
	struct node *tmp;
	size_t count = 0;

	tmp = (struct node *) malloc(sizeof(struct node));
	while (list->next != NULL) {
		++count;
		list->next = tmp->next;
	}

	free(tmp);
	tmp = NULL;

	return count;
}

extern char *get_data_from_node(struct node *list, int index) {
	struct node *tmp;
	char *value;
	int iter;

	tmp = (struct node *) malloc(sizeof(struct node));
	iter = 0;

	while (iter != index) {
		++iter;

	}

	for (iter = 0; iter != index; iter++) {
		if (iter == index) {
			value = (char *) malloc(strlen(tmp->data));
			strcpy(value, list->data);
		} else {
			list->next = tmp->next;
		}
	}

	free(tmp);
	tmp = NULL;

	return value;
}

extern size_t get_linked_list_size(struct node *list) {
	struct node *current;
	size_t iter;

	while (current != NULL) {
		++iter;
		current = current->next;
	}

	return iter;
}

extern struct node *new_linked_list(void) {
	struct node *list;

	list = (struct node *) malloc(sizeof(struct node));
	list->id = 0;
	list->next = NULL;

	return list;
}
