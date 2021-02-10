/*
 * sys_platform.h
 *
 *  Created on: 2020/07/28
 *      Author: kimi
 */

#ifndef INC_SYS_PLATFORM_H_
#define INC_SYS_PLATFORM_H_


/********** Include **********/

/********** Define **********/

#define MAIN_PERIODIC		33	/* メイン周期[ms] */

enum connectedControllerType {
	CONNECTED_NONE = 0x00,
	CONNECTED_L = 0x01,
	CONNECTED_C = 0x02,
	CONNECTED_R = 0x04
};

enum controllerIdType {			/* 並びをSW IDとJoystick IDと同じにすること */
	CONTROLLER_L = 0,
	CONTROLLER_C,
	CONTROLLER_R,
	CONTROLLER_NUM
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void SysPlatformInit(void);
void SysPlatformMain(void);

void SysSetNextApplication(uint8_t application_id);

uint8_t SysGetConnectedController(void);
uint8_t SysGetMainController(void);


#endif /* INC_SYS_PLATFORM_H_ */
