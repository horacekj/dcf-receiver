#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include "serial.h"
#include "dcf.h"
#include "bcd.h"
#include "rtc.h"
#include "i2c.h"

void hw_init(void);

ISR(TIMER1_COMPA_vect) {
	dcf_1ms_update();
}

void dcf_on_received(volatile DcfDatetime* dt) {
	char str[32];

	sprintf(str, "%d.%d.%d %d:%d:00",
		bcd_num_to_dec(dt->data.parsed.day, 4, 2),
		bcd_num_to_dec(dt->data.parsed.month, 4, 1),
		bcd_num_to_dec(dt->data.parsed.year, 4, 4),
		bcd_num_to_dec(dt->data.parsed.hr, 4, 2),
		bcd_num_to_dec(dt->data.parsed.min, 4, 3)
	);
	serial_putline(str);
}

int main(void) {
	hw_init();
	serial_putline("Device initialized");

	RTC data;
	uint8_t *ptr = &data.cntrl;              /* Definition of ptr */

	*ptr++ = 0x80;                     /* Set control 32.768kHz, */
	*ptr++ = 0;                     /* Set hundredth of sec */   
	*ptr++ = 0x56;                  /* Set 56 secounds */
	*ptr++ = 0x34;                  /* Set 34 minutes */
	*ptr++ = 0x12;                  /* Set 12 hours */

	rtc_set_clock(data);
	data.cntrl = 0;
	rtc_set_clock(data);

	while (true) {
		if (GET_INT_STAT) {
			data = rtc_get_clock();
			char str[10];
			itoa(data.s_1, str, 10);
			serial_putline(str);
		}

		for(int i = 0; i < 4; i++)
			_delay_ms(200);
	}
}

void timer_init(void) {
	// Initialize timer 1 to 1 ms.

	const int T1_PRESC = 8;
	const int T1_FREQ = 1000;

	OCR1A = (F_CPU / (T1_PRESC * T1_FREQ)) - 1;
	TCCR1B |= (1 << WGM12) | (1 << CS11); // prescaler 8, CTC mode
	TIMSK |= (1 << OCIE1A); // enable interrupt OCIE1A
}

void hw_init(void) {
	serial_init();
	dcf_init();
	dcf_register_on_received(&dcf_on_received);
	timer_init();
	rtc_init();

	sei(); // allow interrupts globally
}
