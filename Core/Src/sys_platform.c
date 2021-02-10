/*
 * sys_platform.c
 *
 *  Created on: 2020/07/28
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "common_drv.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

/********** Variable **********/

static uint32_t nextProcessingTick;
static uint8_t currentApplicationId;
static uint8_t nextApplicationId;
static uint8_t connectedController;
static uint8_t mainController;

/********** Function Prototype **********/

static void initController(void);
static void checkController(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void SysPlatformInit(void)
{
#if 0
	/* デバッグ用テストピン */
	GPIOA->BSRR = 0x00000100; /* Center SW A (PA8) Output High */
	GPIOA->BSRR = 0x01000000; /* Center SW A (PA8) Output Low */
	GPIOA->MODER = (GPIOA->MODER & 0xFFFCFFFF) | 0x00010000; /* Center SW A (PA8)を汎用出力に変更 */
#endif

	/* Waiting for stability (OLED) */
	HAL_Delay(2000);

	/* Initialize drivers */
	DrvSwInit();
	DrvJoystickInit();
	DrvOledInit();

	initController();

	/* 起動時はオープニングを実行 */
	currentApplicationId = APL_OPENING;
	//currentApplicationId = APL_PUZZLE;
	//currentApplicationId = APL_HOCKEY;
	nextApplicationId = currentApplicationId;
	ApplicationTable[currentApplicationId].init_func();
}

/*=== メインループ関数 ===*/
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

		nextApplicationId = currentApplicationId;

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
		/* コントローラー接続確認 */
		checkController();

		/* アプリケーション周期処理 */
		ApplicationTable[currentApplicationId].main_func();

		/* アプリケーション切替判定・処理 */
		if (nextApplicationId != currentApplicationId) {
			ApplicationTable[nextApplicationId].init_func();
			currentApplicationId = nextApplicationId;

		}
	}
}

/*=== 実行アプリケーション設定関数 ===*/
void SysSetNextApplication(uint8_t application_id)
{
	if (application_id < APL_NUM) {
		nextApplicationId = application_id;
	}
}

/*=== 接続コントローラー取得関数 ===*/
uint8_t SysGetConnectedController(void)
{
	return connectedController;
}

/*=== メインコントローラー取得関数 ===*/
uint8_t SysGetMainController(void)
{
	return mainController;
}

/*=== コントローラー接続状態初期化関数 ===*/
static void initController(void)
{
	connectedController = CONNECTED_NONE;
	mainController = CONTROLLER_NUM;
}

/*=== コントローラー接続確認関数 ===*/
static void checkController(void)
{
	uint8_t controller_id;
	uint8_t sw;

	for (controller_id=0; controller_id<CONTROLLER_NUM; controller_id++) {
		sw = DrvSwGetState(controller_id);
		if (sw != PUSH_NONE) {
			connectedController |= (0x01 << controller_id);
			if (mainController == CONTROLLER_NUM) {
				mainController = controller_id;
			}
		}
	}
}
