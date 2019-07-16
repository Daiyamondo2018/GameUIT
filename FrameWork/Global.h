﻿#pragma once

#include <d3dx9.h>
#include <d3d9.h>
#include <dinput.h>
#include <WinUser.h>
#include <unordered_map>

#define WINDOW_CLASS_NAME "Frame Work"		
#define GAME_TITLE "Frame Work"		
#define SCREEN_WIDTH 256						
#define SCREEN_HEIGHT 224			
#define MAX_FRAME_RATE 90						
#define BALL_WIDTH 40
#define BALL_HEIGHT 40
#define BAT_WIDTH 90
#define BAT_HEIGHT 30
#define TIME_PER_FRAME 150

//Normal Player Define
#define NORMALPLAYER_WIDTH 40
#define NORMALPALYER_HEIGHT 40
#define PLAYER_MAX_JUMPING_HEIGHT 500
#define PLAYER_NORMAL_SPEED 2
#define PLAYER_JUMP_SPEED 10
#define GROUND_GRAVITY 0.5

// Moving keyboard input
#define PLAYER_MOVE_LEFT DIK_A
#define PLAYER_MOVE_RIGHT DIK_D
#define PLAYER_JUMP DIK_SPACE

#define DENTA_PIXEL 4

#define TILE_SIZE 16
#define NUMBER_MAP 190


class AnimFilePath
{
public:
	static char * WATER_TOP_ANIM;
	static char * WATER_BOTTOM_ANIM;
	static char * SEWER_ANIM;
};

enum Tag {

	PLAYER,
	ITEM,
	MAP1,
	TileSet1,
	MAP2,
	MAP3,
	MAP4,
	ENERMY,
	BULLET,
	SHIELD,
	WATERTOP,
	WATERBOTTOM,
	SEWER
};
enum Type {
	NONE,
	CLOUD,
	BRICK,
	WIZARD,
	MINIBOSS,
	LOCOT,
	GROUND,
	SOLIDBOX,
	WATERRL
	//cac loai dan
};
enum State {
	STANDING,
	RUNNING,
	WALKING,
	JUMPING,
	DASHING,
	FLYING,
	SITTING,
	DEAD,
	ATTACKING,
	ATTACKING_STAND,
	ATTACKING_SIT,
	SWIMING,
	ATTACKING_FLY,
	FALLING
};

extern HINSTANCE hInstance;
extern HWND hwnd;
extern LPD3DXSPRITE spriteHandler;
extern LPDIRECT3DDEVICE9 d3ddev;