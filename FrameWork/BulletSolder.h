﻿#pragma once
#include"Bullet.h"
#include"Collision.h"
class BulletSolder:public Bullet {
public:
	BulletSolder()
	{
		this->animation = new Animation(Tag::BLUESOLDERBULLET,0);
		this->isActive = false;
		this->vy = 0;
		this->existTime = 0;
		this->vx = ENEMY_BULLET_SPEED;
	}
	void Update(float dt) override
	{

		if (this->direction == Player::MoveDirection::RightToLeft)
			this->vx = ENEMY_BULLET_SPEED* -1;
		else
		{
			this->vx = ENEMY_BULLET_SPEED;
		}
		this->pos.x += this->vx;
		this->pos.y += this->vy;
		if (existTime >= ENEMY_BULLET_EXIST_TIME)
		{
			existTime = 0;
		}
		else
		{
			existTime += dt;
		}
	}

	void OnCollision(Object* object, collisionOut* colOut)override
	{
		

	}
};