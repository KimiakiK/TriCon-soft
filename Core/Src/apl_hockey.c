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
#include "math.h"

#include "apl_hockey.h"

/********** Define **********/

#define FLOAT_EPSILON		0.00001f

#define STAGE_TOP			7.0f
#define STAGE_BOTTOM		120.0f
#define STAGE_LEFT			1.0f
#define STAGE_RIGHT			62.0f
#define GOAL_TOP			0.0f
#define GOAL_BOTTOM			128.0f
#define GOAL_LEFT			15.0f
#define GOAL_RIGHT			48.0f
#define STRIKER_AREA_HEIGHT	45.0f

#define PACK_MAX_SPEED_NORMAL		1.6f
#define PACK_MAX_SPEED_SKILL		2.5f
#define STRIKER_SPEED_NORMAL		1.0f
#define STRIKER_SPEED_SKILL			1.3f
#define STRIKER_COEFFICIENT_NORMA	1.8f
#define STRIKER_COEFFICIENT_SKILL	2.1f

#define PACK_NUM_MAX		2

#define GOALPOST_NUM		4

enum stateType {
	STATE_WAITING = 0,
	STATE_PLAYING,
	STATE_NUM
};

enum objectTypeType {
	TYPE_NORMAL = 0,
	TYPE_POWER,
	TYPE_GOAL_1P,
	TYPE_GOAL_2P
};

enum buttonType {
	BUTTON_A = 0,
	BUTTON_B,
	BUTTON_NUM
};

enum buttonStateType {
	BUTTON_OFF = 0,
	BUTTON_ON,
	BUTTON_INIT
};

enum playerNumType {
	PLAYER_1P = 0,
	PLAYER_2P,
	PLAYER_NUM
};

enum rotateType {
	ROTATE_NONE = 0,
	ROTATE_CW,
	ROTATE_CCW,
	ROTATE_INV
};

/********** Type **********/

typedef struct objectType {
	vector_t pos;
	vector_t vec;
	float r;
	float speed;
	uint8_t type;
} object_t;

typedef struct areaType {
	float top;
	float bottom;
	float left;
	float right;
} area_t;

/********** Constant **********/

static const area_t pack_area = {
		STAGE_TOP, STAGE_BOTTOM, STAGE_LEFT, STAGE_RIGHT
};

static const area_t goal_area = {
		GOAL_TOP, GOAL_BOTTOM, GOAL_LEFT, GOAL_RIGHT
};

static const area_t striker_area[PLAYER_NUM] = {
		/* 1P */
		{STAGE_BOTTOM - STRIKER_AREA_HEIGHT, STAGE_BOTTOM, STAGE_LEFT, STAGE_RIGHT},
		/* 2P */
		{STAGE_TOP, STAGE_TOP + STRIKER_AREA_HEIGHT, STAGE_LEFT, STAGE_RIGHT}
};

static const object_t goalpost[GOALPOST_NUM] = {
		/* 左上 */
		{{GOAL_LEFT, STAGE_TOP}, {0.0f, 0.0f}, 0.0f, TYPE_NORMAL},
		/* 右上 */
		{{STAGE_RIGHT, STAGE_TOP}, {0.0f, 0.0f}, 0.0f, TYPE_NORMAL},
		/* 左下 */
		{{GOAL_LEFT, STAGE_BOTTOM}, {0.0f, 0.0f}, 0.0f, TYPE_NORMAL},
		/* 右下 */
		{{STAGE_RIGHT, STAGE_BOTTOM}, {0.0f, 0.0f}, 0.0f, TYPE_NORMAL}
};

static const uint8_t img_number [] = {
 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1,
 1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1,
 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 1,
};
static const sprite_t sprite_number = {img_number, 10, 94, 30, 5};
#define NUMBER_WIDTH	3
#define NUMBER_HEIGHT	5

static const uint8_t img_pack_normal [] = {
 0, 1, 1, 0,
 1, 1, 1, 1,
 1, 1, 1, 1,
 0, 1, 1, 0,
};
static const sprite_t sprite_pack_normal = {img_pack_normal, 30, 68, 4, 4};

