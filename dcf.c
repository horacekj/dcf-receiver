#include <avr/interrupt.h>
#include "dcf.h"

volatile int fall_edge_counter = 0;

// External interrupt from DCF77 pin
ISR(INT0_vect) {
	if (fall_edge_counter < 800)
		return;

	char str[8];
	itoa(fall_edge_counter, str, 10);
	fall_edge_counter = 0;
	serial_putline(str);
}

void dcf_1ms_update(void) {
	fall_edge_counter++;
	if (fall_edge_counter >= 5000)
		fall_edge_counter = 0;
}

void dcf_init(void) {
	DDRD &= ~((1 << PIN0) | (1 << PIN1)); // DCF data pins as inputs
	PORTD |= ((1 << PIN0) | (1 << PIN1)); // enable pull-ups
	EICRA |= (1 << ISC01); // set INT0 to falling edge
	EICRA &= ~(1 << ISC00); // set INT0 to falling edge
	EIMSK |= (1 << INT0); // enable INT0
}
