#include <avr/interrupt.h>
#include "dcf.h"

const uint16_t FALL_EDGE_TIME_MAX = 3000;

volatile uint16_t fall_edge_counter = 0;
volatile int8_t bit_index = -1;

// External interrupt from DCF77 pin.
ISR(INT0_vect) {
	if (fall_edge_counter < 800) {
		// Ignore sooner falling edge. We hope ve did not loose sync.
		return;
	}
	if (fall_edge_counter >= FALL_EDGE_TIME_MAX) {
		// Falling edge too late -> we loosed sync -> reset counter.
		fall_edge_counter = 0;
		return;
	}

	char str[8];
	itoa(fall_edge_counter, str, 10);
	fall_edge_counter = 0;
	serial_putline(str);
}

void dcf_1ms_update(void) {
	// Do do increment when limit reached.
	if (fall_edge_counter < FALL_EDGE_TIME_MAX)
		fall_edge_counter++;
}

void dcf_init(void) {
	DDRD &= ~((1 << PIN0) | (1 << PIN1)); // DCF data pins as inputs
	PORTD |= ((1 << PIN0) | (1 << PIN1)); // enable pull-ups
	EICRA |= (1 << ISC01); // set INT0 to falling edge
	EICRA &= ~(1 << ISC00); // set INT0 to falling edge
	EIMSK |= (1 << INT0); // enable INT0
}