static const uint8_t img_striker_normal [] = {
 0, 0, 1, 1, 0, 0,
 0, 1, 0, 0, 1, 0,
 1, 0, 1, 1, 0, 1,
 1, 0, 1, 1, 0, 1,
 0, 1, 0, 0, 1, 0,
 0, 0, 1, 1, 0, 0,
};
static const sprite_t sprite_striker_normal = {img_striker_normal, 29, 109, 6, 6};

/********** Variable **********/

static uint8_t state;
static uint8_t controller_id[PLAYER_NUM];
static uint8_t button[PLAYER_NUM][BUTTON_NUM];
static uint16_t score[PLAYER_NUM];
static uint8_t packNum;
static object_t pack[PACK_NUM_MAX];
static object_t striker[PLAYER_NUM];


/********** Function Prototype **********/

static void initButton(void);
static void initScore(void);
static void initPack(uint8_t pack_id);
static void initStriker(void);

static void input(void);

static void addPack(void);
static void movePack(void);
static void moveStriker(void);

static void checkGoalpost(object_t* pack);
static vector_t calcCollision(const object_t* o1, const object_t* o2, float k);

static void draw(void);
static void drawStage(void);
static void drawPack(void);
static void drawStriker(void);
static void drawScoreNumber(void);

static void drawRect(uint8_t pos_x, uint8_t pos_y, uint8_t w, uint8_t h);
static void drawSpritePosition(sprite_t sprite, int16_t pos_x, int16_t pos_y);
static void drawNumber(uint8_t number, uint8_t pos_x, uint8_t pos_y, uint8_t rotate);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplHockeyInit(void)
{
	DrvOledSetDisplayAngle(OLED_ANGLE_LEFT);

	controller_id[PLAYER_1P] = CONTROLLER_L;
	controller_id[PLAYER_2P] = CONTROLLER_R;

	packNum = 2;

	initButton();
	initPack(PACK_NUM_MAX);
	initStriker();
	initScore();
}

/*=== 周期関数 ===*/
void AplHockeyMain(void)
{
	/* 入力処理 */
	input();

	/* パック追加 */
	addPack();

	/* 移動処理 */
	moveStriker();
	movePack();

	/* 描画処理 */
	draw();
}

/*=== ボタン入力情報初期化関数 ===*/
static void initButton(void)
{
	uint8_t i, j;

	for (i=0; i<PLAYER_NUM; i++) {
		for (j=0; j<BUTTON_NUM; j++) {
			button[i][j] = BUTTON_INIT;
		}
	}
}

static void initScore(void)
{
	uint8_t i;

	for (i=0; i<PLAYER_NUM; i++) {
		score[i] = 0;
	}
}

/*=== ボール情報初期化関数 ===*/
static void initPack(uint8_t pack_id)
{
	uint8_t i, start, end;

	if (pack_id >= PACK_NUM_MAX) {
		/* 全初期化 */
		start = 0;
		end = PACK_NUM_MAX;
	} else {
		/* 指定初期化 */
		start = pack_id;
		end = pack_id + 1;
	}

	for (i=start; i<end; i++) {
		/* 画面外(ゴールラインを超えた画面外)で静止 */
		pack[i].pos.x = 31.0f;
		pack[i].pos.y = -100.0f - 10.0f * i;
		pack[i].vec.x = 0.0f;
		pack[i].vec.y = 0.0f;
		pack[i].r = 2.0;
		pack[i].speed = PACK_MAX_SPEED_NORMAL;
		pack[i].type = TYPE_GOAL_1P + (i % 2);
	}
}

/*=== ストライカー情報初期化関数 ===*/
static void initStriker(void)
{
	striker[PLAYER_1P].pos.x = 31.0f;
	striker[PLAYER_1P].pos.y = 109.0f;
	striker[PLAYER_1P].vec.x = 0.0f;
	striker[PLAYER_1P].vec.y = 0.0f;
	striker[PLAYER_1P].r = 3.0f;
	striker[PLAYER_1P].speed = STRIKER_SPEED_NORMAL;
	striker[PLAYER_1P].type = TYPE_NORMAL;

	striker[PLAYER_2P].pos.x = 31.0f;
	striker[PLAYER_2P].pos.y = 16.0f;
	striker[PLAYER_2P].vec.x = 0.0f;
	striker[PLAYER_2P].vec.y = 0.0f;
	striker[PLAYER_2P].r = 3.0f;
	striker[PLAYER_2P].speed = STRIKER_SPEED_NORMAL;
	striker[PLAYER_2P].type = TYPE_NORMAL;
}

