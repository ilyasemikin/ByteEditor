#pragma once

#include "bytes_buffer.h"
#include "list.h"

// bbuffer_remove_bytes
// Создает новый буффер байт из buffer с удаленными байтами bytes
// Возвращает NULL в случае провала
bytes_buffer_t *bbuffer_remove_bytes(const bytes_buffer_t buffer, bytes_buffer_t **bytes, size_t bytes_size);

// bbuffer_search_bytes
// Поиск байт в буффере байт
// Возвращает список позиций в буффере байт, в которые входят байты bytes
// В случае провала возвращает NULL
list_t *bbuffer_search_bytes(bytes_buffer_t buffer, bytes_buffer_t **bytes, size_t bytes_size);

// bbuffer_print
// Вывод на экран буффера байт по 16 байт в строке со смещениями
// Возвращает число выведенных байт
size_t bbuffer_print(bytes_buffer_t buffer);

// bbuffer_from_string
// Создание буффер байт из строки
// Возвращает NULL в случае провала
bytes_buffer_t *bbuffer_from_string(const char *str, size_t len);
