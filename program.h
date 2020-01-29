#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "bytes_buffer.h"
#include "command.h"

struct program_description {
	const char *name;
};

typedef struct program_description program_description_t;

// Modes functions
void print(const char *p_name, int argc, char **argv);
void remove_each_occur(const char *p_name, int argc, char **argv);
void search(const char *p_name, int argc, char **argv);

void error_exit(const char *p_name, const char *msg);
void error_exit_help(const char *p_name, const char *msg);
void help(const char *p_name, command_desc_t *cmds, size_t cmds_size);
