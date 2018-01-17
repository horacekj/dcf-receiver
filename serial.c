#include <avr/io.h>
#include "serial.h"

void serial_init(uint16_t baud) {
	/* set baud rate */
	UBRR0H = (uint8_t)(baud>>8);
	UBRR0L = (uint8_t) baud;

	/* enable receiver and transmitter */
	UCSR0B = (1<<RXEN)|(1<<TXEN);

	/* set frame format: 8 data, 2 stop bits */
	UCSR0C = (1<<USBS)|(3<<UCSZ0);
}

void serial_putch(uint8_t c) {
	/* wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE)) );

	/* put data into buffer and sends the data */
	UDR0 = c;
}

void serial_putstr(char* c) {
	while (*c) {
		serial_putch(*c);
		c++;
	}
}

void serial_putline(char* c) {
	serial_putstr(c);
	serial_putch('\r');
	serial_putch('\n');
}

uint8_t serial_getch(void) {
	/* wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );

	/* get and return received data from buffer */
	return UDR0;
}
