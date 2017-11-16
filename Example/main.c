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

#define CALIBRATION_AVERAGE	25
#define MES_AVERAGE	5
#define	OFF_TIMEOUT	1000

#define BUTTON_PRESS	BitIsClear(PIND, 2)

enum Code {
	Release,
	Short,
	Long
};

volatile uint8_t 	ButtonCode = Release;
volatile uint16_t	OffTimer = 0;
volatile int32_t Weigth = 0;
volatile uint8_t Power = 1;
volatile uint8_t PreOffCalibration = 0;

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

ISR(TIMER0_COMPA_vect)
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
	if(Weigth <= 0) {
		if(OffTimer < OFF_TIMEOUT) {
			if(++OffTimer == OFF_TIMEOUT) {
				MAX72xx_Clear(0);
				PreOffCalibration = 1;
				Power = 0;
			}
		}
	}
	else {
		OffTimer = 0;
	}
}

int main()
{
	MAX72xx_Init(7);
	WSCALES_Init();

	// Port init
	SetBit(PORTD, 2);

    //  Timer 0 Initialization
	TCCR0A = 0x00;
	TCCR0B = ( 1 << CS02 ) | ( 0 << CS01 ) | ( 1 << CS00 );
    TCNT0 = 0x00;
    OCR0A = 0x26;
    TIMSK0 = ( 1 << OCIE0A );

	eeprom_read_block( (uint8_t*)&Param, 0, sizeof( Param ) );

	MAX72xx_OutSym("--------", 8);
	WSCALE_SetZero(CALIBRATION_AVERAGE);
	MAX72xx_OutSym("   --   ", 8);

	OffTimer = 0;
	Power = 1;

	if(Param.Init == 0x01) {
		WSCALES_SetCalibrationFactor(Param.CalibrationFactor);
	}

	MAX72xx_Clear(0);

	sei();

	while(1)
	{
		if(Param.Init == 0x01) {
			Weigth = WSCALES_GetWeight(MES_AVERAGE);
			if(Power == 1) {
				MAX72xx_OutIntFormat(Weigth, 3, 8, 6);
			}
			else {
				if (Weigth > 50) {
					MAX72xx_Clear(0);
					for(int i = 0; i < 4; i++) {
						_delay_ms(250);
						MAX72xx_OutSym("--------", 8);
						_delay_ms(250);
						MAX72xx_OutSym("        ", 8);
					}
					MAX72xx_OutSym("--------", 8);
					WSCALE_SetZero(CALIBRATION_AVERAGE);
					MAX72xx_OutSym("   --   ", 8);
					Power = 1;
				}
				if (Weigth < 0) {
					MAX72xx_SetComma(1, 1);
					WSCALE_SetZero(CALIBRATION_AVERAGE);
					MAX72xx_SetComma(1, 0);
				}
			}
		}
		else {
			MAX72xx_OutSym("________", 8);
		}

		if(PreOffCalibration) {
			WSCALE_SetZero(CALIBRATION_AVERAGE);
			PreOffCalibration = 0;
		}

		if( ButtonCode == Short ) {
			while(BUTTON_PRESS);
			MAX72xx_OutSym("--------", 8);
			WSCALE_SetZero(CALIBRATION_AVERAGE);
			MAX72xx_OutSym("   --   ", 8);
			_delay_ms(500);
			MAX72xx_Clear(0);
			ButtonCode = Release;
		}

		if( ButtonCode == Long ) {
			MAX72xx_Clear(0);
			MAX72xx_OutSym("===CA===", 8);
			while(BUTTON_PRESS);
			_delay_ms(1000);
			MAX72xx_OutSym(" ==CA== ", 8);
			_delay_ms(1000);
			MAX72xx_OutSym("  =CA=  ", 8);
			_delay_ms(1000);
			MAX72xx_OutSym("   CA   ", 8);
			_delay_ms(1000);
			Param.CalibrationFactor = WSCALES_Calibrate(1286, CALIBRATION_AVERAGE);
			Param.Init = 0x01;
			MAX72xx_OutSym("= Good =", 8);
			_delay_ms(500);
			MAX72xx_Clear(0);
			ButtonCode = Release;
			save_eeprom();
		}
	}
}
