#include <stdio.h>
#include <stdlib.h>
#include "file.h"
#include "bytes_fun.h"

int main(int argc, char **argv) {
	bytes_buffer_t *buffer;
	buffer = read_file_bytes("text.txt");
	if (!buffer) {
		printf("Error read\n");
		return -1;
	}

	printf("Read:\n");
	bbuffer_print(*buffer);

	return 0;
}
