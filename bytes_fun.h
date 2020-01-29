#pragma once

#include "bytes_buffer.h"

bytes_buffer_t *bbuffer_remove_bytes(bytes_buffer_t buffer, bytes_buffer_t **bytes, size_t bytes_size);
int bbuffer_print(bytes_buffer_t buffer);
bytes_buffer_t *bbuffer_from_string(const char *str, size_t len);
