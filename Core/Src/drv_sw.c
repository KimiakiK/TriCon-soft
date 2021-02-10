/*
 * sw.c
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "drv_sw.h"
#include "stm32f3xx_hal.h"
#include "main.h"

/********** Define **********/

#define SW_INVALID		0xFF

/********** Type **********/

struct swConfigType {
	GPIO_TypeDef * type;
	uint16_t pin;
};

/********** Constant **********/

const struct swConfigType swConfig[SW_NUM] = {
	{L_SW_A_GPIO_Port, L_SW_A_Pin},
	{L_SW_B_GPIO_Port, L_SW_B_Pin},
	{L_SW_C_GPIO_Port, L_SW_C_Pin},
	{L_SW_D_GPIO_Port, L_SW_D_Pin},
	{C_SW_A_GPIO_Port, C_SW_A_Pin},
	{C_SW_B_GPIO_Port, C_SW_B_Pin},
	{C_SW_C_GPIO_Port, C_SW_C_Pin},
	{C_SW_D_GPIO_Port, C_SW_D_Pin},
	{R_SW_A_GPIO_Port, R_SW_A_Pin},
	{R_SW_B_GPIO_Port, R_SW_B_Pin},
	{R_SW_C_GPIO_Port, R_SW_C_Pin},
	{R_SW_D_GPIO_Port, R_SW_D_Pin}
};

/********** Variable **********/

static uint8_t swState[SW_NUM];
static uint8_t swValue[SW_NUM];

/********** Function Prototype **********/

/********** Function **********/

void DrvSwInit(void)
{
	uint8_t sw_id;

	for (sw_id = 0; sw_id < SW_NUM; sw_id++) {
		swState[sw_id] = SW_OFF;
		swValue[sw_id] = SW_INVALID;
	}
}

/* swStateは3回一致で確定 */
void DrvSwReadFirst(void)
{
	uint8_t sw_id;

	for (sw_id = 0; sw_id < SW_NUM; sw_id++) {
		swValue[sw_id] = HAL_GPIO_ReadPin(swConfig[sw_id].type, swConfig[sw_id].pin);
	}
}

void DrvSwReadMid(void)
{
	uint8_t sw_id;
	uint8_t readValue;

	for (sw_id = 0; sw_id < SW_NUM; sw_id++) {
		readValue = HAL_GPIO_ReadPin(swConfig[sw_id].type, swConfig[sw_id].pin);
		if (readValue != swValue[sw_id]) {
			swValue[sw_id] = SW_INVALID;
		}
	}
}

void DrvSwReadEnd(void)
{
	uint8_t sw_id;

	DrvSwReadMid();

	for (sw_id = 0; sw_id < SW_NUM; sw_id++) {
		if (swValue[sw_id] != SW_INVALID) {
			swState[sw_id] = swValue[sw_id];
		}
	}
}
