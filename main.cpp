#include <avr/io.h>
#include <util/delay.h>
#include "shift.h"
#include "rtc.h"

void Hw_init(void);
void DevBoardShiftLedOut(uint8_t);

int main() {
	RTC rtc;
	rtc.mockup();
	Hw_init();

	while (1) {
		auto clock = rtc.get();                // Read new values of RTC
		// Is there some time difference ?
//		if (clock.s_old != ((clock.s_10 << 4) | clock.s_1)) {
//			clock.s_old = (clock.s_10 << 4) | clock.s_1; // <Y> Store new value
			DevBoardShiftLedOut(~*((&clock.cntrl) + 1));           // send value to bargraph
			_delay_ms(100);
//		}
	}
}

void Hw_init(void) {
	/* serial clock -> output pin, serial data -> output pin */
	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR); /* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);     /* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);     /* serial data stream -> output pin */
 }

void DevBoardShiftLedOut(uint8_t val) {
	uint8_t i=8;

	SHIFT_DDR |= _BV(SHIFT_LED_SET_DDR); /* parallel write -> output pin */
	SHIFT_DDR |= _BV(SHIFT_CLK_DDR);     /* serial clock -> output pin */
	SHIFT_DDR |= _BV(SHIFT_OUT_DDR);     /* serial data stream -> output pin */

	SHIFT_PORT &= ~(_BV(SHIFT_LED_SET)); /* define start values */
	SHIFT_PORT &= ~(_BV(SHIFT_CLK));

	while(i--) { /* send data to 74595, MSB (most significant bit) first */
		if( val & (1 << i) )
			SHIFT_PORT |= _BV(SHIFT_OUT);
		else
			SHIFT_PORT &= ~(_BV(SHIFT_OUT));

		SHIFT_PORT |= _BV(SHIFT_CLK);   /* 0 -> 1 data valid */

		asm volatile("nop\n\t"); /* wait */
		SHIFT_PORT &= ~(_BV(SHIFT_CLK));
	}

	asm volatile("nop\n\t"); /* wait */
	SHIFT_PORT |= _BV(SHIFT_LED_SET); /* 0 -> 1 parallel write */
}
