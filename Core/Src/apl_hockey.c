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
#define GOAL_LEFT			15.0f
#define GOAL_RIGHT			48.0f
#define STRIKER_AREA_HEIGHT	45.0f

#define PACK_NUM_MAX		16

#define GOALPOST_NUM		4

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

enum playerNumType {
	PLAYER_1P = 0,
	PLAYER_2P,
	PLAYER_NUM
};

/********** Type **********/

typedef struct objectType {
	vector_t pos;
	vector_t vec;
	float r;
	float speed;
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
		STAGE_TOP, STAGE_BOTTOM, GOAL_LEFT, GOAL_RIGHT
};

static const area_t striker_area[PLAYER_NUM] = {
		/* 1P */
		{STAGE_BOTTOM - STRIKER_AREA_HEIGHT, STAGE_BOTTOM, STAGE_LEFT, STAGE_RIGHT},
		/* 2P */
		{STAGE_TOP, STAGE_TOP + STRIKER_AREA_HEIGHT, STAGE_LEFT, STAGE_RIGHT}
};

static const object_t goalpost[GOALPOST_NUM] = {
		/* 左上 */
		{{GOAL_LEFT, STAGE_TOP}, {0.0f, 0.0f}, 0.0f},
		/* 右上 */
		{{STAGE_RIGHT, STAGE_TOP}, {0.0f, 0.0f}, 0.0f},
		/* 左下 */
		{{GOAL_LEFT, STAGE_BOTTOM}, {0.0f, 0.0f}, 0.0f},
		/* 右下 */
		{{STAGE_RIGHT, STAGE_BOTTOM}, {0.0f, 0.0f}, 0.0f}
};

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

static uint8_t packNum;
static object_t pack[PACK_NUM_MAX];
static object_t striker[PLAYER_NUM];


/********** Function Prototype **********/

static void initPack(void);
static void initStriker(void);

static void movePack(void);
static void moveStriker(void);

static void checkGoalpost(object_t* pack);
static vector_t calcCollision(const object_t* o1, const object_t* o2, float k);

static void draw(void);
static void drawStage(void);
static void drawPack(void);
static void drawStriker(void);

static void drawRect(uint8_t pos_x, uint8_t pos_y, uint8_t w, uint8_t h);
static void drawSpritePosition(sprite_t sprite, int16_t pos_x, int16_t pos_y);

/********** Function **********/

/*=== 初期化関数 ===*/
void AplHockeyInit(void)
{
	DrvOledSetDisplayAngle(OLED_ANGLE_LEFT);

	initPack();
	initStriker();
}

/*=== 周期関数 ===*/
void AplHockeyMain(void)
{
	moveStriker();
	movePack();
	draw();
}

/*=== ボール情報初期化関数 ===*/
static void initPack(void)
{
#if 1
	packNum = 1;
	pack[0].pos.x = 31.0f;
	pack[0].pos.y = 100.0f;
	pack[0].vec.x = 0.0f;
	pack[0].vec.y = 0.0f;
	pack[0].r = 2.0;
	pack[0].speed = 2.5f;
#endif
#if 0
	uint8_t i, x, y;
	packNum = 10;

	pack[0].pos.x = 31.0f;
	pack[0].pos.y = 100.0f;
	pack[0].vec.x = 0.0f;
	pack[0].vec.y = 0.0f;
	pack[0].r = 2.0;
	pack[0].speed = 2.5f;

	for (i=1; i<packNum; i++) {
		if (i==1){
			x = 31; y = 48;
		} else if (i==2) {
			x = 29; y = 44;
		} else if (i==4) {
			x = 27; y = 40;
		} else if (i==7) {
			x = 29; y = 36;
		} else if (i==9) {
			x = 31; y = 32;
		}
		pack[i].pos.x = x;
		pack[i].pos.y = y;
		pack[i].vec.x = 0.0f;
		pack[i].vec.y = 0.0f;
		pack[i].r = 2.0;
		pack[i].speed = 2.5f;

		x += 4;
	}
#endif
}

/*=== ストライカー情報初期化関数 ===*/
static void initStriker(void)
{
	striker[PLAYER_1P].pos.x = 31.0f;
	striker[PLAYER_1P].pos.y = 112.0f;
	striker[PLAYER_1P].vec.x = 0.0f;
	striker[PLAYER_1P].vec.y = 0.0f;
	striker[PLAYER_1P].r = 3.0f;
	striker[PLAYER_2P].pos.x = 31.0f;
	striker[PLAYER_2P].pos.y = 16.0f;
	striker[PLAYER_2P].vec.x = 0.0f;
	striker[PLAYER_2P].vec.y = 0.0f;
	striker[PLAYER_2P].r = 3.0f;
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
	vector_t input;
	vector_t diff;
	float distance, ratio;

	input = DrvJoystickGetAnalogPos(0);
	striker[PLAYER_1P].speed = 1.3f;
	striker[PLAYER_1P].vec.x = input.x * striker[PLAYER_1P].speed;
	striker[PLAYER_1P].vec.y = input.y * striker[PLAYER_1P].speed;
	input = DrvJoystickGetAnalogPos(2);
	striker[PLAYER_2P].vec.x = - input.x;
	striker[PLAYER_2P].vec.y = - input.y;
	striker[PLAYER_2P].speed = 1.0f;

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

				/* 衝突後ベクトル計算 */
				pack[i_p].vec = calcCollision(&(pack[i_p]), &(striker[i_s]), 1.8f);
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
