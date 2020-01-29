#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argv_fun.h"
#include "program_fun.h"
#include "bytes_fun.h"
#include "file.h"

void program_description_clean(program_description_t *pr_desc) {
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
}

void error_exit(program_description_t *pr_desc, const char *msg, bool help_msg) {
	printf("%s: %s\n", pr_desc->name, msg);
	if (help_msg)
		printf("Try \'%s -h\' for more info\n", pr_desc->name);

	program_description_clean(pr_desc);
	
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
				// Это выглядит не очень хорошо, но пока так
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
				case 's':
					if (mode_selected)
						error_exit(&pr_desc, "mode selected again", true);
					pr_desc.mode = M_SEARCH;
					mode_selected = true;
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
					help(&pr_desc);
					break;
				default:
					{
						char buf[20];
						snprintf(buf, sizeof(buf), "unknown parametr -%c", str_argv[1]);
						error_exit(&pr_desc, buf, true);
					}
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
	if (pr_desc.mode == M_SEARCH && (output_selected || !bytes_selected))
		error_exit(&pr_desc, "invalid parametrs for -s", true);
	return pr_desc;
}

// Вывод параметра на экран в виде таблицы
static void print_parametr_description(const char *p_name, const char *p_desc) {
	const size_t length = 100;
	const size_t p_name_offset = 1;
	const size_t p_desc_offset = 30;

	size_t p_name_len = strlen(p_name);
	size_t p_desc_len = strlen(p_desc);

	size_t p_name_part_len = p_desc_offset - p_name_offset - p_name_len;
	size_t p_desc_part_len = length - p_desc_offset;
	
	// Вывод параметра
	size_t i;
	for (i = 0; i < p_name_offset; i++)
		putchar(' ');

	printf("%s", p_name);

	for (i = 0; i < p_name_part_len; i++)
		putchar(' ');

	size_t p_desc_i = 0;
	size_t count_lines = p_desc_len / p_desc_part_len + (p_desc_len % p_desc_part_len ? 1 : 0);
	size_t line;
	// Вывод описания параметра
	for (line = 0; line < count_lines; line++) {
		size_t count_chars = (line == count_lines - 1) ? p_desc_len - p_desc_i : p_desc_part_len;
		for (i = 0; i < count_chars; i++)
			putchar(p_desc[p_desc_i++]);
		putchar('\n');
		if (line != count_lines - 1)
			for (i = 0; i < p_desc_offset; i++)
				putchar(' ');
	}
	if (count_lines == 0)
		putchar('\n');
}

void help(program_description_t *pr_desc) {
	printf("%s [MODE] [OPTION] ... [-i] path\n\n", pr_desc->name);

	printf("modes:\n");
	print_parametr_description("-p", "print file contens in hex");
	print_parametr_description("-r", "remove bytes");
	print_parametr_description("-s", "search bytes in file");

	printf("\n");

	print_parametr_description("-i [path]", "input file path");
	print_parametr_description("-o [path]", "output file path");
	print_parametr_description("-b [bytes]", "bytes");

	program_description_clean(pr_desc);

	exit(0);
}

static int bbuffer_compare(const void *a, const void *b) {
	int f_arg = (*((bytes_buffer_t **)a))->size;
	int s_arg = (*((bytes_buffer_t **)b))->size;
	return s_arg - f_arg;
}
