#include <ctype.h>
#include "hex_char.h"

bool is_hex_char(char c) {
	c = tolower(c);
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

uint8_t get_hex_value(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	switch (c) {
		case 'A': case 'a':
			return 10;
		case 'B': case 'b':
			return 11;
		case 'C': case 'c':
			return 12;
		case 'D': case 'd':
			return 13;
		case 'E': case 'e':
			return 14;
		case 'F': case 'f':
			return 15;
	}
	return 0;
}
