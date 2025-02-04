﻿#pragma once
#include "Sprite.h"
#include "TileSet.h"
#include "Camera.h"
#include "Animation.h"

class TileMap
{
protected:
	TileSet* tileSet; //Sprite của các loại tile.
	int ** tileMap; //File txt chứa Map.
	int maxCol;
	int maxRow;
	int tileSize;
	int numOfTileType;
public:
	//TileMap load theo hàng trước xong mới đến cột.
	TileMap(const char * tileSetFile, const char * tileMapFile, Tag tag);
	~TileMap();
	long GetMapHeight() { return maxRow * tileSize; };
	long GetMapWidth() { return maxCol * tileSize; };
	virtual void Update(double dt);
	virtual void Draw(Camera* camera);
};