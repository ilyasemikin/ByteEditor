#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "file.h"
#include "bytes_buffer.h"
#include "bytes_fun.h"
#include "argv_fun.h"

enum PROGRAM_MODE {
	M_PRINT, M_REMOVE
};

struct program_description {
	const char *name;
	enum PROGRAM_MODE mode;
	char *input_file;
	char *output_file;
	char **bytes;
	size_t bytes_size;
};

typedef struct program_description program_description_t;

void error_exit(const char *pname, const char *msg, bool help_msg);
program_description_t parse_argv(int argc, char **argv);

int main(int argc, char **argv) {	
	program_description_t pr_desc = parse_argv(argc, argv);

	switch (pr_desc.mode) {
		case M_PRINT:
			{
				bytes_buffer_t *buffer;
				buffer = read_file_bytes(pr_desc.input_file);
				if (buffer == NULL)
					error_exit(pr_desc.name, "can't read bytes from file", false);
				bbuffer_print(*buffer);
				break;
			}
		case M_REMOVE:
			{	
				size_t i;
				for (i = 0; i < pr_desc.bytes_size; i++) {
					size_t len = strlen(pr_desc.bytes[i]);
					bytes_buffer_t * buf = bbuffer_from_string(pr_desc.bytes[i], len);
					if (buf == NULL)
						error_exit(pr_desc.name, "can't parsing bytes", false);
					bbuffer_print(*buf);
				}
			}
			break;
	}

	return 0;
}

void error_exit(const char *pname, const char *msg, bool help_msg) {
	printf("%s: %s\n", pname, msg);
	if (help_msg)
		printf("Try \'%s -h\' for more info\n", pname);
	exit(-1);
}

void help(const char *pname);

program_description_t parse_argv(int argc, char **argv) {
	program_description_t pr_desc;
	pr_desc.name = &(*argv)[0];
	
	bool mode_selected = false;
	bool input_selected = false;
	bool output_selected = false;
	bool bytes_selected = false;

	while (--argc > 0) {
		const char *str_argv = *(++argv);
		size_t length = strlen(*argv);
		if ((length == 1 && str_argv[0] == '-') || length == 0)
			continue;
		else if (length > 2 && str_argv[0] == '-' && str_argv[1] == '-') {
			printf("LONG PARAM\n");
		}
		else if (length == 2 && str_argv[0] == '-') {
			switch (str_argv[1]) {
				case 'p':
					if (mode_selected)
						error_exit(pr_desc.name, "mode selected again", true);
					pr_desc.mode = M_PRINT;
					mode_selected = true;
					break;
				case 'r':
					if (mode_selected)
						error_exit(pr_desc.name, "mode selected again", true);
					pr_desc.mode = M_REMOVE;
					mode_selected = true;;
					break;
				case 'i': 
					{
						if (input_selected)
							error_exit(pr_desc.name, "input file selected again", true);

						if (set_param_from_argv(&argc, &argv, &pr_desc.input_file) == -1)
							error_exit(pr_desc.name, "for parametr -i expected only 1 parametr", true);

						input_selected = true;
						break;
					}
				case 'o':
					{
						if (output_selected)
							error_exit(pr_desc.name, "output selected again", true);
					
						if (set_param_from_argv(&argc, &argv, &pr_desc.output_file) == -1)
							error_exit(pr_desc.name, "for parametr -o expected only 1 parametr", true);

						output_selected = true;
						break;
					}
				case 'b': 
					{
						if (bytes_selected)
							error_exit(pr_desc.name, "bytes selected again", true);
						if (set_params_from_argv(&argc, &argv, &pr_desc.bytes, &pr_desc.bytes_size) == -1)
							error_exit(pr_desc.name, "for parametr -b expected >1 parametrs", true);
					
						bytes_selected = true;
						break;
					}
				case 'h':
					help(pr_desc.name);
					break;
				default:
					error_exit(pr_desc.name, "unknown parametr", true);
					break;
			}
		}
		else {
		
		}
	}

	if (!mode_selected)
		error_exit(pr_desc.name, "mode not selected", true);
	if (!input_selected)
		error_exit(pr_desc.name, "missing input file", true);
	if (pr_desc.mode == M_PRINT && (output_selected || bytes_selected))
		error_exit(pr_desc.name, "invalid parametrs for -p", true);
	if (pr_desc.mode == M_REMOVE && (!output_selected || !bytes_selected))
		error_exit(pr_desc.name, "invalid parametrs for -r", true);
	return pr_desc;
}

void help(const char *pname) {
	printf("%s\n", pname);

	exit(0);
}
