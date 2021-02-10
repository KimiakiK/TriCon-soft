/*
 * joystick.c
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "drv_joystick.h"
#include "stm32f3xx_hal.h"

/********** Define **********/

#define DRV_JOYSTICK_ADC_TIMEOUT	((uint32_t)10)

enum {
	ID_ADC1 = 0,
	ID_ADC2,
	ADC_NUM
};

enum {
	JOYSTICK_ADC_L_V = 0,
	JOYSTICK_ADC_L_H,
	JOYSTICK_ADC_C_V,
	JOYSTICK_ADC_C_H,
	JOYSTICK_ADC_R_V,
	JOYSTICK_ADC_R_H,
	JOYSTICK_ADC_NUM
};

/********** Type **********/

struct joystickAdcIdTableType {
	uint8_t V;
	uint8_t H;
};

struct joystickAdcConfigType {
	uint8_t id;
	uint8_t ch;
};

/********** Constant **********/

const struct joystickAdcIdTableType joystickAdcIdTable[JOYSTICK_NUM] = {
		{JOYSTICK_ADC_L_V, JOYSTICK_ADC_L_H}, /* JOYSTICK_L */
		{JOYSTICK_ADC_C_V, JOYSTICK_ADC_C_H}, /* JOYSTICK_C */
		{JOYSTICK_ADC_R_V, JOYSTICK_ADC_R_H}  /* JOYSTICK_R */
};

const struct joystickAdcConfigType joystickAdcConfig[JOYSTICK_ADC_NUM] = {
		{ID_ADC1, ADC_CHANNEL_2}, /* JOYSTICK_ADC_L_V */
		{ID_ADC1, ADC_CHANNEL_1}, /* JOYSTICK_ADC_L_H */
		{ID_ADC1, ADC_CHANNEL_4}, /* JOYSTICK_ADC_C_V */
		{ID_ADC1, ADC_CHANNEL_3}, /* JOYSTICK_ADC_C_H */
		{ID_ADC2, ADC_CHANNEL_2}, /* JOYSTICK_ADC_R_V */
		{ID_ADC2, ADC_CHANNEL_1}  /* JOYSTICK_ADC_R_H */
};

/********** Variable **********/

static ADC_HandleTypeDef * adcHandle[ADC_NUM];
static ADC_ChannelConfTypeDef adcChannelConfig[ADC_NUM];
static uint16_t rawValues[JOYSTICK_ADC_NUM];

/********** Function Prototype **********/

static uint16_t drvJoystickGetAdcValue(uint8_t joystick_id);

/********** Function **********/

void DrvJoystickInit(void)
{
	uint8_t joystick_id;

	for (joystick_id=0; joystick_id<JOYSTICK_ADC_NUM; joystick_id++) {
		rawValues[joystick_id] = 0;
	}
}

void DrvJoystickSetConfig(ADC_HandleTypeDef * hadc, ADC_ChannelConfTypeDef * sConfig)
{
	uint8_t id;

	if (hadc->Instance == ADC1) {
		id = ID_ADC1;
	} else {
		id = ID_ADC2;
	}

	adcHandle[id] = hadc;

	adcChannelConfig[id].Channel = sConfig->Channel;
	adcChannelConfig[id].Rank = sConfig->Rank;
	adcChannelConfig[id].SingleDiff = sConfig->SingleDiff;
	adcChannelConfig[id].SamplingTime = sConfig->SamplingTime;
	adcChannelConfig[id].OffsetNumber = sConfig->OffsetNumber;
	adcChannelConfig[id].Offset = sConfig->Offset;
}

void DrvJoystickRead(void) /* 124us */
{
	uint8_t joystick_id;

	for (joystick_id = 0; joystick_id < JOYSTICK_ADC_NUM; joystick_id ++) {
		rawValues[joystick_id] = drvJoystickGetAdcValue(joystick_id);
	}
}

uint8_t DrvJoystickGetSimplePos(uint8_t joystick_id)
{
	uint8_t pos;

	pos = SIMPLE_NONE;

	if (joystick_id < JOYSTICK_NUM) {
		/* JUDGE: UP */
		if (joystickAdcIdTable[joystick_id].H < SIMPLE_UP_THRESHOLD) {
			pos |= SIMPLE_UP;
		}/* JUDGE: DOWN */
		else if (joystickAdcIdTable[joystick_id].H > SIMPLE_DOWN_THRESHOLD) {
			pos |= SIMPLE_DOWN;
		}
		/* JUDGE: LEFT */
		if (joystickAdcIdTable[joystick_id].V < SIMPLE_LEFT_THRESHOLD) {
			pos |= SIMPLE_LEFT;
		}/* JUDGE: RIGHT */
		else if (joystickAdcIdTable[joystick_id].V > SIMPLE_RIGHT_THRESHOLD) {
			pos |= SIMPLE_RIGHT;
		}
	}

	return pos;
}

static uint16_t drvJoystickGetAdcValue(uint8_t joystick_id)
{
	uint16_t adc_value;

	adcChannelConfig[joystickAdcConfig[joystick_id].id].Channel = joystickAdcConfig[joystick_id].ch;

	HAL_ADC_ConfigChannel(adcHandle[joystickAdcConfig[joystick_id].id], &(adcChannelConfig[joystickAdcConfig[joystick_id].id]));
	HAL_ADC_Start(adcHandle[joystickAdcConfig[joystick_id].id]);
	HAL_ADC_PollForConversion(adcHandle[joystickAdcConfig[joystick_id].id], DRV_JOYSTICK_ADC_TIMEOUT);
	adc_value = HAL_ADC_GetValue(adcHandle[joystickAdcConfig[joystick_id].id]);
	HAL_ADC_Stop(adcHandle[joystickAdcConfig[joystick_id].id]);

	return adc_value;
}