/*=== 入力処理関数 ===*/
static void input(void)
{
	uint8_t sw;
	vector_t pos;
	uint8_t i;

	for (i=0; i<PLAYER_NUM; i++) {
		sw = DrvSwGetState(controller_id[i]);
		pos = DrvJoystickGetAnalogPos(controller_id[i]);

		/* Aボタン：ストライカーパワー上昇 */
		if ((sw & PUSH_A)==PUSH_A) {
			if (button[i][BUTTON_A] == BUTTON_OFF) {
				button[i][BUTTON_A] = BUTTON_ON;
				striker[i].type = TYPE_POWER;
			}
		} else {
			button[i][BUTTON_A] = BUTTON_OFF;
			striker[i].type = TYPE_NORMAL;
		}

		/* Bボタン：ストライカー移動速度上昇 */
		if ((sw & PUSH_B)==PUSH_B) {
			if (button[i][BUTTON_B] == BUTTON_OFF) {
				button[i][BUTTON_B] = BUTTON_ON;
				striker[i].speed = STRIKER_SPEED_SKILL;
			}
		} else {
			button[i][BUTTON_B] = BUTTON_OFF;
			striker[i].speed = STRIKER_SPEED_NORMAL;
		}

		/* ストライカー移動量設定 (2Pは符号反転) */
		if (i == PLAYER_2P) {
			pos.x = - pos.x;
			pos.y = - pos.y;
		}
		striker[i].vec.x = pos.x * striker[i].speed;
		striker[i].vec.y = pos.y * striker[i].speed;
	}
}

/* パック追加関数 */
static void addPack(void)
{
	uint8_t i;
	for (i=0; i<packNum; i++) {
		if (pack[i].type == TYPE_GOAL_1P){
			/* 2P側に追加 */
			pack[i].pos.x = 51.0f;
			pack[i].pos.y = 57.0f;
			pack[i].vec.x = -0.3f;
			pack[i].vec.y = -0.6f;
		} else if (pack[i].type == TYPE_GOAL_2P) {
			/* 1P側に追加 */
			pack[i].pos.x = 11.0f;
			pack[i].pos.y = 68.0f;
			pack[i].vec.x = 0.3f;
			pack[i].vec.y = 0.6f;
		}
		pack[i].type = TYPE_NORMAL;
	}
}

