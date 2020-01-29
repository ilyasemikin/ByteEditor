#include <stdio.h>
#include <string.h>
#include "program.h"
#include "command.h"

static long long command_find(const char *name, const command_desc_t *cmds, size_t cmds_size) {
	size_t i;
	for (i = 0; i < cmds_size; i++)
		if (strcmp(cmds[i].name, name) == 0)
			return i;
	return -1;
}


void perform_command(command_desc_t *cmds, size_t cmds_size, int argc, char **argv) {
	const char *p_name = *argv;

	if (argc < 2)
		error_exit_help(p_name, "missing command");
	
	const char *cmd = argv[1];
	long long cmd_index = command_find(cmd, cmds, cmds_size);
	if (cmd_index == -1) {
		char error_msg[100];
		snprintf(error_msg, sizeof(error_msg), "unknown command \'%s\'", cmd);
		error_exit_help(p_name, error_msg);
	}
	cmds[cmd_index].func(p_name, argc - 2, argv + 2);
}
