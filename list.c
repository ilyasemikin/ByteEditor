#include <stdlib.h>
#include "list.h"

node_t *node_create(node_t *prev, node_t *next, void *data) {
	node_t *ret;
	ret = (node_t *)malloc(sizeof(node_t));
	if (ret == NULL)
		return NULL;
	ret->prev = prev;
	ret->next = next;
	ret->data = data;
	return ret;
}

list_t *list_create() {
	list_t *ret;
	ret = (list_t *)malloc(sizeof(list_t));
	ret->head = ret->tail = NULL;
	return ret;
}

int list_add_begin(list_t *list, void *data) {
	if (list == NULL)
		return -1;
	if (list->head == NULL) {
		node_t *node = node_create(NULL, NULL, data);
		if (node == NULL)
			return -1;
		list->head = list->tail = node;
	}
	else {
		node_t *node = node_create(NULL, list->head, data);
		if (node == NULL)
			return -1;
		list->head->prev = node;
		list->head = node;
	}
	return 0;
}

int list_add_end(list_t *list, void *data) {
	if (list == NULL)
		return -1;
	if (list->tail == NULL) {
		node_t *node = node_create(NULL, NULL, data);
		if (node == NULL)
			return -1;
		list->head = list->tail = node;
	}
	else {
		node_t *node = node_create(list->tail, NULL, data);
		if (node == NULL)
			return -1;
		list->tail->next = node;
		list->tail = node;
	}
	return 0;
}

void list_delete(list_t *list) {
	node_t *p;
	p = list->head;
	while (p != NULL) {
		node_t *next = p->next;
		free(p);
		p = next;
	}
	free(list);
}

void list_delete_with_data(list_t *list) {
	node_t *p;
	p = list->head;
	while (p != NULL) {
		node_t *next = p->next;
		if (p->data != NULL)
			free(p->data);
		free(p);
		p = next;
	}
	free(list);
}

size_t list_count(list_t list) {
	size_t count = 0;
	node_t *p;
	p = list.head;
	while (p != NULL) {
		count++;
		p = p->next;
	}
	return count;
}
