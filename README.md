# Simple DCF77 Receiver

This project contains FW for ATmega128 at Charon II developer board. It
receives time synchronization signal from [DCF77 radio
station](https://en.wikipedia.org/wiki/DCF77).

Pinout:
 * RTC 8583: PD0 (SCL), PD1 (SDA), PD2 (INT)
 * DCF77 receiver: PD3

Program receives time from DCF77, stores it in RTC chip and then sends it
to PC. Because of missing simple buttons at developer board, new
synchronization request could only be triggered via RESET.

This project was created as a final work at PV198 course @ FI MUNI, autumn 2017
by Jan Horacek.
