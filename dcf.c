#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dcf.h"
#include "serial.h"

const uint16_t FALL_EDGE_TIME_MAX = 3000;
const uint8_t PULSE_MIN_RECOGNIZE_VAL = 40;

volatile uint16_t fall_edge_counter = 0;
volatile int8_t bit_index = -1;
volatile int16_t low_counter = -1;
volatile int8_t pulse_amplitude = 0; // length of "low" pulse
volatile DcfDatetime received;
volatile void(*callback)(volatile DcfDatetime*) = NULL;
volatile bool message_ready = false;

void evaluate_bit(int8_t state);
bool check_bit(bool state, int8_t index);
void time_complete(void);
bool check_min_parity(void);
bool check_hr_parity(void);
bool check_date_parity(void);
uint8_t bcd_min_to_int(uint8_t min);

// External interrupt from DCF77 pin.
ISR(INT3_vect) {
	if (fall_edge_counter < 800) {
		// Ignore sooner falling edge. We hope ve did not loose sync.
		return;
	}
	if (fall_edge_counter >= FALL_EDGE_TIME_MAX) {
		// Falling edge too late -> we loosed sync -> reset counter.
		fall_edge_counter = 0;
		serial_putline("Timeout!");
		return;
	}

	if (message_ready && callback) {
		(*callback)(&received);
		message_ready = false;
	}

	if ((bit_index >= 0) && (bit_index < 58)) {
		// New bit -> start measuring length of "low" state.
		bit_index++;
		low_counter = 0;
	}

	if ((fall_edge_counter >= 1950) && (fall_edge_counter <= 2050)) {
		// Beginning of a message.
		bit_index = 0;
		low_counter = 0;
	}


	fall_edge_counter = 0;
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

		if (low_counter > 120)
			pulse_amplitude += ((PIND >> 3) & 0x01) ? 1 : -1;

		if (low_counter == 200) {
			low_counter = -1;
			evaluate_bit(pulse_amplitude);
			pulse_amplitude = 0;
		}
	}
}

void evaluate_bit(int8_t state) {
	bool value, sure;
	if (state > PULSE_MIN_RECOGNIZE_VAL) {
		value = false;
		sure = true;
	} else if (state < -PULSE_MIN_RECOGNIZE_VAL) {
		value = true;
		sure = true;
	} else {
		sure = false;
	}

	if (sure) {
		received.raw_sure |= (1ULL << bit_index);
		if (value)
			received.data.raw |= (1ULL << bit_index);
		else
			received.data.raw &= ~(1ULL << bit_index);

		if (!check_bit(value, bit_index)) {
			// Message badly corrupted -> stop receiving message.
			bit_index = -1;
			serial_putline("Message parse error!");
		}
	} else {
		received.raw_sure &= ~(1ULL << bit_index);
		serial_putch('!');
	}

	char str[8];
	itoa(bit_index, str, 10);
	serial_putline(str);

	if (bit_index == 58)
		time_complete();
}

bool check_bit(bool state, int8_t index) {
	if (index == 0 && state) return false;
	if (index == 20 && !state) return false;

	return true;
}

void time_complete(void) {
	if (received.raw_sure != DCF_SURE_ALL) {
		serial_putline("Read error!");
		return;
	}

	if (!check_min_parity() || !check_hr_parity() || !check_date_parity()) {
		serial_putline("Parity error!");
		return;
	}

	message_ready = true;
}

bool check_min_parity(void) {
	bool parity = false;
	for(int i = 0; i < MIN_BITS; i++)
		if (((received.data.parsed.min >> i) & 0x1) == 0x1)
			parity = !parity;
	if (received.data.parsed.min_parity)
		parity = !parity;
	return !parity;
}

bool check_hr_parity(void) {
	bool parity = false;
	for(int i = 0; i < HR_BITS; i++)
		if (((received.data.parsed.hr >> i) & 0x1) == 0x1)
			parity = !parity;
	if (received.data.parsed.hr_parity)
		parity = !parity;
	return !parity;
}

bool check_date_parity(void) {
	bool parity = false;
	for(int i = 36; i < 59; i++)
		if (((received.data.raw >> i) & 0x1) == 0x1)
			parity = !parity;
	return !parity;
}

void dcf_init(void) {
	DDRD &= ~((1 << PIN3) | (1 << PIN4)); // DCF data pins as inputs
	PORTD |= ((1 << PIN3) | (1 << PIN4)); // enable pull-ups
	EICRA |= (1 << ISC31); // set INT3 to falling edge
	EICRA &= ~(1 << ISC30); // set INT3 to falling edge
	EIMSK |= (1 << INT3); // enable INT3

	received.raw_sure = 0;
}

void dcf_register_on_received(void(*fp)(volatile DcfDatetime*)) {
	callback = fp;
}
