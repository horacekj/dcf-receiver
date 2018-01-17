#ifndef _RTC_H
#define _RTC_H

#include "i2c.h"

typedef struct {
	uint8_t cntrl;     /*! \brief Control register */
	uint8_t s_1_100:4; /*! \brief ._x seconds */
	uint8_t s_1_10:4;  /*! \brief .x seconds */
	uint8_t s_1:4;     /*! \brief x. seconds */
	uint8_t s_10:4;    /*! \brief x_.seconds */
	uint8_t m_1:4;     /*! \brief x minutes */
	uint8_t m_10:4;    /*! \brief x_ minutes */
	uint8_t h_1:4;     /*! \brief x hours */
	uint8_t h_10:4;    /*! \brief x_ hours */
	uint8_t d_1:4;     /*! \brief x days */
	uint8_t d_10:4;    /*! \brief x_ days */
	uint8_t stat;      /*! \brief Status register */
	uint8_t s_old;     /*! \brief Old value of seconds counter (temporary) */
} RTCState;

class RTC {
public:
	RTCState state;

	RTC() {
		I2C_DDR |= _BV(I2C_SCL) | _BV(I2C_SDA);
		P_SDA_ON;
		P_SCL_ON;
		state.cntrl = 0x80;
		set();
		state.cntrl = 0;
		set();
	}

	void mockup() {
		state.s_1_100 = 0;
		state.s_1_10 = 0;
		state.s_1 = 5;
		state.s_10 = 10;
		state.m_1 = 0;
		state.m_10 = 4;
		state.h_1 = 0;
		state.h_10 = 0;
		state.d_1 = 0;
		state.d_10 = 0;
		set();                    /* Set RTC registers */
	}

	void set() {
		uint8_t i;
		uint8_t *ptr = &state.cntrl;                    /* Set temporary pointer */
		I2C_Stop();                                     /* Generate stop */
		I2C_Set_Address(0,0);                           /* Set low address and func, write */
		for (i = 0; i < 6; i++) {
			I2C_Write_B(*ptr);                          /* Write 6 byte to RTC */
			ptr++;
		}
		I2C_Stop();                                     /* Generate stop */
	}

	RTCState& get() {
		I2C_Stop();                                     /* Generate stop condition */
		I2C_Set_Address(0,0);                           /* Set RTC address to 0, func. write */
		I2C_Start();
		I2C_Set_Address(0,1);                           /* Set RTC address to 0, func. read */
		I2C_Read_Block(6, &state.cntrl);                  /* Read first 6 byte from RTC and store data to RTC structure */
		I2C_Stop();                                     /* Generate stop condition */
		return this->state;
	}

};

#endif
