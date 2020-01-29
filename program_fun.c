#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argv_fun.h"
#include "program_fun.h"
#include "bytes_fun.h"
#include "file.h"

void error_exit(program_description_t *pr_desc, const char *msg, bool help_msg) {
	printf("%s: %s\n", pr_desc->name, msg);
	if (help_msg)
		printf("Try \'%s -h\' for more info\n", pr_desc->name);

	if (pr_desc->input_buffer != NULL)
		bytes_buffer_delete(pr_desc->input_buffer);
	if (pr_desc->bytes != NULL) {
		size_t i;
		for (i = 0; i < pr_desc->bytes_size; i++)
			if (pr_desc->bytes[i] != NULL)
				bytes_buffer_delete(pr_desc->bytes[i]);
		free(pr_desc->bytes);
		pr_desc->bytes_size = 0;
	}

	exit(-1);
}

static int bbuffer_compare(const void *a, const void *b);

program_description_t parse_argv(int argc, char **argv) {
	program_description_t pr_desc;
	pr_desc.name = &(*argv)[0];
	pr_desc.input_buffer = NULL;
	pr_desc.output_file = NULL;
	pr_desc.bytes = NULL;
	pr_desc.bytes_size = 0;

	bool mode_selected = false;
	bool input_selected = false;
	bool output_selected = false;
	bool bytes_selected = false;

	while (--argc > 0) {
		const char *str_argv = *(++argv);
		size_t length = strlen(*argv);
		if ((length == 1 && str_argv[0] == '-') || length == 0)
			continue;
		else if (length == 2 && str_argv[0] == '-') {
			switch (str_argv[1]) {
				case 'p':
					if (mode_selected)
						error_exit(&pr_desc, "mode selected again", true);
					pr_desc.mode = M_PRINT;
					mode_selected = true;
					break;
				case 'r':
					if (mode_selected)
						error_exit(&pr_desc, "mode selected again", true);
					pr_desc.mode = M_REMOVE;
					mode_selected = true;;
					break;
				case 'i': 
					{
						if (input_selected)
							error_exit(&pr_desc, "input file selected again", true);

						char *input_file;
						if (set_param_from_argv(&argc, &argv, &input_file) == -1)
							error_exit(&pr_desc, "for parametr -i expected only 1 parametr", true);

						pr_desc.input_buffer = read_file_bytes(input_file);
						if (pr_desc.input_buffer == NULL)
							error_exit(&pr_desc, "can't read bytes from file", false);

						input_selected = true;
						break;
					}
				case 'o':
					{
						if (output_selected)
							error_exit(&pr_desc, "output selected again", true);
					
						if (set_param_from_argv(&argc, &argv, &pr_desc.output_file) == -1)
							error_exit(&pr_desc, "for parametr -o expected only 1 parametr", true);

						output_selected = true;
						break;
					}
				case 'b': 
					{
						if (bytes_selected)
							error_exit(&pr_desc, "bytes selected again", true);
						
						char **strs_buf;
						size_t strs_buf_size;
						if (set_params_from_argv(&argc, &argv, &strs_buf, &strs_buf_size) == -1)
							error_exit(&pr_desc, "for parametr -b expected >1 parametrs", true);

						pr_desc.bytes_size = strs_buf_size;
						pr_desc.bytes = (bytes_buffer_t **)malloc(sizeof(bytes_buffer_t *) * pr_desc.bytes_size);
						size_t i;
						for (i = 0; i < strs_buf_size; i++) {
							size_t str_len = strlen(strs_buf[i]);
							bytes_buffer_t *buf = bbuffer_from_string(strs_buf[i], str_len);
							if (buf == NULL) 
								error_exit(&pr_desc, "incorrect bytes", false);

							pr_desc.bytes[i] = buf;
						}

						qsort(pr_desc.bytes, pr_desc.bytes_size, sizeof(bytes_buffer_t *), bbuffer_compare);

						bytes_selected = true;
						break;
					}
				case 'h':
					help(pr_desc.name);
					break;
				default:
					error_exit(&pr_desc, "unknown parametr", true);
					break;
			}
		}
		else {
		
		}
	}

	if (!mode_selected)
		error_exit(&pr_desc, "mode not selected", true);
	if (!input_selected)
		error_exit(&pr_desc, "missing input file", true);
	if (pr_desc.mode == M_PRINT && (output_selected || bytes_selected))
		error_exit(&pr_desc, "invalid parametrs for -p", true);
	if (pr_desc.mode == M_REMOVE && (!output_selected || !bytes_selected))
		error_exit(&pr_desc, "invalid parametrs for -r", true);
	return pr_desc;
}

void help(const char *pname) {
	printf("%s\n", pname);

	exit(0);
}

static int bbuffer_compare(const void *a, const void *b) {
	int f_arg = (*((bytes_buffer_t **)a))->size;
	int s_arg = (*((bytes_buffer_t **)b))->size;
	return s_arg - f_arg;
}
