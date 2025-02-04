﻿#pragma once
#include "Object.h"
#include"ItemManager.h"
#include"Global.h"
class Container :
	public Object
{
public:
	int ticuframe;
	Animation *animation;
	Item* item;
	int numberItems;
	BoundingBox getBoundingBox();
	void Update(float dt);
	void Render();
	//Container không bị xoá khỏi Grid. Vì vậy nó sẽ không bị deactivated.
	void DeactivateObjectInGrid() override {} ;
	void OnCollision(Object * object, collisionOut* colOut) override;
	bool OnRectCollided(Object* object, CollisionSide side)override;
	void OnNotCollision(Object* object)override;
	void Respawn() {};
	void addItem(Item* item);
	void SetPosition(D3DXVECTOR2 pos);
	//tạo mới một container cần phải tạo tất cả các item trước// dùng hàm loadallitem của itemmanager
	//truyền vào số lượng các items tương ứng của từng loại
	Container(ItemType type, bool containerOfMap2 = false);
	~Container();
};

