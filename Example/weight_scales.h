/*
 * weight_scales.h
 *
 *  Created on: 30 ����. 2017 �.
 *      Author: gavrilov.iv
 */

#ifndef WEIGHT_SCALES_H_
#define WEIGHT_SCALES_H_

#include <stdint.h>

#define SHIFT_HX711_DATA	0

void WSCALES_Init();
uint32_t WSCALE_SetZero(uint8_t average);
void WSCALES_SetCalibrationFactor(float value);
float WSCALES_Calibrate(uint16_t weight, uint8_t average);
float WSCALES_GetWeight(uint8_t average);
uint32_t WSCALES_GetADCData(uint8_t average);

#endif /* WEIGHT_SCALES_H_ */
