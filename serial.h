#ifndef _SERIAL_H
#define _SERIAL_H

#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)
#define DEFAULT_BAUD 9600

void serial_init_br(uint16_t baud);
void serial_init(void);
void serial_putch(uint8_t c);
void serial_putstr(char *str);
void serial_putline(char *str);
uint8_t ser_getch(void);

#endif
