#include "program.h"
#include "command.h"

int main(int argc, char **argv) {
	command_desc_t commands[] = {
		{
			.name = "print",
			.desc = "print file content in hex",
			.usage = "[INPUT]",
			.count_params = 1,
			.more_params_allow = false,
			.func = print
		},

		{
			.name = "remeach",
			.desc = "remove every byte occurrence in file",
			.usage = "[INPUT] [OUTPUT] [BYTES...]",
			.count_params = 3,
			.more_params_allow = true,
			.func = remove_each_occur
		},

		{
			.name = "search",
			.desc = "search bytes in file",
			.usage = "[INPUT] [BYTES...]",
			.count_params = 2,
			.more_params_allow = true,
			.func = search
		}
	};

	perform_command(commands, sizeof(commands) / sizeof(command_desc_t), argc, argv);
}
