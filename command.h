#pragma once

#include <stddef.h>

struct command_desc {
	const char *name;
	const char *desc;
	const char *usage;
	void (*func)(const char *pname, int argc, char **argv);
};

typedef struct command_desc command_desc_t; 

void perform_command(command_desc_t *cmds, size_t cmds_size, int argc, char **argv);
