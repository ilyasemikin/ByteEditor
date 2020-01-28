#pragma once

#include <stddef.h>

struct node {
	void *data;
	struct node *prev;
	struct node *next;
};

typedef struct node node_t;

struct list {
	node_t *head;
	node_t *tail;
};

typedef struct list list_t;

node_t *node_create(node_t *prev, node_t *next, void *data);

list_t *list_create();
int list_add_begin(list_t *list, void *data);
int list_add_end(list_t *list, void *data);
void list_delete(list_t *list);
size_t list_count(list_t list);
