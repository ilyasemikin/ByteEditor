#include <stdlib.h>
#include "bytes_buffer.h"

bytes_buffer_t *bytes_buffer_create(size_t size) {
	bytes_buffer_t *buffer;
	buffer = (bytes_buffer_t *)malloc(sizeof(bytes_buffer_t));
	buffer->size = size;
	buffer->buffer = (byte_t *)malloc(sizeof(byte_t) * size);
	return buffer;
}

void bytes_buffer_delete(bytes_buffer_t *buffer) {
	if (buffer == NULL)
		return;
	if (buffer->buffer != NULL)
		free(buffer->buffer);
	free(buffer);
}
