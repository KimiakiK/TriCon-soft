/*
 * joystick.c
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "drv_joystick.h"
#include "stdlib.h"
#include "math.h"

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
		{ID_ADC1, ADC_CHANNEL_1}, /* JOYSTICK_ADC_L_V */
		{ID_ADC1, ADC_CHANNEL_2}, /* JOYSTICK_ADC_L_H */
		{ID_ADC1, ADC_CHANNEL_3}, /* JOYSTICK_ADC_C_V */
		{ID_ADC1, ADC_CHANNEL_4}, /* JOYSTICK_ADC_C_H */
		{ID_ADC2, ADC_CHANNEL_1}, /* JOYSTICK_ADC_R_V */
		{ID_ADC2, ADC_CHANNEL_2}  /* JOYSTICK_ADC_R_H */
};

/********** Variable **********/

static ADC_HandleTypeDef * adcHandle[ADC_NUM];
static ADC_ChannelConfTypeDef adcChannelConfig[ADC_NUM];
static uint16_t rawValues[JOYSTICK_ADC_NUM];

/********** Function Prototype **********/

static uint16_t drvJoystickGetAdcValue(uint8_t joystick_id);
static void setSeed(void);

/********** Function **********/

void DrvJoystickInit(void)
{
	uint8_t joystick_id;

	for (joystick_id=0; joystick_id<JOYSTICK_ADC_NUM; joystick_id++) {
		rawValues[joystick_id] = 0;
	}

	setSeed();
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

	pos = POS_NONE;

	if (joystick_id < JOYSTICK_NUM) {
		/* JUDGE: UP */
		if (rawValues[joystickAdcIdTable[joystick_id].V] < POS_UP_THRESHOLD) {
			pos |= POS_UP;
		}/* JUDGE: DOWN */
		else if (rawValues[joystickAdcIdTable[joystick_id].V] > POS_DOWN_THRESHOLD) {
			pos |= POS_DOWN;
		}
		/* JUDGE: LEFT */
		if (rawValues[joystickAdcIdTable[joystick_id].H] < POS_LEFT_THRESHOLD) {
			pos |= POS_LEFT;
		}/* JUDGE: RIGHT */
		else if (rawValues[joystickAdcIdTable[joystick_id].H] > POS_RIGHT_THRESHOLD) {
			pos |= POS_RIGHT;
		}
	}

	return pos;
}

vector_t DrvJoystickGetAnalogPos(uint8_t joystick_id)
{
	vector_t pos;
	float norm;

	/* -1.0 〜 1.0の範囲に変換 */
	pos.x = ((float)(rawValues[joystickAdcIdTable[joystick_id].H] * 2) / (float)ADC_MAX) - 1.0f;
	pos.y = ((float)(rawValues[joystickAdcIdTable[joystick_id].V] * 2) / (float)ADC_MAX) - 1.0f;

	/* ベクトルを正規化して位置データに変換 */
	norm = sqrtf(pos.x * pos.x + pos.y * pos.y);
	if (norm > ANALOG_THRESHOLD){
		pos.x = pos.x / norm * fabsf(pos.x);
		pos.y = pos.y / norm * fabsf(pos.y);
	} else {
		pos.x = 0.0f;
		pos.y = 0.0f;
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

static void setSeed(void)
{
	uint16_t seed;
	uint8_t joystick_id;

	seed = 0;

	for (joystick_id = 0; joystick_id < JOYSTICK_ADC_NUM; joystick_id ++) {
		seed += drvJoystickGetAdcValue(joystick_id);
	}

	srand(seed);
}
