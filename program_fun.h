#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "bytes_buffer.h"

enum PROGRAM_MODE {
	M_PRINT, M_REMOVE
};

struct program_description {
	const char *name;
	enum PROGRAM_MODE mode;
	bytes_buffer_t *input_buffer;
	char *output_file;
	bytes_buffer_t **bytes;
	size_t bytes_size;
};

typedef struct program_description program_description_t;

void error_exit(program_description_t *pr_desc, const char *msg, bool help_msg);
program_description_t parse_argv(int argc, char **argv);

void help(const char *pname);
