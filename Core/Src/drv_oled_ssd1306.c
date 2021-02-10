/*
 * drv_oled_ssd1306.c
 *
 *  Created on: Mar 15, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "drv_oled_ssd1306.h"
#include "stm32f3xx_hal.h"

/********** Define **********/

#define SSD1306_I2C_ADDRESS		0x3C
#define DEV_ADDRESS				(SSD1306_I2C_ADDRESS << 1)

#define I2C_TIMEOUT				100 /* 100ms */

#define CONTROL_BYTE_COMMAND	0x00
#define CONTROL_BYTE_DATA		0x40

/********** Type **********/

typedef struct {
	uint8_t control;
	uint8_t display[OLED_WIDTH][OLED_HEIGHT / 8];
}txDataDisplayType;

/********** Constant **********/

const static uint8_t txDataSSD1306Initilize[] = {
		CONTROL_BYTE_COMMAND,
		0xAE,			/* Set Display ON/OFF:  Display OFF */
		0xA8, 0x3F,		/* Set MUX Ratio:  Default(63) */
		0xD3, 0x00,		/* Set Display Offset:  Default(0) */
		0x40,			/* Set Display Start Line:  Default(0) */
		0xA0,			/* Set Segment Re-map:  column address 0 is mapped to SEG0 */
		0xC0,			/* Set COM Output Scan Direction:  normal mode (RESET) Scan from COM0 to COM[N –1] */
		0xDA, 0x02,		/* Set COM Pins Hardware Configuration:  Sequential COM pin configuration, Disable COM Left/Right remap */
		0x81, 0x7F,		/* Set Contrast Control:  Default(127) */
		0xA4,			/* Entire Display ON:  Resume to RAM content display */
		0xA6,			/* Set Normal/Inverse Display:  Normal */
		0xD5, 0x80,		/* Set Display Clock Divide Ratio/Oscillator Frequency:  Default */
		0x8D, 0x14,		/* Charge Pump Setting:  Enable Charge Pump */
		0x20, 0x01,			/* Set Memory Addressing Mode:  Vertical Addressing Mode */
		0x21, 0x00, 0x7F,	/* Set Column Address:  Column start address(0), Column end address(127)  */
		0x22, 0x00, 0x07	/* Set Page Address:  Page start Address(0), Page end Address(7) */
};

const static uint8_t txDataSSD1306DisplayOn[] = {
		CONTROL_BYTE_COMMAND,
		0xAF			/* Set Display ON/OFF:  Display ON */
};

/********** Variable **********/

uint8_t DisplayMemory[OLED_WIDTH][OLED_HEIGHT];

static I2C_HandleTypeDef* i2cHandle;

static txDataDisplayType txDataDisplay;

/********** Function Prototype **********/

static void makeTxDataDisplay(void);

/********** Function **********/

void DrvOledInit(void) /* 32.6ms */
{
	uint8_t x, y;

	/* OLED SSD1306初期化 */
	HAL_I2C_Master_Transmit(i2cHandle, DEV_ADDRESS, (uint8_t*)txDataSSD1306Initilize, sizeof(txDataSSD1306Initilize), I2C_TIMEOUT);

	/* 画面を全領域黒で初期化 */
	for (x=0; x<OLED_WIDTH; x++) {
		for (y=0; y<OLED_HEIGHT; y++) {
			DisplayMemory[x][y] = 0;
		}
	}
	makeTxDataDisplay();
	HAL_I2C_Master_Transmit(i2cHandle, DEV_ADDRESS, (uint8_t*)&txDataDisplay, sizeof(txDataDisplay), I2C_TIMEOUT);

	/* Display ON */
	HAL_I2C_Master_Transmit(i2cHandle, DEV_ADDRESS, (uint8_t*)txDataSSD1306DisplayOn, sizeof(txDataSSD1306DisplayOn), I2C_TIMEOUT);
}

void DrvOledSetConfig(I2C_HandleTypeDef* hi2c)
{
	i2cHandle = hi2c;
}

void DrvOledDisplay(void)
{
	HAL_I2C_StateTypeDef i2cState;

	i2cState = HAL_I2C_GetState(i2cHandle);

	if (i2cState == HAL_I2C_STATE_READY) {
		makeTxDataDisplay();

		//HAL_I2C_Master_Transmit(i2cHandle, DEV_ADDRESS, (uint8_t*)&txDataDisplay, sizeof(txDataDisplay), I2C_TIMEOUT);
		/* Tx 25.3ms */
		HAL_I2C_Master_Transmit_DMA(i2cHandle, DEV_ADDRESS, (uint8_t*)&txDataDisplay, sizeof(txDataDisplay));
	}
}

static void makeTxDataDisplay(void) /* 3.23ms */
{
	uint8_t x, y;

	txDataDisplay.control = CONTROL_BYTE_DATA;

	for (x=0; x<OLED_WIDTH; x++) {
		for (y=0; y<OLED_HEIGHT / 8; y++) {
			txDataDisplay.display[x][y] =
					( (DisplayMemory[x][y*8+0] & 0x01) << 0) |
					( (DisplayMemory[x][y*8+1] & 0x01) << 1) |
					( (DisplayMemory[x][y*8+2] & 0x01) << 2) |
					( (DisplayMemory[x][y*8+3] & 0x01) << 3) |
					( (DisplayMemory[x][y*8+4] & 0x01) << 4) |
					( (DisplayMemory[x][y*8+5] & 0x01) << 5) |
					( (DisplayMemory[x][y*8+6] & 0x01) << 6) |
					( (DisplayMemory[x][y*8+7] & 0x01) << 7);
		}
	}
}
