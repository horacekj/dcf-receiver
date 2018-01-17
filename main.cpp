#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "serial.h"

volatile int fall_edge_counter = 0;

void hw_init(void);

// External interrupt from DCF77 pin
ISR(INT0_vect) {
	if (fall_edge_counter < 800)
		return;

	char str[8];
	itoa(fall_edge_counter, str, 10);
	fall_edge_counter = 0;
	serial_putline(str);
}

ISR(TIMER1_COMPA_vect) {
	fall_edge_counter++;
	if (fall_edge_counter >= 5000)
		fall_edge_counter = 0;
}

int main() {
	hw_init();
	serial_putline("Device initialized");

	while (true);
}

void dcf_init(void) {
	DDRD &= ~((1 << PIN0) | (1 << PIN1)); // DCF data pins as inputs
	PORTD |= ((1 << PIN0) | (1 << PIN1)); // enable pull-ups
	EICRA |= (1 << ISC01); // set INT0 to falling edge
	EICRA &= ~(1 << ISC00); // set INT0 to falling edge
	EIMSK |= (1 << INT0); // enable INT0
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
	timer_init();

	sei(); // allow interrupts globally
}
