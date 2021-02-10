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
#define SW_ON			0x00
#define SW_OFF			0x01

/********** Type **********/

struct swConfigType {
	uint8_t sw_id;
	uint8_t sw_pos;
	GPIO_TypeDef * type;
	uint16_t pin;
};

/********** Constant **********/

const struct swConfigType swConfig[SW_NUM] = {
	{SW_L, PUSH_A, L_SW_A_GPIO_Port, L_SW_A_Pin},
	{SW_L, PUSH_B, L_SW_B_GPIO_Port, L_SW_B_Pin},
	{SW_L, PUSH_C, L_SW_C_GPIO_Port, L_SW_C_Pin},
	{SW_L, PUSH_D, L_SW_D_GPIO_Port, L_SW_D_Pin},
	{SW_C, PUSH_A, C_SW_A_GPIO_Port, C_SW_A_Pin},
	{SW_C, PUSH_B, C_SW_B_GPIO_Port, C_SW_B_Pin},
	{SW_C, PUSH_C, C_SW_C_GPIO_Port, C_SW_C_Pin},
	{SW_C, PUSH_D, C_SW_D_GPIO_Port, C_SW_D_Pin},
	{SW_R, PUSH_A, R_SW_A_GPIO_Port, R_SW_A_Pin},
	{SW_R, PUSH_B, R_SW_B_GPIO_Port, R_SW_B_Pin},
	{SW_R, PUSH_C, R_SW_C_GPIO_Port, R_SW_C_Pin},
	{SW_R, PUSH_D, R_SW_D_GPIO_Port, R_SW_D_Pin}
};

/********** Variable **********/

static uint8_t swState[SW_ID_NUM];
static uint8_t swFixedValue[SW_NUM];
static uint8_t swRawValue[SW_NUM];

/********** Function Prototype **********/

/********** Function **********/

void DrvSwInit(void)
{
	uint8_t sw_id;
	uint8_t sw_num;

	for (sw_id=0; sw_id<SW_ID_NUM; sw_id++) {
		swState[sw_id] = PUSH_NONE;
	}

	for (sw_num = 0; sw_num < SW_NUM; sw_num++) {
		swFixedValue[sw_num] = SW_OFF;
		swRawValue[sw_num] = SW_INVALID;
	}
}

/* swStateは3回一致で確定 */
void DrvSwReadFirst(void)
{
	uint8_t sw_num;

	for (sw_num = 0; sw_num < SW_NUM; sw_num++) {
		swRawValue[sw_num] = HAL_GPIO_ReadPin(swConfig[sw_num].type, swConfig[sw_num].pin);
	}
}

void DrvSwReadMid(void)
{
	uint8_t sw_num;
	uint8_t readValue;

	for (sw_num = 0; sw_num < SW_NUM; sw_num++) {
		readValue = HAL_GPIO_ReadPin(swConfig[sw_num].type, swConfig[sw_num].pin);
		if (readValue != swRawValue[sw_num]) {
			swRawValue[sw_num] = SW_INVALID;
		}
	}
}

void DrvSwReadEnd(void)
{
	uint8_t sw_num;
	uint8_t sw_id;

	DrvSwReadMid();

	for (sw_id=0; sw_id<SW_ID_NUM; sw_id++) {
		swState[sw_id] = PUSH_NONE;
	}

	for (sw_num = 0; sw_num < SW_NUM; sw_num++) {
		/* SW確定 */
		if (swRawValue[sw_num] != SW_INVALID) {
			swFixedValue[sw_num] = swRawValue[sw_num];
		}
		/* SW状態作成 */
		if (swFixedValue[sw_num] == SW_ON) {
			swState[swConfig[sw_num].sw_id] |= swConfig[sw_num].sw_pos;
		}
	}

}

uint8_t DrvSwGetState(uint8_t sw_id)
{
	return swState[sw_id];
}