/*=== パック移動関数 ===*/
static void movePack(void)
{
	uint8_t i, j;
	vector_t diff;
	float distance, ratio;
	vector_t temp;

	/* 移動量加算 */
	for (i=0; i<packNum; i++) {
		pack[i].pos.x += pack[i].vec.x;
		pack[i].pos.y += pack[i].vec.y;
	}

	/* パック同士の衝突 */
	for (i=0; i<packNum; i++) {
		for (j=i+1; j<packNum; j++) {
			diff.x = pack[i].pos.x - pack[j].pos.x;
			diff.y = pack[i].pos.y - pack[j].pos.y;
			distance = sqrtf(diff.x * diff.x + diff.y * diff.y);
			if (distance < (pack[i].r + pack[j].r)) {
				/* パック衝突 */
				ratio = (pack[i].r + pack[j].r - distance) / distance / 2.0f;
				pack[i].pos.x += diff.x * ratio;
				pack[i].pos.y += diff.y * ratio;
				pack[j].pos.x -= diff.x * ratio;
				pack[j].pos.y -= diff.y * ratio;

				/* 衝突後ベクトル計算 */
				temp = calcCollision(&(pack[i]), &(pack[j]), 1.0f);
				pack[j].vec = calcCollision(&(pack[j]), &(pack[i]), 1.0f);
				pack[i].vec = temp;
			}
		}
	}

	/* ステージ判定 */
	for (i=0; i<packNum; i++) {
		/* 上 */
		if (pack[i].pos.y < pack_area.top + pack[i].r) {
			if (pack[i].pos.x < goal_area.left || pack[i].pos.x >= goal_area.right) {
				/* ステージに衝突 */
				pack[i].vec.y = -pack[i].vec.y;
				pack[i].pos.y += (pack_area.top + pack[i].r) - pack[i].pos.y;
			} else if (pack[i].pos.y < pack_area.top) {
				/* ゴールラインを通過 */
				/* 左 */
				if (pack[i].pos.x < goal_area.left + pack[i].r) {
					pack[i].vec.x = -pack[i].vec.x;
					pack[i].pos.x += (goal_area.left + pack[i].r) - pack[i].pos.x;
				}
				/* 右 */
				if (pack[i].pos.x >= goal_area.right - pack[i].r) {
					pack[i].vec.x = -pack[i].vec.x;
					pack[i].pos.x += (goal_area.right - pack[i].r) - pack[i].pos.x - FLOAT_EPSILON;
				}
				/* 画面外 */
				if (pack[i].pos.y < goal_area.top - pack[i].r) {
					if (pack[i].type != TYPE_GOAL_1P && pack[i].type != TYPE_GOAL_2P){
						score[PLAYER_1P]++;
						initPack(i);
						pack[i].type = TYPE_GOAL_1P;
					}
				}
			} else {
				/* ゴールポスト判定 */
				checkGoalpost(&(pack[i]));
			}
		}
		/* 下 */
		if (pack[i].pos.y >= pack_area.bottom - pack[i].r) {
			if (pack[i].pos.x < goal_area.left || pack[i].pos.x >= goal_area.right) {
				/* ステージに衝突 */
				pack[i].vec.y = -pack[i].vec.y;
				pack[i].pos.y += (pack_area.bottom - pack[i].r) - pack[i].pos.y - FLOAT_EPSILON;
			} else if (pack[i].pos.y >= pack_area.bottom) {
				/* ゴールラインを通過 */
				/* 左 */
				if (pack[i].pos.x < goal_area.left + pack[i].r) {
					pack[i].vec.x = -pack[i].vec.x;
					pack[i].pos.x += (goal_area.left + pack[i].r) - pack[i].pos.x;
				}
				/* 右 */
				if (pack[i].pos.x >= goal_area.right - pack[i].r) {
					pack[i].vec.x = -pack[i].vec.x;
					pack[i].pos.x += (goal_area.right - pack[i].r) - pack[i].pos.x - FLOAT_EPSILON;
				}
				/* 画面外 */
				if (pack[i].pos.y >= goal_area.bottom + pack[i].r) {
					if (pack[i].type != TYPE_GOAL_1P && pack[i].type != TYPE_GOAL_2P){
						score[PLAYER_2P]++;
						initPack(i);
						pack[i].type = TYPE_GOAL_2P;
					}
				}
			} else {
				/* ゴールポスト判定 */
				checkGoalpost(&(pack[i]));
			}
		}
		/* 左 */
		if (pack[i].pos.x < pack_area.left + pack[i].r) {
			pack[i].vec.x = -pack[i].vec.x;
			pack[i].pos.x += (pack_area.left + pack[i].r) - pack[i].pos.x;
		}
		/* 右 */
		if (pack[i].pos.x >= pack_area.right - pack[i].r) {
			pack[i].vec.x = -pack[i].vec.x;
			pack[i].pos.x += (pack_area.right - pack[i].r) - pack[i].pos.x - FLOAT_EPSILON;
		}
	}
}

