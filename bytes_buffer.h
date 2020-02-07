#pragma once

#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte_t;

struct bytes_buffer {
	byte_t *buffer;
	size_t size;
};

typedef struct bytes_buffer bytes_buffer_t;

// bytes_buffer_create
// Создает и инициализирует буффер байт размером length
// В случае провала возвращает NULL
bytes_buffer_t *bytes_buffer_create(size_t length);

// bytes_buffer_delete
// Освобождает буффер байт
// В случае передачи NULL не делает ничего
void bytes_buffer_delete(bytes_buffer_t *buffer);
