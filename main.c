#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "serial.h"
#include "dcf.h"

void hw_init(void);

ISR(TIMER1_COMPA_vect) {
	dcf_1ms_update();
}

int main(void) {
	hw_init();
	serial_putline("Device initialized");

	while (true);
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
