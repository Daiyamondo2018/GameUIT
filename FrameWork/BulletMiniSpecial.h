﻿#pragma once
#include "Bullet.h"
#include"Equation.h"
#include"Shield.h"
class BulletMiniSpecial : public Bullet
{

public:
	bool isBeaten;
	bool isOnBossMini;
	Equation *parapol;
	Animation* anim = new Animation(Tag::BOSSMINIBULLET, 0, 1);
	float minMap, maxMap;
	/*bool CanGetThroughShield() override
	{
		return true;
	}*/

	BulletMiniSpecial(D3DXVECTOR2 pos1, D3DXVECTOR2 pos2)
	{
		this->tag = Tag::BOSSMINIBULLET;
		this->animation = anim;
		parapol = new Equation(pos1, pos2);
		this->isOnBossMini = false;
		this->isBeaten = false;
		this->vx = this->vy = 0;
		minMap = 0;
		maxMap = 256;
	}
	~BulletMiniSpecial()
	{

	}
	void setCollidale(bool collide)
	{
		this->isCollidable = collide;
	}
	void Update(float dt)override {
		if (animation->curframeindex == animationExplode->toframe-1)
		{
			SoundManager::getinstance()->play(SoundManager::SoundName::object_explode);
			DeactivateObjectInGrid();
		}
		animation->Update(dt);
		/*this->vy = (this->parapol->GetYFromX(this->pos.x + this->vx) - this->parapol->GetYFromX(this->pos.x));
		if (vy < -3)
			this->vx *= 0.9;
		this->pos.x += this->vx;

		this->pos.y = this->parapol->GetYFromX(this->pos.x);*/
		
		if (parapol != NULL)
		{
			this->vy = (this->parapol->GetYFromX(this->pos.x + this->vx) - this->parapol->GetYFromX(this->pos.x));
			if (vy < -3)
				this->vx *= 0.9;

			this->pos.x += this->vx;
			this->pos.y = this->parapol->GetYFromX(this->pos.x);

			if (this->pos.x < minMap + this->getWidth() / 2 && this->direction == MoveDirection::RightToLeft) // đầu map thì chuyển direction
			{
				this->direction = MoveDirection::LeftToRight;
				this->vx = 1;
				this->vy = -1;
				this->parapol = NULL;
				return;
			}
			if (this->pos.x > maxMap - this->getWidth() / 2 && this->direction == MoveDirection::LeftToRight)// cuối map thì chuyển direction
			{
				this->direction = MoveDirection::RightToLeft;
				this->vy = -1;
				this->vx = -1;
				this->parapol = NULL;
				return;
			}
		}
		else
		{
			this->pos.x += this->vx;
			this->pos.y += this->vy;
		}
	}

	int GetCollisionDamage()override
	{
		return 1;
	}

	void OnCollision(Object* object, collisionOut* colOut)
	{
		if (object->tag == Tag::PLAYER_PART)
		{
			if (this->isOnBossMini)
				this->isBeaten = true;
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			return;
		}
		switch (object->type)
		{
		case Type::GROUND:
		case Type::SOLIDBOX:
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			break;
		default:
			break;
		}
	}

	bool OnRectCollided(Object* object, CollisionSide side)
	{
		switch (object->type)
		{
		case Type::GROUND:
		case Type::SOLIDBOX:
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			break;
		default:
			break;
		}
		if (object->tag == Tag::PLAYER_PART)
		{
			if (this->isOnBossMini)
				this->isBeaten = true;
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			return false;
		}
		/*if (object->tag == Tag::PLAYER)
		{
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			return true;
		}*/
		if (object->tag == Tag::SHIELD&&Shield::getInstance()->state==Shield::ShieldState::Attack)
		{
			if (this->isOnBossMini)
				this->isBeaten = true;
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			return false;
		}
	
	}

	BoundingBox getBoundingBox()override
	{
		BoundingBox box;
		box.vx = this->vx;
		box.vy = this->vy;
		auto sprite = this->animation->getSprite(this->animation->curframeindex);
		RECT rect = sprite->getRECT();
		height = rect.top - rect.bottom;
		width = rect.right - rect.left;
		box.top = this->pos.y + height / 2;
		box.bottom = this->pos.y - height / 2;
		box.left = this->pos.x - width / 2;
		box.right = this->pos.x + width / 2;
		return box;
	}
};