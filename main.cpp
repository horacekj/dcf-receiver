#include <avr/io.h>
#include <util/delay.h>
#include "serial.h"

void hw_init(void);

int main() {
	serial_init();

	while (true) {
		serial_putch('a');
		_delay_ms(200);
	}
}

void hw_init(void) {

}
