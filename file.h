#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "byte.h"

bytes_buffer_t *read_file_bytes(const char *filename);
int write_file_bytes(const char *filename, const bytes_buffer_t *buffer);
