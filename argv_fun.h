#pragma once

#include <stddef.h>
#include "list.h"

list_t *get_params(int *argc, char ***argv);

int set_param_from_argv(int *argc, char ***argv, char **dest);
int set_params_from_argv(int *argc, char ***argv, char ***dest, size_t *dest_size);
