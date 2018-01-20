#ifndef _RTC_H
#define _RTC_H

#include "i2c.h"

typedef struct {
	uint8_t cntrl;
	uint8_t s_1_100:4;
	uint8_t s_1_10:4;
	uint8_t s_1:4;
	uint8_t s_10:4;
	uint8_t m_1:4;
	uint8_t m_10:4;
	uint8_t h_1:4;
	uint8_t h_10:4;
	uint8_t d_1:4;
	uint8_t d_10:4;
	uint8_t mon_1:4;
	uint8_t mon_10:4;
} RTC;

void rtc_set_clock(RTC data);
RTC rtc_get_clock(void);
void rtc_init(void);

#endif
