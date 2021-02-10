/*
 * sys_platform.c
 *
 *  Created on: 2020/07/28
 *      Author: kimi
 */

/********** Include **********/

#include "sys_platform.h"
#include "stm32f3xx_hal.h"
#include "drv_sw.h"
#include "drv_joystick.h"
#include "drv_oled_ssd1306.h"
#include "apl_puzzle.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static uint32_t nextProcessingTick;

/********** Function Prototype **********/

/********** Function **********/

void SysPlatformInit(void)
{
#if 0
	/* デバッグ用テストピン */
	GPIOA->BSRR = 0x00000100; /* Center SW A (PA8) Output High */
	GPIOA->BSRR = 0x01000000; /* Center SW A (PA8) Output Low */
	GPIOA->MODER = (GPIOA->MODER & 0xFFFCFFFF) | 0x00010000; /* Center SW A (PA8)を汎用出力に変更 */
#endif

	/* Waiting for stability (OLED) */
	HAL_Delay(100);

	/* Initialize drivers */
	DrvSwInit();
	DrvJoystickInit();
	DrvOledInit();

	AplPuzzleInit();
}

void SysPlatformMain(void)
{
	/* 初回は即時に周期処理を実行 */
	nextProcessingTick = 0;
	while (1)
	{
		uint32_t currentTick;

		/* 処理周期待ち */
		do {
			currentTick = HAL_GetTick();
		} while (currentTick < nextProcessingTick);

		/* 次回処理時間計算 (簡易版なので桁溢れは考慮しない) */
		nextProcessingTick = currentTick + MAIN_PERIODIC;

		/* SW読み出し1回目 */
		DrvSwReadFirst();
		/* 表示更新(DMA転送開始) */
		DrvOledDisplay();
		/* SW読み出し2回目 */
		DrvSwReadMid();
		/* Joystick読み出し */
		DrvJoystickRead();
		/* SW読み出し3回目(確定) */
		DrvSwReadEnd();

		/* アプリケーション周期処理 */
		AplPuzzleMain();
	}
}
