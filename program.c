#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "program.h"
#include "bytes_buffer.h"
#include "bytes_fun.h"
#include "hex_char.h"
#include "file.h"

// Вспомогательные функции, объявление

// err_read_bytes
// Попытка прочить байты файла
// В случае провала завершает работу программы с выводом сообщения на экран
static bytes_buffer_t *err_read_bytes(const char *p_name, const char *input_file);

// err_write_bytes
// Попытка создать файл на основе буфера байт
// В случае провла завершает работу программы с выводом сообщения на экран
static void err_write_bytes(const char *p_name, const char *output_file, bytes_buffer_t *buf);

// bytes_delete
// Освобождения ресурсов, занимаемых массивом bytes 
static void bytes_delete(bytes_buffer_t **bytes, size_t bytes_size);

// bytes_get_from_string
// Создания массива байт на основе строк strs
// Сохраняет результат в переменные res и res_size
// Возвращает 0 в случае успеха
static int bytes_get_from_string(char **strs, size_t strs_size, bytes_buffer_t ***res, size_t *res_size);

// bbuffer_compare
// Фукнция для qsort, для сортировки в порядке убывания
static int bbuffer_compare(const void *a, const void *b);

// Конец блока объевления вспомагательных функций

void print(const char *p_name, int argc, char **argv) {
	const char *input_file = *argv;
	bytes_buffer_t *buf = err_read_bytes(p_name, input_file);
	bbuffer_print(*buf);
	bytes_buffer_delete(buf);
}

void remove_byte(const char *p_name, int argc, char **argv) {
	const char *input_file = argv[0];
	const char *output_file = argv[1];
	const char *offset_str = argv[2];
	const char *count_str = argv[3];

	if (!is_hex_str(offset_str))
		error_exit(p_name, "position bytes incorrect");
	if (!is_hex_str(count_str))
		error_exit(p_name, "count bytes incorrect");

	unsigned long long offset, count;
	sscanf(offset_str, "%llx", &offset);
	sscanf(count_str, "%llx", &count);

	bytes_buffer_t *buf = err_read_bytes(p_name, input_file);

	if (offset > buf->size || offset + count > buf->size)
		error_exit(p_name, "error");

	size_t cp_block_size = buf->size - (offset + count);
	const void *src = &buf->buffer[offset + count];
	void *dst = &buf->buffer[offset];
	memcpy(dst, src, sizeof(byte_t) * cp_block_size);
	
	buf->size -= count;
	buf->buffer = (byte_t *)realloc(buf->buffer, sizeof(byte_t) * buf->size);
	if (buf->buffer == NULL) {
		free(buf);
		error_exit(p_name, "error realloc buffer");
	}

	err_write_bytes(p_name, output_file, buf);

	bytes_buffer_delete(buf);
}

void remove_each_occur(const char *p_name, int argc, char **argv) {
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
	bytes_buffer_delete(in_buf);

	err_write_bytes(p_name, output_file, out_buf);

	bytes_delete(bytes, bytes_size);
	bytes_buffer_delete(out_buf);

	printf("Bytes remove: %llu\n", bytes_remove);
}

void search(const char *p_name, int argc, char **argv) {
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
			++index;
			p = p->next;
		}
		putchar('\n');
	}

	list_delete_with_data(finded);
}

// Вспомагательные функции, определение
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

void error_exit(const char *p_name, const char *msg) {
	printf("%s: %s\n", p_name, msg);
	exit(1);
}

void error_exit_help(const char *p_name, const char *msg) {
	printf("%s: %s\n", p_name, msg);
	printf("Try \'%s help\' for more information\n", p_name);
	exit(1);
}

static const size_t out_length = 100;
static const size_t out_name_offset = 3;
// Смещение описания/использования от левого края терминала
static const size_t out_info_offset = 30;
static void print_command_info(const char *name, const char *info);

void help(const char *p_name, command_desc_t *cmds, size_t cmds_size) {
	printf("%s [COMMAND] [OPTIONS] ... \n", p_name);

	putchar('\n');
	printf("Commands:\n");

	size_t i;
	for (i = 0; i < cmds_size; i++)
		print_command_info(cmds[i].name, cmds[i].desc);
	print_command_info("help", "print this help and exit");

	putchar('\n');
	printf("Commands usage:\n");
	for (i = 0; i < cmds_size; i++)
		print_command_info(cmds[i].name, cmds[i].usage);
	
	exit(0);
}

static void print_command_info(const char *name, const char *info) {
	size_t name_len = strlen(name);
	size_t info_len = strlen(info);
	
	size_t i;
	for (i = 0; i < out_name_offset; i++)
		putchar(' ');
	
	// Вывод названия команды
	size_t out_name_len; 
	out_name_len = name_len < out_info_offset ? name_len : out_info_offset - out_name_offset;
	for (i = 0; i < out_name_len; i++)
		putchar(name[i]);

	// Вывод промежутка между названием команды и информацией о команде
	size_t count_char_info = out_info_offset - out_name_len - out_name_offset;
	for (i = 0; i < count_char_info; i++)
		putchar(' ');

	// Вывод информации о команде
	size_t out_info_part_size = out_length - out_info_offset;
	size_t count_lines;
	count_lines = info_len / out_info_part_size + (info_len % out_info_part_size ? 1 : 0);
	if (count_lines == 0)
		putchar('\n');
	size_t line;
	for (line = 0; line < count_lines; line++) {
		size_t out_count_chars;
		out_count_chars = line == count_lines - 1 ? (info_len - out_info_part_size * (count_lines - 1)) : out_info_part_size;
		for (i = 0; i < out_count_chars; i++)
			putchar(info[line * out_info_part_size + i]);
		
		putchar('\n');

		// Вывод отступа при переносе информации на новую строку
		if (line != count_lines - 1)
			for (i = 0; i < out_info_offset; i++)
				putchar(' ');
	}
}
