/*
 * joystick.h
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_JOYSTICK_H_
#define INC_DRV_JOYSTICK_H_

/********** Include **********/

/********** Define **********/

enum joystickIdType {
	JOYSTICK_L = 0,
	JOYSTICK_C,
	JOYSTICK_R,
	JOYSTICK_NUM
};

#define POS_NONE		0x00
#define POS_UP			0x01
#define POS_DOWN		0x02
#define POS_LEFT		0x04
#define POS_RIGHT		0x08
#define POS_UP_LEFT		(POS_UP | POS_LEFT)
#define POS_UP_RIGHT	(POS_UP | POS_RIGHT)
#define POS_DOWN_LEFT	(POS_DOWN | POS_LEFT)
#define POS_DOWN_RIGHT	(POS_DOWN | POS_RIGHT)

#define ADC_MAX 			0x0fff
#define POS_THRESHOLD 		256

#define POS_UP_THRESHOLD	POS_THRESHOLD
#define POS_DOWN_THRESHOLD	(ADC_MAX - POS_THRESHOLD)
#define POS_LEFT_THRESHOLD	POS_THRESHOLD
#define POS_RIGHT_THRESHOLD	(ADC_MAX - POS_THRESHOLD)

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvJoystickInit(void);
void DrvJoystickSetConfig(ADC_HandleTypeDef * hadc, ADC_ChannelConfTypeDef * sConfig);
void DrvJoystickRead(void);
uint8_t DrvJoystickGetSimplePos(uint8_t joystick_id);

#endif /* INC_DRV_JOYSTICK_H_ */
