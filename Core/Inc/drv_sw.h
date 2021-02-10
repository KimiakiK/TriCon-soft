/*
 * sw.h
 *
 *  Created on: Mar 12, 2020
 *      Author: kimi
 */

#ifndef DRIVER_SW_H_
#define DRIVER_SW_H_

/********** Include **********/

/********** Define **********/

#define PUSH_NONE	0x00
#define PUSH_A		0x01
#define PUSH_B		0x02
#define PUSH_C		0x04
#define PUSH_D		0x08

enum swIdType {
	SW_L = 0,
	SW_C,
	SW_R,
	SW_ID_NUM
};

#define SW_NUM		12

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvSwInit(void);
void DrvSwReadFirst(void);
void DrvSwReadMid(void);
void DrvSwReadEnd(void);
uint8_t DrvSwGetState(uint8_t sw_id);

#endif /* DRIVER_SW_H_ */
