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

enum {
	SW_L_A = 0,
	SW_L_B,
	SW_L_C,
	SW_L_D,
	SW_C_A,
	SW_C_B,
	SW_C_C,
	SW_C_D,
	SW_R_A,
	SW_R_B,
	SW_R_C,
	SW_R_D,
	SW_NUM
};

enum {
	SW_OFF = 0,
	SW_ON
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

/********** Function Prototype **********/

void DrvSwInit(void);
void DrvSwReadFirst(void);
void DrvSwReadMid(void);
void DrvSwReadEnd(void);

#endif /* DRIVER_SW_H_ */
