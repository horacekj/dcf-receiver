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
volatile bool show_time = false;

ISR(TIMER1_COMPA_vect) {
	dcf_1ms_update();
}

void dcf_on_received(volatile DcfDatetime* dt) {
	dcf_stop();

	RTC rtc;
	rtc.cntrl = 0x80;
	rtc.s_1_100 = 0;
	rtc.s_1_10 = 0;
	rtc.s_1 = 0;
	rtc.s_10 = 0;
	rtc.m_1 = dt->data.parsed.min & 0x0F;
	rtc.m_10 = (dt->data.parsed.min >> 4) & 0x07;
	rtc.h_1 = dt->data.parsed.hr & 0x0F;
	rtc.h_10 = (dt->data.parsed.hr >> 4) & 0x03;
	rtc.d_1 = dt->data.parsed.day & 0x0F;
	rtc.d_10 = (dt->data.parsed.day >> 4) & 0x03;
	rtc.mon_1 = dt->data.parsed.month & 0x0F;
	rtc.mon_10 = (dt->data.parsed.month >> 4) & 0x01;

	rtc_set_clock(rtc);
	rtc.cntrl = 0;
	rtc_set_clock(rtc);

	show_time = true;
	PORTD |= 0x8F;
}

void rtc_send_dt(RTC dt) {
	char str[32];

	sprintf(str, "%d.%d. %d:%d:%d",
		dt.d_1 + 10*dt.d_10,
		dt.mon_1 + 10*dt.mon_10,
		dt.h_1 + 10*dt.h_10,
		dt.m_1 + 10*dt.m_10,
		dt.s_1 + 10*dt.s_10
	);
	serial_putline(str);
}

int main(void) {
	hw_init();
	serial_putline("Device initialized.");
	serial_putline("Waiting for beginning of a minute...");

	while (true) {
		if (show_time)
			rtc_send_dt(rtc_get_clock());

		for(int i = 0; i < 5; i++)
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
	dcf_start();
	dcf_register_on_received(&dcf_on_received);
	timer_init();
	rtc_init();

	sei(); // allow interrupts globally
}
