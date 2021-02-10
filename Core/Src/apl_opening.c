/*
 * sys_opening.c
 *
 *  Created on: Aug 12, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "common_drv.h"
#include "common_sprite.h"

#include "apl_opening.h"

/********** Define **********/

#define TIMER_INCREMENT		(MAIN_PERIODIC)	/* 1周期の時間[ms] */

#define LOGO_DISPLAY_TIME	500				/* ロゴ表示時間[ms] */
#define BLINK_ON_TIME		600
#define BLINK_OFF_TIME		400

enum stateType {
	STATE_LOGO_ANIME = 0,
	STATE_LOGO,
	STATE_CONTROLLER
};

enum timerIdType {
	TIMER_LOGO = 0,
	TIMER_BLINK,
	TIMER_NUM
};

enum animeStateType {
	ANIME_START = 0,
	ANIME_R,
	ANIME_I,
	ANIME_DONE
};

enum rotateType {
	ROTATE_NONE = 0,
	ROTATE_CW,
	ROTATE_CCW,
	ROTATE_INV
};

/********** Type **********/

/********** Constant **********/

static const uint8_t img_logo_t [] = {
 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
};
static const sprite_t sprite_logo_t = {img_logo_t, 11, 22, 17, 20};

static const uint8_t img_logo_r [] = {
 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static const sprite_t sprite_logo_r = {img_logo_r, 33, 25, 15, 17};

static const uint8_t img_logo_i [] = {
 0, 1, 1, 1, 0,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 0, 1, 1, 1, 0,
 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0,
 0, 1, 1, 1, 0,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1,
 0, 1, 1, 1, 0,
};
static const sprite_t sprite_logo_i = {img_logo_i, 53, 22, 5, 20};

static const uint8_t img_logo_con [] = {
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0,
};
static const sprite_t sprite_logo_con = {img_logo_con, 63, 22, 55, 20};

static const uint8_t img_press_the_button [] = {
 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 0,
 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1,
 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1,
 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1,
};
static const sprite_t sprite_press_the_button = {img_press_the_button, 35, 58, 61, 6};

/********** Variable **********/

static uint8_t state;
static uint16_t timer[TIMER_NUM];
static uint8_t animeState;
static uint8_t animeFrame;

/********** Function Prototype **********/

static void transitionLauncher(void);

static void displayLogoAnime(void);
static void displayLogo(void);
static void displayController(void);

static void drawSprite(sprite_t sprite);
static void drawSpriteOffset(sprite_t sprite, int16_t offset_x, int16_t offset_y);
static void drawSpriteRotate(sprite_t sprite, uint8_t rotate, int16_t pos_x, int16_t pos_y);

static void initTimer(void);
static void updateTimer(void);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplOpeningInit(void)
{
	state = STATE_LOGO_ANIME;
	initTimer();
	animeState = ANIME_START;
	animeFrame = 0;
}

/*=== 周期関数 ===*/
void AplOpeningMain(void)
{
	uint8_t connectedController;

	connectedController = SysGetConnectedController();

	updateTimer();

	/* 状態遷移 */
	switch (state) {
	case STATE_LOGO_ANIME:
		if ((connectedController != CONNECTED_NONE) || (animeState == ANIME_DONE)) {
			state = STATE_LOGO;
			initTimer();
		}
		break;
	case STATE_LOGO:
		if (timer[TIMER_LOGO] > LOGO_DISPLAY_TIME) {
			if (connectedController == CONNECTED_NONE) {
				state = STATE_CONTROLLER;
				initTimer();
			} else {
				/* コントローラー接続済はそのままランチャーへ */
				transitionLauncher();
			}
		}
		break;
	case STATE_CONTROLLER:
		if (connectedController != CONNECTED_NONE) {
			transitionLauncher();
		}
		break;
	default:
		AplOpeningInit();
		break;
	}

	/* 状態処理 */
	switch (state) {
	case STATE_LOGO_ANIME:
		displayLogoAnime();
		break;
	case STATE_LOGO:
		displayLogo();
		break;
	case STATE_CONTROLLER:
		displayController();
		break;
	default:
		/* Do nothing */
		break;
	}
}

static void transitionLauncher(void)
{
	SysSetNextApplication(APL_LAUNCHER);
}

static void displayLogoAnime(void)
{
	int16_t offset;

	DrvOledClearMemory();

	animeFrame++;

	switch (animeState) {
	case ANIME_START:
		/* "T"を左からスライドイン */
		offset = animeFrame - (sprite_logo_t.x + sprite_logo_t.w);
		drawSpriteOffset(sprite_logo_t, offset, 0);
		if (offset >= -1) {
			animeState = ANIME_R;
			animeFrame = 0;
		}
		break;
	case ANIME_R:
		drawSprite(sprite_logo_t);
		/* "r"を下からスライドイン */
		offset = OLED_HEIGHT - (animeFrame * 2);
		drawSpriteOffset(sprite_logo_r, 0, offset);
		if (offset <= 2) {
			animeState = ANIME_I;
			animeFrame = 0;
		}
		break;
	case ANIME_I:
		drawSprite(sprite_logo_t);
		drawSprite(sprite_logo_r);
		/* "i"を右からスライドイン */
		offset = OLED_WIDTH - (animeFrame * 4);
		drawSpriteOffset(sprite_logo_i, offset, 0);
		if (offset <= 4) {
			animeState = ANIME_DONE;
			animeFrame = 0;
		}
		break;
	default:
		animeState = ANIME_DONE;
		break;
	}

}

static void displayLogo(void)
{
	DrvOledClearMemory();

	drawSprite(sprite_logo_t);
	drawSprite(sprite_logo_r);
	drawSprite(sprite_logo_i);
	drawSprite(sprite_logo_con);
}

static void displayController(void)
{
	displayLogo();

	if (timer[TIMER_BLINK] >= (BLINK_ON_TIME + BLINK_OFF_TIME)) {
		timer[TIMER_BLINK] = 0;
	}

	if (timer[TIMER_BLINK] < BLINK_ON_TIME) {
		drawSpriteRotate(sprite_press_the_button, ROTATE_NONE, 35, 58);
		drawSpriteRotate(sprite_press_the_button, ROTATE_CW, 0, 2);
		drawSpriteRotate(sprite_press_the_button, ROTATE_CCW, 122, 1);
	}
}

static void drawSprite(sprite_t sprite)
{
	uint8_t x, y;

	for (x=0; x<sprite.w; x++) {
		for (y=0; y<sprite.h; y++) {
			DisplayMemory[MAP_HORIZONTAL(x + sprite.x, y + sprite.y)] = sprite.img[y * sprite.w + x];
		}
	}
}

static void drawSpriteOffset(sprite_t sprite, int16_t offset_x, int16_t offset_y)
{
	uint8_t x, y;
	int16_t target_x, target_y;

	for (x=0; x<sprite.w; x++) {
		for (y=0; y<sprite.h; y++) {
			target_x = x + sprite.x + offset_x;
			target_y = y + sprite.y + offset_y;
			if ((target_x >= 0) && (target_x < OLED_WIDTH) && (target_y >= 0) && (target_y < OLED_HEIGHT)) {
				DisplayMemory[MAP_HORIZONTAL(target_x, target_y)] = sprite.img[y * sprite.w + x];
			}
		}
	}
}

static void drawSpriteRotate(sprite_t sprite, uint8_t rotate, int16_t pos_x, int16_t pos_y)
{
	uint8_t x, y;
	int16_t target_x, target_y;
	uint8_t color;

	for (x=0; x<sprite.w; x++) {
			for (y=0; y<sprite.h; y++) {
				if ((rotate == ROTATE_CW) || (rotate == ROTATE_CCW)) {
					target_x = y + pos_x;
					target_y = x + pos_y;
				} else {
					target_x = x + pos_x;
					target_y = y + pos_y;
				}
				if ((target_x >= 0) && (target_x < OLED_WIDTH) && (target_y >= 0) && (target_y < OLED_HEIGHT)) {
					switch (rotate) {
					case ROTATE_NONE:
						color = sprite.img[y * sprite.w + x];
						break;
					case ROTATE_CW:
						color = sprite.img[(sprite.h - 1 - y) * sprite.w + x];
						break;
					case ROTATE_CCW:
						color = sprite.img[y * sprite.w + (sprite.w - 1 - x)];
						break;
					case ROTATE_INV:
						color = sprite.img[(sprite.h - 1 - y) * sprite.w + (sprite.w - 1 - x)];
						break;
					default:
						color = DOT_BLACK;
						break;
					}
					DisplayMemory[MAP_HORIZONTAL(target_x, target_y)] = color;
				}
			}
		}
}

static void initTimer(void)
{
	uint8_t timer_id;

	for (timer_id=0; timer_id<TIMER_NUM; timer_id++) {
		timer[timer_id] = 0;
	}
}

static void updateTimer(void)
{
	uint8_t timer_id;

	for (timer_id=0; timer_id<TIMER_NUM; timer_id++) {
		timer[timer_id] += TIMER_INCREMENT;
	}
}

