/*
 * drv_oled_ssd1306.h
 *
 *  Created on: Mar 15, 2020
 *      Author: kimi
 */

#ifndef INC_DRV_OLED_SSD1306_H_
#define INC_DRV_OLED_SSD1306_H_

/********** Include **********/

/********** Define **********/

#define DOT_BLACK		0
#define DOT_WHITE		1

#define OLED_HEIGHT		64
#define OLED_WIDTH		128

#define MAP_VERTICAL(x, y)		((x) + (y) * OLED_HEIGHT)
#define MAP_HORIZONTAL(x, y)	((x) * OLED_HEIGHT + (y))

enum angleModeType {
	OLED_ANGLE_CENTER = 0,
	OLED_ANGLE_LEFT,
	OLED_ANGLE_RIGHT
};

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

extern uint8_t DisplayMemory[OLED_WIDTH * OLED_HEIGHT];

/********** Function Prototype **********/

void DrvOledInit(void);
void DrvOledSetConfig(I2C_HandleTypeDef* hi2c);
void DrvOledSetDisplayAngle(uint8_t angle);
void DrvOledDisplay(void);
void DrvOledClearMemory(void);

#endif /* INC_DRV_OLED_SSD1306_H_ */