/*=== ストライカー移動関数 ===*/
static void moveStriker(void)
{
	uint8_t i_s, i_p;
	vector_t diff;
	float distance, ratio;
	float coefficient;

	/* 移動量加算 */
	for (i_s=0; i_s<PLAYER_NUM; i_s++) {
		striker[i_s].pos.x += striker[i_s].vec.x * striker[i_s].speed;
		striker[i_s].pos.y += striker[i_s].vec.y * striker[i_s].speed;
	}

	/* 壁判定 */
	for (i_s=0; i_s<PLAYER_NUM; i_s++) {
		/* 上 */
		if (striker[i_s].pos.y < striker_area[i_s].top + striker[i_s].r) {
			striker[i_s].pos.y = striker_area[i_s].top + striker[i_s].r;
		}
		/* 下 */
		if (striker[i_s].pos.y >= (striker_area[i_s].bottom - striker[i_s].r)) {
			striker[i_s].pos.y = striker_area[i_s].bottom - striker[i_s].r - FLOAT_EPSILON;
		}
		/* 左 */
		if (striker[i_s].pos.x < striker_area[i_s].left + striker[i_s].r) {
			striker[i_s].pos.x = striker_area[i_s].left + striker[i_s].r;
		}
		/* 右 */
		if (striker[i_s].pos.x >= striker_area[i_s].right - striker[i_s].r) {
			striker[i_s].pos.x = striker_area[i_s].right - striker[i_s].r - FLOAT_EPSILON;
		}
	}

	/* パック当たり判定 */
	for (i_s=0; i_s<PLAYER_NUM; i_s++) {
		for (i_p=0; i_p<packNum; i_p++) {
			diff.x = pack[i_p].pos.x - striker[i_s].pos.x;
			diff.y = pack[i_p].pos.y - striker[i_s].pos.y;
			distance = sqrtf(diff.x * diff.x + diff.y * diff.y);
			if (distance < (pack[i_p].r + striker[i_s].r)) {
				/* ストライカーとパックが当たっている場合はパックを移動 */
				ratio = ((pack[i_p].r + striker[i_s].r) - distance) / distance;
				pack[i_p].pos.x += diff.x * ratio;
				pack[i_p].pos.y += diff.y * ratio;

				/* パワースキル処理 */
				if (striker[i_s].type == TYPE_POWER) {
					pack[i_p].speed = PACK_MAX_SPEED_SKILL;
					coefficient = STRIKER_COEFFICIENT_SKILL;
				} else {
					pack[i_p].speed = PACK_MAX_SPEED_NORMAL;
					coefficient = STRIKER_COEFFICIENT_NORMA;
				}

				/* 衝突後ベクトル計算 */
				pack[i_p].vec = calcCollision(&(pack[i_p]), &(striker[i_s]), coefficient);
			}
		}
	}
}

/*=== ゴールポスト衝突判定・処理関数 ===*/
static void checkGoalpost(object_t* pack)
{
	uint8_t i;
	vector_t diff;
	float distance, ratio;

	for (i=0; i<GOALPOST_NUM; i++) {
		diff.x = pack->pos.x - goalpost[i].pos.x;
		diff.y = pack->pos.y - goalpost[i].pos.y;
		distance = sqrtf(diff.x * diff.x + diff.y * diff.y);
		if (distance < pack->r) {
			/* パックとゴールポストが当たっている場合はパックを移動 */
			ratio = (pack->r - distance) / distance;
			pack->pos.x += diff.x * ratio;
			pack->pos.y += diff.y * ratio;

			/* 衝突後ベクトル計算 */
			pack->vec = calcCollision(pack, &(goalpost[i]), 2.0f);
		}
	}
}

/*=== 衝突後ベクトル計算関数 ===*/
static vector_t calcCollision(const object_t* o1, const object_t* o2, float k)
{
	vector_t diff, unit;
	vector_t ret;
	float norm, dot;

	diff.x = o2->pos.x - o1->pos.x;
	diff.y = o2->pos.y - o1->pos.y;
	norm = sqrtf(diff.x * diff.x + diff.y * diff.y);
	unit.x = diff.x / norm;
	unit.y = diff.y / norm;

	dot = (o2->vec.x - o1->vec.x) * unit.x + (o2->vec.y - o1->vec.y) * unit.y;

	ret.x = k * dot * unit.x + o1->vec.x;
	ret.y = k * dot * unit.y + o1->vec.y;

	/* 速度制限 */
	norm = sqrtf(ret.x * ret.x + ret.y * ret.y);
	if (norm > o1->speed) {
		ret.x = ret.x * o1->speed / norm;
		ret.y = ret.y * o1->speed / norm;
	}

	return ret;
}

