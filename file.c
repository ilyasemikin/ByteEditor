#include <stdio.h>
#include "file.h"

bytes_buffer_t *read_file_bytes(const char *filename) {
	if (filename == NULL)
		return NULL;

	FILE *file = fopen(filename, "rb");
	if (file == NULL)
		return NULL;
	
	fseek(file, 0L, SEEK_END);
	size_t buf_size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	bytes_buffer_t *buffer;
	buffer = bytes_buffer_create(buf_size);

	size_t read_bytes = fread(buffer->buffer, sizeof(byte), buffer->size, file);
	bool success = read_bytes == buffer->size;

	fclose(file);

	if (!success) {
		bytes_buffer_delete(buffer);
		return NULL;
	}
	return buffer;
}

int write_file_bytes(const char *filename, const bytes_buffer_t buffer) {
	if (filename == NULL)
		return -1;

	FILE *file = fopen(filename, "wb");
	if (file == NULL)
		return -1;

	size_t write_bytes = fwrite(buffer.buffer, sizeof(byte), buffer.size, file);
	bool success = write_bytes == buffer.size;

	fclose(file);
	if (!success)
		return -1;
	return 0;
}
