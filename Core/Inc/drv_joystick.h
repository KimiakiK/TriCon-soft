/*
 * joystick.h
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_JOYSTICK_H_
#define INC_DRV_JOYSTICK_H_

/********** Include **********/

#include "stm32f3xx_hal.h"

/********** Define **********/

enum {
	JOYSTICK_L = 0,
	JOYSTICK_C,
	JOYSTICK_R,
	JOYSTICK_NUM
};

#define SIMPLE_NONE			0x00
#define SIMPLE_UP			0x01
#define SIMPLE_DOWN			0x02
#define SIMPLE_LEFT			0x04
#define SIMPLE_RIGHT		0x08
#define SIMPLE_UP_LEFT		(SIMPLE_UP | SIMPLE_LEFT)
#define SIMPLE_UP_RIGHT		(SIMPLE_UP | SIMPLE_RIGHT)
#define SIMPLE_DOWN_LEFT	(SIMPLE_DOWN | SIMPLE_LEFT)
#define SIMPLE_DOWN_RIGHT	(SIMPLE_DOWN | SIMPLE_RIGHT)

#define ADC_MAX 				0x0fff

#define SIMPLE_THRESHOLD 		256
#define SIMPLE_UP_THRESHOLD		SIMPLE_THRESHOLD
#define SIMPLE_DOWN_THRESHOLD	(ADC_MAX - SIMPLE_THRESHOLD)
#define SIMPLE_LEFT_THRESHOLD	(ADC_MAX - SIMPLE_THRESHOLD)
#define SIMPLE_RIGHT_THRESHOLD	SIMPLE_THRESHOLD

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvJoystickInit(void);
void DrvJoystickSetConfig(ADC_HandleTypeDef * hadc, ADC_ChannelConfTypeDef * sConfig);
void DrvJoystickRead(void);
uint8_t DrvJoystickGetSimplePos(uint8_t joystick_id);

#endif /* INC_DRV_JOYSTICK_H_ */
