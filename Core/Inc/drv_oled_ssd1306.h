/*
 * drv_oled_ssd1306.h
 *
 *  Created on: Mar 15, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_OLED_SSD1306_H_
#define INC_DRV_OLED_SSD1306_H_

/********** Include **********/

#include "stm32f3xx_hal.h"

/********** Define **********/

#define OLED_HEIGHT		64
#define OLED_WIDTH		128

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

extern uint8_t DisplayMemory[OLED_WIDTH][OLED_HEIGHT];

/********** Function Prototype **********/

void DrvOledInit(void);
void DrvOledSetConfig(I2C_HandleTypeDef* hi2c);
void DrvOledDisplay(void);

#endif /* INC_DRV_OLED_SSD1306_H_ */
