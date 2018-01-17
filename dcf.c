#include <avr/interrupt.h>
#include "dcf.h"

const uint16_t FALL_EDGE_TIME_MAX = 3000;
const uint8_t PULSE_MIN_RECOGNIZE_VAL = 70;

volatile uint16_t fall_edge_counter = 0;
volatile int8_t bit_index = -1;
volatile int16_t low_counter = -1;
volatile int8_t pulse_amplitude = 0; // length of "low" pulse
volatile DcfDatetime data;

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

	if ((bit_index > 0) && (bit_index < 59)) {
		// New bit -> start measuring lenght of "low" state.
		bit_index++;
		low_counter = 0;
	}

	if ((fall_edge_counter >= 1950) && (fall_edge_counter <= 2050)) {
		// Beginning of a message.
		bit_index = 0;
		low_counter = 0;
	}

	char str[8];
	itoa(fall_edge_counter, str, 10);
	fall_edge_counter = 0;
	serial_putline(str);
}

void dcf_1ms_update(void) {
	// Measuring time between 2 falling edges.
	// Do do increment when limit reached.
	if (fall_edge_counter < FALL_EDGE_TIME_MAX)
		fall_edge_counter++;

	// Measuring length of the "low" state.
	if (low_counter > -1) {
		low_counter++;
		if (low_counter == 100)
			pulse_amplitude = 0;

		if (low_counter > 100)
			pulse_amplitude += (PIND & 0x01) ? 1 : -1;

		if (low_counter == 200) {
			low_counter = -1;
			evaluate_bit(pulse_amplitude);
			pulse_amplitude = 0;
		}
	}
}

void evaluate_bit(int8_t state) {
	bool value;
	if (state > PULSE_MIN_RECOGNIZE_VAL)
		value = true;
	else if (state < -PULSE_MIN_RECOGNIZE_VAL)
		value = false;
	else {
		// Not sure what the data is.
	}
}

void dcf_init(void) {
	DDRD &= ~((1 << PIN0) | (1 << PIN1)); // DCF data pins as inputs
	PORTD |= ((1 << PIN0) | (1 << PIN1)); // enable pull-ups
	EICRA |= (1 << ISC01); // set INT0 to falling edge
	EICRA &= ~(1 << ISC00); // set INT0 to falling edge
	EIMSK |= (1 << INT0); // enable INT0
}
