#include "program.h"
#include "command.h"

int main(int argc, char **argv) {
	command_desc_t commands[] = {
		{
			.name = "print",
			.desc = "print file content in hex",
			.usage = "[INPUT]",
			.func = print
		},
		{
			.name = "remeach",
			.desc = "remove every byte occurrence in file",
			.usage = "[INPUT] [OUTPUT] [BYTES...]",
			.func = remove_each_occur
		},
		{
			.name = "search",
			.desc = "search bytes in file",
			.usage = "[INPUT] [BYTES...]",
			.func = search
		}
	};

	perform_command(commands, sizeof(commands) / sizeof(command_desc_t), argc, argv);
}
