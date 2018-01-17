#ifndef _DCF_H
#define _DCF_H

// dcf.c registers INT0_vect interrupt!

typedef struct {
	uint8_t min;
	uint8_t hr;
	uint8_t day;
	uint8_t day_of_week;
	uint8_t month;
	uint8_t year;
} DcfDatetime;

void dcf_init(void);
void dcf_1ms_update(void);

#endif
