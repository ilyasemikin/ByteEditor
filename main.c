#include "program.h"
#include "command.h"

int main(int argc, char **argv) {
	command_desc_t commands[] = {
		{
			.name = "print",
			.desc = "",
			.usage = "",
			.func = print
		},
		{
			.name = "remeach",
			.desc = "",
			.usage = "",
			.func = remove_each_occur
		},
		{
			.name = "search",
			.desc = "",
			.usage = "",
			.func = search
		}
	};

	perform_command(commands, sizeof(commands) / sizeof(command_desc_t), argc, argv);
}
