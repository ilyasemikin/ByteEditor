#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte_t;

struct bytes_buffer {
	byte_t *buffer;
	size_t size;
};

typedef struct bytes_buffer bytes_buffer_t;

bytes_buffer_t *bytes_buffer_create(size_t length);
void bytes_buffer_delete(bytes_buffer_t *buffer);
