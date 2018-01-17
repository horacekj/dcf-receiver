#ifndef _SERIAL_H
#define _SERIAL_H

#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define DEFAULT_BAUD 9600

void serial_init(uint16_t baud = UART_BAUD_SELECT(DEFAULT_BAUD, F_CPU));
void serial_putch(uint8_t c);
uint8_t ser_getch(void);

#endif
