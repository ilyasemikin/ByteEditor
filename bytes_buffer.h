#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte;

struct bytes_buffer {
	byte *buffer;
	size_t size;
};

typedef struct bytes_buffer bytes_buffer_t;

bytes_buffer_t *bytes_buffer_create(size_t length);
void bytes_buffer_delete(bytes_buffer_t *buffer);
