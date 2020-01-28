#include <stdio.h>
#include "file.h"

int main(int argc, char **argv) {
	struct bytes_buffer *buffer;
	buffer = read_file_bytes("text.txt");
	if (!buffer) {
		printf("Error read\n");
		return -1;
	}

	printf("Bytes: %lu\n", buffer->size);
	size_t i;
	for (i = 0; i < buffer->size; i++)
		printf("%d\n", buffer->buffer[i]);

	write_file_bytes("text1.txt", buffer);
	return 0;
}
