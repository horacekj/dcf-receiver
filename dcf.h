#ifndef _DCF_H
#define _DCF_H

// dcf.c registers INT0_vect interrupt!

#define DCF_SURE_ALL 0x7FFFFFFFFFFFFFF
#define MIN_BITS 7
#define HR_BITS 6
#define DAY_BITS 6
#define DOW_BITS 3
#define MONTH_BITS 5
#define YEAR_BITS 8

typedef struct {
	uint64_t raw_sure;

	union {
		uint64_t raw;
		struct {
			bool message_start:1;
			uint16_t warning:14;
			bool abnormal:1;
			bool summer:1;
			bool cest:1;
			bool cet:1;
			bool leap:1;
			bool time_start:1;
			uint8_t min:MIN_BITS;
			bool min_parity:1;
			uint8_t hr:HR_BITS;
			bool hr_parity:1;
			uint8_t day:DAY_BITS;
			uint8_t day_of_week:DOW_BITS;
			uint8_t month:MONTH_BITS;
			uint8_t year:YEAR_BITS;
			bool date_parity:1;
		} parsed;
	} data;
} DcfDatetime;

void dcf_init(void);
void dcf_1ms_update(void);
void dcf_register_on_received(void(*fp)(DcfDatetime*));

#endif
