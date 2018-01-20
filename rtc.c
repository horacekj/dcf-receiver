#include "rtc.h"
#include "i2c.h"
#include "serial.h"

void rtc_set_clock(RTC data) {
	uint8_t i;
	uint8_t *ptr = &data.cntrl;
	I2C_Set_Address(0, 0);
	char str[10];

	for (i = 0; i < 5; i++) {
		I2C_Write_B(*ptr);
		itoa(*ptr, str, 16);
		serial_putline(str);
		ptr++;
	}
	I2C_Stop();
}

RTC rtc_get_clock(void) {
	char str[10];
	RTC data;

	I2C_Set_Address(0, 0);
	I2C_Stop();
	I2C_Set_Address(0, 1);
	I2C_Read_Block(6, &data.cntrl);
	I2C_Stop();

	for (int i = 0; i < 5; i++) {
		itoa(*(&data.cntrl + i), str, 16);
		serial_putline(str);
	}

	return data;
}

void rtc_init(void) {
	TWBR = 100;                             /* Set bit rate register*/
	TWSR |= 1;                              /* Set clock dividet 4 */
}

