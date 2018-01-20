#include "rtc.h"
#include "i2c.h"

void rtc_set_clock(RTC data) {
	uint8_t i;
	uint8_t *ptr = &data.cntrl;
	I2C_Set_Address(0, 0);

	for (i = 0; i < 6; i++) {
		I2C_Write_B(*ptr);
		ptr++;
	}
	I2C_Stop();
}

RTC rtc_get_clock(void) {
	RTC data;

	I2C_Set_Address(0, 0);
	I2C_Stop();
	I2C_Set_Address(0, 1);
	I2C_Read_Block(7, &data.cntrl);
	I2C_Stop();

	return data;
}

void rtc_init(void) {
	TWBR = 100;                             /* Set bit rate register*/
	TWSR |= 1;                              /* Set clock dividet 4 */
}

