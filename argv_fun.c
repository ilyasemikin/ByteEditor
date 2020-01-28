#include <stdlib.h>
#include <string.h>
#include "argv_fun.h"

#include <stdio.h>

list_t *get_params(int *argc, char ***argv) {
	list_t *list = list_create();
	(*argc)--;
	(*argv)++;
	while (*argc > 0) {
		size_t length = strlen(**argv);
		if (length > 0 && (**argv)[0] == '-') {
			(*argc)++;
			(*argv)--;
			return list;
		}
		list_add_end(list, **argv);
		(*argc)--;
		(*argv)++;
	}
	return list;
}

int set_param_from_argv(int *argc, char ***argv, char **dest) {
	list_t *params = get_params(argc, argv);

	size_t count = list_count(*params);

	if (count != 1) {
		list_delete(params);
		return -1;
	}

	*dest = (char *)params->head->data;

	list_delete(params);

	return 0;
}

int set_params_from_argv(int *argc, char ***argv, char ***dest, size_t *dest_size) {
	list_t *params = get_params(argc, argv);

	size_t count = list_count(*params);

	if (count == 0) {
		list_delete(params);
		return -1;
	}

	*dest = (char **)malloc(sizeof(char *) * count);
	size_t i = 0;
	node_t *p = params->head;
	while (p != NULL) {
		(*dest)[i++] = (char *)p->data;
		p = p->next;
	}
	*dest_size = count;

	list_delete(params);

	return 0;
}
