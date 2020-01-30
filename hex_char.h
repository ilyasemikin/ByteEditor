#pragma once

#include <stdbool.h>
#include <stdint.h>

bool is_hex_char(char c);
bool is_hex_str(const char *str);
uint8_t get_hex_value(char c);
