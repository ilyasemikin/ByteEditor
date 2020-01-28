#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "hex_char.h"
#include "bytes_fun.h"

bool bbuffer_contain_bytes(bytes_buffer_t buffer, size_t offset, bytes_buffer_t bytes) {
	if (offset + bytes.size > buffer.size)
		return false;
	size_t i;
	for (i = 0; i < bytes.size; i++)
		if (buffer.buffer[offset + i] != bytes.buffer[i])
			return false;
	return true;
}

bytes_buffer_t *bbuffer_remove_bytes(bytes_buffer_t buffer, bytes_buffer_t *bytes, size_t bytes_size) {
	bytes_buffer_t *ret;
	ret = bytes_buffer_create(buffer.size);
	if (ret == NULL)
		return NULL;

	size_t ret_i = 0;
	size_t i;
	for (i = 0; i < buffer.size; i++) {
		size_t j;
		for (j = 0; j < bytes_size; j++) {
			if (bbuffer_contain_bytes(buffer, i, bytes[j])) {
				i += bytes[j].size - 1;
				ret->size -= bytes[j].size;
			}
			else
				ret->buffer[ret_i++] = buffer.buffer[i];
		}
	}

	ret->buffer = (byte *)realloc(ret->buffer, sizeof(byte) * ret->size);
	if (ret->buffer == NULL)
		return NULL;
	return ret;
}

int bbuffer_print(bytes_buffer_t buffer) {
	size_t offset;
	for (offset = 0; offset < buffer.size; offset++) {
		if (offset % 16 == 0)
			printf("%08lX ", offset);
		
		printf("%02X ", buffer.buffer[offset]);

		if (offset != 0 && offset % 15 == 0)
			printf("\n");
	}

	if (offset % 16 != 0)
		printf("\n");
	printf("%08lX\n", offset);

	return offset - 1;
}

bytes_buffer_t *bbuffer_from_string(const char *str, size_t len) {
	bytes_buffer_t *res = bytes_buffer_create(len / 2 + len % 2);
	uint8_t h_byte, l_byte;
	size_t res_i = 0;
	size_t i;
	for (i = 0; i < len; i += 2) {
		if (is_hex_char(str[i])) {
			if (len - i > 1) {
				h_byte = get_hex_value(str[i]);
				l_byte = get_hex_value(str[i + 1]);
			}
			else {
				h_byte = 0;
				l_byte = get_hex_value(str[i]);
			}
			res->buffer[res_i++] = (h_byte << 4) + l_byte;
		}
		else
			return NULL;
	}
	return res;
}
