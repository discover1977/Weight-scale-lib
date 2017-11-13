/*
 * main.c
 *
 *  Created on: 11 но€б. 2017 г.
 *      Author: gavrilov.iv
 */

#include <avr/eeprom.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "MAX72xx.h"
#include "HX711.h"
#include "weight_scales.h"

#define CALIBRATION_AVERAGE	50
#define MES_AVERAGE	5

#define BUTTON_PRESS	BitIsClear(PINA, 6)

enum Code {
	Release,
	Short,
	Long
};

volatile uint8_t 	ButtonCode = Release;

struct EEPROMData {
	uint8_t Init;
	float CalibrationFactor;
} Param;

void save_eeprom()
{
	cli();
	eeprom_update_block( (uint8_t*)&Param, 0, sizeof( Param ) );
	sei();
}

void set_zero(uint8_t average)
{
	MAX72xx_Clear(0);
	MAX72xx_Clear(1);
	MAX72xx_OutSym("SEt ", 4);
	MAX72xx_OutSym("nULL", 8);
	_delay_ms(2000);
	WSCALE_SetZero(CALIBRATION_AVERAGE);
	MAX72xx_OutSym("--------", 8);
	MAX72xx_Clear(0);
	MAX72xx_Clear(1);
}

void calibration(uint8_t average)
{
	MAX72xx_OutSym("-  -", 8);
	Param.CalibrationFactor = WSCALES_Calibrate(1286, CALIBRATION_AVERAGE);
	MAX72xx_OutSym("--------", 8);
	MAX72xx_Clear(0);
	MAX72xx_Clear(1);
}

ISR(TIMER0_COMP_vect)
{
	static uint16_t Cnt = 0;
	TCNT0 = 0x00;

	if(BUTTON_PRESS) {
		if(Cnt < 2000) Cnt++;
		if (Cnt >= 500) ButtonCode = Long;
	}
	else {
		if((Cnt >= 50) && (Cnt <= 200)) ButtonCode = Short;
		Cnt = 0;
	}

}

int main()
{
	int32_t Weigth = 0;

	MAX72xx_Init(7);
	WSCALES_Init();

	// Port init
	SetBit(PORTA, 6);

    //  Timer 0 Initialization
    TCCR0 = ( 1 << CS02 ) | ( 0 << CS01 ) | ( 1 << CS00 );
    TCNT0 = 0x00;
    OCR0 = 0x26;
    TIMSK = ( 1 << OCIE0 );

	eeprom_read_block( (uint8_t*)&Param, 0, sizeof( Param ) );

	set_zero(CALIBRATION_AVERAGE);

	if(Param.Init == 0xFF) {
		MAX72xx_Clear(0);
		MAX72xx_Clear(1);
		MAX72xx_OutSym("CAL-", 4);
		_delay_ms(5000);
		calibration(CALIBRATION_AVERAGE);
		Param.Init = 0x01;
		save_eeprom();
	}
	else {
		WSCALES_SetCalibrationFactor(Param.CalibrationFactor);
	}

	sei();

	while(1)
	{
		Weigth = WSCALES_GetWeight(MES_AVERAGE);
		if (abs(Weigth) < 1000) {
			MAX72xx_OutInt(0, Weigth, 0);
		}
		else {
			MAX72xx_OutInt(0, Weigth, 4);
		}

		if( ButtonCode == Short ) {
			while(BUTTON_PRESS);
			set_zero(CALIBRATION_AVERAGE);
			ButtonCode = Release;
		}
		if( ButtonCode == Long ) {
			MAX72xx_Clear(0);
			MAX72xx_Clear(1);
			MAX72xx_OutSym("CAL-", 4);
			_delay_ms(5000);
			while(BUTTON_PRESS);
			calibration(CALIBRATION_AVERAGE);
			ButtonCode = Release;
		}
	}
}
