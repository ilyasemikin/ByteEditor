#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "hex_char.h"
#include "bytes_fun.h"

static bool bbuffer_contain_bytes(const bytes_buffer_t buffer, size_t offset, bytes_buffer_t bytes) {
	if (offset + bytes.size > buffer.size)
		return false;
	size_t i;
	for (i = 0; i < bytes.size; i++)
		if (buffer.buffer[offset + i] != bytes.buffer[i])
			return false;
	return true;
}

static long long bbuffer_contain_one_of_bytes(const bytes_buffer_t buffer, size_t offset, bytes_buffer_t **bytes, size_t bytes_size) {
	size_t j;
	for (j = 0; j < bytes_size; j++)
		if (bbuffer_contain_bytes(buffer, offset, *bytes[j]))
			return j;
	return -1;
}

bytes_buffer_t *bbuffer_remove_bytes(const bytes_buffer_t buffer, bytes_buffer_t **bytes, size_t bytes_size) {
	bytes_buffer_t *ret;
	ret = bytes_buffer_create(buffer.size);
	if (ret == NULL)
		return NULL;

	size_t ret_i = 0;
	size_t i;
	for (i = 0; i < buffer.size; i++) {
		long long finded = bbuffer_contain_one_of_bytes(buffer, i, bytes, bytes_size);
		if (finded != -1) {
			ret->size -= bytes[finded]->size;
			i -= bytes[finded]->size - 1;
		}
		else
			ret->buffer[ret_i++] = buffer.buffer[i];
	}

	ret->buffer = (byte_t *)realloc(ret->buffer, sizeof(byte_t) * ret->size);
	if (ret->buffer == NULL)
		return NULL;
	return ret;
}

list_t *bbuffer_search_bytes(bytes_buffer_t buffer, bytes_buffer_t **bytes, size_t bytes_size) {
	list_t *ret;
	ret = list_create();
	if (ret == NULL)
		return NULL;

	size_t i;
	for (i = 0; i < buffer.size; i++) {
		size_t j;
		for (j = 0; j < bytes_size; j++)
			if (bbuffer_contain_bytes(buffer, i, *bytes[j])) {
				size_t *value;
				value = (size_t *)malloc(sizeof(size_t));
				if (value == NULL) {
					list_delete_with_data(ret);
					return NULL;
				}
				*value = i;
				list_add_end(ret, value);
			}
	}

	return ret;
}

int bbuffer_print(bytes_buffer_t buffer) {
	size_t offset;
	for (offset = 0; offset < buffer.size; offset++) {
		if (offset != 0 && offset % 16 == 0)
			printf("\n");
		if (offset % 16 == 0)
			printf("%08lX ", offset);
		
		printf("%02X ", buffer.buffer[offset]);

	}

	printf("\n%08lX\n", offset);

	return offset - 1;
}

bytes_buffer_t *bbuffer_from_string(const char *str, size_t len) {
	size_t i;
	for (i = 0; i < len; i++)
		if (!is_hex_char(str[i]))
			return NULL;

	bytes_buffer_t *res = bytes_buffer_create(len / 2 + len % 2);
	if (res == NULL)
		return NULL;
	size_t res_i = 0;
	if (len % 2) 
		res->buffer[res_i++] = get_hex_value(str[0]);
	for (i = len % 2; i < len; i += 2) {
		uint8_t h_byte, l_byte;
		h_byte = get_hex_value(str[i]);
		l_byte = get_hex_value(str[i + 1]);
		res->buffer[res_i++] = (h_byte << 4) + l_byte;
	}
	return res;
}
