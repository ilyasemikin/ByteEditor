#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "bytes_buffer.h"

// read_file_bytes
// Создать буффер байт из файла
// Возвращает NULL в случае провала
bytes_buffer_t *read_file_bytes(const char *filename);

// write_file_bytes
// Создать файл на основе буффера байт
// Возвращает 0 в случае успеха, -1 - в случае провала 
int write_file_bytes(const char *filename, const bytes_buffer_t buffer);
