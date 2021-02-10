/*
 * apl_hockey.c
 *
 *  Created on: Aug 14, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"
#include "common_drv.h"
#include "common_sprite.h"

#include "apl_hockey.h"

/********** Define **********/

#define BALL_R			2.0f

#define STAGE_TOP		8.0f
#define STAGE_BOTTOM	120.0f
#define STAGE_LEFT		2.0f
#define STAGE_RIGHT		61.0f

enum stateType {
	STATE_WAITING = 0,
	STATE_PLAYING,
	STATE_NUM
};

enum inputStateType {
	INPUT_OFF = 0,
	INPUT_ON,
	INPUT_INIT
};

/********** Type **********/

typedef struct objectType {
	float x;
	float y;
	float vx;
	float vy;
} object_t;

/********** Constant **********/

/********** Variable **********/

static uint8_t state;

static object_t ball;


/********** Function Prototype **********/

static void initBall(void);

static void moveBall(void);

static void draw(void);
static void drawStage(void);
static void drawBall(void);

static void drawRect(uint8_t pos_x, uint8_t pos_y, uint8_t w, uint8_t h);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplHockeyInit(void)
{
	DrvOledSetDisplayAngle(OLED_ANGLE_LEFT);

	initBall();
}

/*=== 周期関数 ===*/
void AplHockeyMain(void)
{
	moveBall();
	draw();
}

/*=== ボール情報初期化関数 ===*/
static void initBall(void)
{
	ball.x = 31.0f;
	ball.y = 63.0f;
	ball.vx = 0.5f;
	ball.vy = 1.2f;
}

/*=== ボール移動関数 ===*/
static void moveBall(void)
{
	ball.x += ball.vx;
	ball.y += ball.vy;

	if (ball.x < STAGE_LEFT + BALL_R) {
		ball.vx = -ball.vx;
		ball.x += (STAGE_LEFT + BALL_R) - ball.x;
	}
	if (ball.x >= STAGE_RIGHT - BALL_R) {
		ball.vx = -ball.vx;
		ball.x += (STAGE_RIGHT - BALL_R) - ball.x;
	}
	if (ball.y < STAGE_TOP + BALL_R) {
		ball.vy = -ball.vy;
		ball.y += (STAGE_TOP + BALL_R) - ball.y;
	}
	if (ball.y >= STAGE_BOTTOM - BALL_R) {
		ball.vy = -ball.vy;
		ball.y += (STAGE_BOTTOM - BALL_R) - ball.y;
	}
}

/*=== 描画関数 ===*/
static void draw(void)
{
	DrvOledClearMemory();

	drawStage();
	drawBall();
}

/*=== ホッケー台描画関数 ===*/
static void drawStage(void)
{
	uint8_t x;

	/* 左枠線 */
	drawRect(14, 0, 2, 8);
	drawRect(2, 6, 12, 2);
	drawRect(0, 6, 2, 116);
	drawRect(2, 120, 12, 2);
	drawRect(14, 120, 2, 8);

	/* 右枠線 */
	drawRect(48, 0, 2, 8);
	drawRect(50, 6, 12, 2);
	drawRect(62, 6, 2, 116);
	drawRect(50, 120, 12, 2);
	drawRect(48, 120, 2, 8);

	/* 中央ライン */
	for (x=0; x<30; x++) {
		DisplayMemory[MAP_VERTICAL((x * 2) + 3, 63)] = DOT_WHITE;
		DisplayMemory[MAP_VERTICAL((x * 2) + 2, 64)] = DOT_WHITE;
	}

	/* ゴールライン */
	for (x=0; x<10; x++) {
		DisplayMemory[MAP_VERTICAL((x * 3) + 18, 7)] = DOT_WHITE;
		DisplayMemory[MAP_VERTICAL((x * 3) + 18, 120)] = DOT_WHITE;
	}
}

/*=== ボール描画関数 ===*/
static void drawBall(void)
{
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x - 1, (uint8_t)ball.y + 0)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x - 1, (uint8_t)ball.y + 1)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 0, (uint8_t)ball.y - 1)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 0, (uint8_t)ball.y + 0)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 0, (uint8_t)ball.y + 1)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 0, (uint8_t)ball.y + 2)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 1, (uint8_t)ball.y - 1)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 1, (uint8_t)ball.y + 0)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 1, (uint8_t)ball.y + 1)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 1, (uint8_t)ball.y + 2)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 2, (uint8_t)ball.y + 0)] = DOT_WHITE;
	DisplayMemory[MAP_VERTICAL((uint8_t)ball.x + 2, (uint8_t)ball.y + 1)] = DOT_WHITE;
}

/*=== 四角形描画関数 ===*/
static void drawRect(uint8_t pos_x, uint8_t pos_y, uint8_t w, uint8_t h)
{
	uint8_t x, y;

	for (x=pos_x; x<(pos_x + w); x++) {
		for (y=pos_y; y<(pos_y + h); y++) {
			DisplayMemory[MAP_VERTICAL(x, y)] = DOT_WHITE;
		}
	}
}
