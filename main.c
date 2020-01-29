#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "bytes_buffer.h"
#include "bytes_fun.h"
#include "program_fun.h"
#include "list.h"

int bbuffers_comp(const void *a, const void *b) {
	int f_arg = ((*(bytes_buffer_t **)a))->size;
	int s_arg = ((*(bytes_buffer_t **)b))->size;
	printf("Compared: %d %d\n", f_arg, s_arg);
	return s_arg - f_arg;
}

int main(int argc, char **argv) {	
	program_description_t pr_desc = parse_argv(argc, argv);

	switch (pr_desc.mode) {
		case M_PRINT:
			bbuffer_print(*pr_desc.input_buffer);
			break;
		case M_REMOVE:
			{
				bytes_buffer_t *res;
				res = bbuffer_remove_bytes(*pr_desc.input_buffer, pr_desc.bytes, pr_desc.bytes_size);
				if (res == NULL)
					error_exit(&pr_desc, "can't remove bytes", false);
				write_file_bytes(pr_desc.output_file, *res);
				printf("Byte deleted: %llu\n", pr_desc.input_buffer->size - res->size);
				bytes_buffer_delete(res);
			}
			break;
		case M_SEARCH:
			{
				list_t *finded = bbuffer_search_bytes(*pr_desc.input_buffer, pr_desc.bytes, pr_desc.bytes_size);
				if (finded == NULL)
					error_exit(&pr_desc, "can't search bytes", false);


				if (list_count(*finded)) {
					printf("Bytes finded at position:\n");
					node_t *node;
					node = finded->head;
					size_t cur = 0;
					while (node != NULL) {
						printf("%08llX ", *((size_t *)node->data));
						if (cur != 0 && cur % 16 == 0)
							printf("\n");
						cur++;
						node = node->next;
					}
					if (cur % 17 != 0)
						printf("\n");
				}
				else
					printf("Bytes dont's finded\n");

				list_delete_with_data(finded);
			}
			break;
	}

	if (pr_desc.input_buffer != NULL) 
		bytes_buffer_delete(pr_desc.input_buffer);
	if (pr_desc.bytes != NULL) {
		size_t i;
		for (i = 0; i < pr_desc.bytes_size; i++)
			if (pr_desc.bytes[i] != NULL)
				free(pr_desc.bytes[i]);
		free(pr_desc.bytes);
		pr_desc.bytes = NULL;
		pr_desc.bytes_size = 0;
	}

	return 0;
}