/*=== 描画関数 ===*/
static void draw(void)
{
	DrvOledClearMemory();

	drawStage();
	drawPack();
	drawStriker();
	drawScoreNumber();
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

	/* アクティブライン */
	for (x=0; x<20; x++) {
		DisplayMemory[MAP_VERTICAL((x * 3) + 3, 52)] = DOT_WHITE;
		DisplayMemory[MAP_VERTICAL((x * 3) + 3, 75)] = DOT_WHITE;
	}

	/* ゴールライン */
	for (x=0; x<10; x++) {
		DisplayMemory[MAP_VERTICAL((x * 3) + 18, 7)] = DOT_WHITE;
		DisplayMemory[MAP_VERTICAL((x * 3) + 18, 120)] = DOT_WHITE;
	}
}

/*=== パック描画関数 ===*/
static void drawPack(void)
{
	uint8_t i;

	for (i=0; i<packNum; i++) {
		drawSpritePosition(sprite_pack_normal, ((int16_t)pack[i].pos.x - (int16_t)pack[i].r) + 1, ((int16_t)pack[i].pos.y - (int16_t)pack[i].r) + 1);
	}
}

/*=== ストライカー描画関数 ===*/
static void drawStriker(void)
{
	uint8_t i;

	for (i=0; i<PLAYER_NUM; i++) {
		drawSpritePosition(sprite_striker_normal, ((int16_t)striker[i].pos.x - (int16_t)striker[i].r) + 1, ((int16_t)striker[i].pos.y - (int16_t)striker[i].r) + 1);
	}
}

/*=== スコア描画関数 ===*/
static void drawScoreNumber(void)
{
	uint8_t number;

	/* 1P */
	number = (score[PLAYER_1P] % 1000) / 100;
	drawNumber(number, 1, 123, ROTATE_NONE);
	number = (score[PLAYER_1P] % 100) / 10;
	drawNumber(number, 5, 123, ROTATE_NONE);
	number = score[PLAYER_1P] % 10;
	drawNumber(number, 9, 123, ROTATE_NONE);
	/* 2P */
	number = (score[PLAYER_2P] % 1000) / 100;
	drawNumber(number, 60, 0, ROTATE_INV);
	number = (score[PLAYER_2P] % 100) / 10;
	drawNumber(number, 56, 0, ROTATE_INV);
	number = score[PLAYER_2P] % 10;
	drawNumber(number, 52, 0, ROTATE_INV);
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

/*=== スプライト描画(位置指定)関数 ===*/
static void drawSpritePosition(sprite_t sprite, int16_t pos_x, int16_t pos_y)
{
	uint8_t x, y;
	int16_t target_x, target_y;

	for (x=0; x<sprite.w; x++) {
		for (y=0; y<sprite.h; y++) {
			if (sprite.img[y * sprite.w + x] == DOT_WHITE) {
				target_x = x + pos_x;
				target_y = y + pos_y;
				if ((target_x >= 0) && (target_x < OLED_HEIGHT) && (target_y >= 0) && (target_y < OLED_WIDTH)) {
					DisplayMemory[MAP_VERTICAL(target_x, target_y)] = sprite.img[y * sprite.w + x];
				}
			}
		}
	}
}

/*=== 数字描画関数 ===*/
static void drawNumber(uint8_t number, uint8_t pos_x, uint8_t pos_y, uint8_t rotate)
{
	uint8_t x, y;
	uint8_t target_x, target_y;

	for (x=0; x<NUMBER_WIDTH; x++) {
		for (y=0; y<NUMBER_HEIGHT; y++) {
			target_x = x + pos_x;
			target_y = y + pos_y;
			if (rotate == ROTATE_INV) {
				DisplayMemory[MAP_VERTICAL(target_x, target_y)] = sprite_number.img[(NUMBER_HEIGHT - 1 - y) * sprite_number.w + (NUMBER_WIDTH - 1 - x) + number * NUMBER_WIDTH];
			} else {
				DisplayMemory[MAP_VERTICAL(target_x, target_y)] = sprite_number.img[y * sprite_number.w + x + number * NUMBER_WIDTH];
			}
		}
	}
}
