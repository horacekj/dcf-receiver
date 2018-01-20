#ifndef _BCD_H
#define _BCD_H

#include <stdlib.h>

uint8_t bcd_num_to_dec(uint8_t num, size_t low_len, size_t high_len) {
	return ((num >> low_len) & (0xFF >> (8-high_len))) * 10 + (num & (0xFF >> (8-low_len)));
}

#endif
