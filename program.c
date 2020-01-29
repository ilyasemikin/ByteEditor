#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "program.h"
#include "bytes_fun.h"
#include "file.h"

// Вспомогательные функции
static bytes_buffer_t *err_read_bytes(const char *p_name, const char *input_file);
static void err_write_bytes(const char *p_name, const char *output_file, bytes_buffer_t *buf);
static void bytes_delete(bytes_buffer_t **bytes, size_t bytes_size);
static int bytes_get_from_string(char **strs, size_t strs_size, bytes_buffer_t ***res, size_t *res_size);
static int bbuffer_compare(const void *a, const void *b);

void print(const char *p_name, int argc, char **argv) {
	if (argc != 1)
		error_exit_help(p_name, "incorrect use command print");
	const char *input_file = *argv;
	bytes_buffer_t *buf = err_read_bytes(p_name, input_file);
	bbuffer_print(*buf);
	bytes_buffer_delete(buf);
}

void remove_each_occur(const char *p_name, int argc, char **argv) {
	if (argc < 3)
		error_exit_help(p_name, "incorrect use command remeach");
	const char *input_file = argv[0];
	const char *output_file = argv[1];
	argv += 2;

	bytes_buffer_t *in_buf = err_read_bytes(p_name, input_file);

	bytes_buffer_t **bytes;
	size_t bytes_size = argc - 2; 
	if (bytes_get_from_string(argv, bytes_size, &bytes, &bytes_size) == -1) {
		bytes_buffer_delete(in_buf);
		error_exit(p_name, "error prepare bytes");
	}

	qsort(bytes, bytes_size, sizeof(bytes_buffer_t *), bbuffer_compare);

	bytes_buffer_t *out_buf = bbuffer_remove_bytes(*in_buf, bytes, bytes_size);
	if (out_buf == NULL)
		error_exit(p_name, "error remove bytes");

	size_t bytes_remove = in_buf->size - out_buf->size;
	free(in_buf);

	err_write_bytes(p_name, output_file, out_buf);

	bytes_delete(bytes, bytes_size);
	bytes_buffer_delete(out_buf);

	printf("Bytes remove: %llu\n", bytes_remove);
}

void search(const char *p_name, int argc, char **argv) {
	if (argc < 2)
		error_exit_help(p_name, "incorrect use command search");

	const char *input_file = argv[0];
	argv++;

	bytes_buffer_t *in_buf = err_read_bytes(p_name, input_file);

	bytes_buffer_t **bytes;
	size_t bytes_size;
	bytes_size = argc - 1;

	if (bytes_get_from_string(argv, bytes_size, &bytes, &bytes_size) == -1) {
		bytes_buffer_delete(in_buf);
		error_exit(p_name, "error prepare bytes");
	}

	list_t *finded = bbuffer_search_bytes(*in_buf, bytes, bytes_size);
	if (finded == NULL)
		printf("error count bytes\n");
		
	size_t finded_count = list_count(*finded);

	if (finded_count == 0)
		printf("not found\n");
	else {
		size_t index = 0;
		node_t *p = finded->head;
		while (p != NULL) {
			if (index != 0 && index % 16 == 0)
				putchar('\n');
			printf("%08llX ", *((size_t *)p->data));
			p = p->next;
		}
		putchar('\n');
	}

	list_delete_with_data(finded);
}

static bytes_buffer_t *err_read_bytes(const char *p_name, const char *input_file) {
	bytes_buffer_t *ret = read_file_bytes(input_file);
	if (ret == NULL) {
		char error_msg[100];
		snprintf(error_msg, sizeof(error_msg), "error read bytes from file \'%s\'", input_file);
		error_exit(p_name, error_msg);
	}
	return ret;
}

static void err_write_bytes(const char *p_name, const char *output_file, bytes_buffer_t *buf) {
	if (write_file_bytes(output_file, *buf) == -1) {
		bytes_buffer_delete(buf);
		char error_msg[100];
		snprintf(error_msg, sizeof(error_msg), "error write file \'%s\'", output_file);
		error_exit(p_name, error_msg);
	}
}

static void bytes_delete(bytes_buffer_t **bytes, size_t bytes_size) {
	size_t i;
	for (i = 0; i < bytes_size; i++)
		free(bytes[i]);
	free(bytes);
}

static int bytes_get_from_string(char **strs, size_t strs_size, bytes_buffer_t ***res, size_t *res_size) {
	bytes_buffer_t **bytes;
	size_t bytes_size;
	bytes_size = strs_size;
	bytes = (bytes_buffer_t **)malloc(sizeof(bytes_buffer_t) * strs_size);

	if (bytes == NULL)
		return -1;
	size_t i;
	for (i = 0; i < bytes_size; i++) {
		bytes[i] = bbuffer_from_string(strs[i], strlen(strs[i]));
		if (bytes[i] == NULL) {
			bytes_delete(bytes, i);
			return -1;
		}
	}

	*res = bytes;
	*res_size = bytes_size;

	return 0;
}

static int bbuffer_compare(const void *a, const void *b) {
	int f_arg = (*((bytes_buffer_t **)a))->size;
	int s_arg = (*((bytes_buffer_t **)b))->size;
	return s_arg - f_arg;
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

void error_exit(const char *p_name, const char *msg) {
	printf("%s: %s\n", p_name, msg);
	exit(1);
}

void error_exit_help(const char *p_name, const char *msg) {
	printf("%s: %s\n", p_name, msg);
	printf("Try \'%s help\' for more info\n", p_name);
	exit(1);
}

void help(const char *pname) {
	printf("%s [MODE] [OPTION] ... [-i] path\n\n", pname);

	printf("modes:\n");
	print_parametr_description("-p", "print file contens in hex");
	print_parametr_description("-r", "remove bytes");
	print_parametr_description("-s", "search bytes in file");

	printf("\n");

	print_parametr_description("-i [path]", "input file path");
	print_parametr_description("-o [path]", "output file path");
	print_parametr_description("-b [bytes]", "bytes");

	exit(0);
}
