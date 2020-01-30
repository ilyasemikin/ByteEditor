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

	const char *input_cmd = argv[1];

	if (strcmp(input_cmd, "help") == 0)
		help(p_name, cmds, cmds_size);

	long long cmd_index = command_find(input_cmd, cmds, cmds_size);
	if (cmd_index == -1) {
		char error_msg[100];
		snprintf(error_msg, sizeof(error_msg), "unknown command \'%s\'", input_cmd);
		error_exit_help(p_name, error_msg);
	}

	command_desc_t cmd = cmds[cmd_index];

	argc -= 2;
	argv += 2;

	if ((!cmd.more_params_allow && cmd.count_params != argc) 
	    || (cmd.more_params_allow && cmd.count_params > argc)) {
		char error_msg[100];
		snprintf(error_msg, sizeof(error_msg), "incorrect usage command \'%s\'", cmd.name);
		error_exit_help(p_name, error_msg);
	}
	cmd.func(p_name, argc, argv);
}
