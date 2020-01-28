#include <stdlib.h>
#include "byte.h"

bytes_buffer_t *bytes_buffer_create(size_t size) {
	bytes_buffer_t *buffer;
	buffer = (bytes_buffer_t *)malloc(sizeof(bytes_buffer_t));
	buffer->size = size;
	buffer->buffer = (byte *)malloc(sizeof(byte) * size);
	return buffer;
}

void bytes_buffer_delete(bytes_buffer_t *buffer) {
	if (buffer == NULL)
		return;
	if (buffer->buffer != NULL)
		free(buffer->buffer);
	free(buffer);
}

