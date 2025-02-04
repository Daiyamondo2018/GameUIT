#pragma once
#include "Bullet.h"

class BulletMiniNormal : public Bullet
{

public:
	Animation* anim = new Animation(Tag::BOSSMINIBULLET, 1, 2);

	bool CanGetThroughShield() override
	{
		return true;
	}

	BulletMiniNormal()
	{
		this->tag = Tag::BOSSMINIBULLET;
		this->animation = anim;
	}
	~BulletMiniNormal()
	{

	}

	void Update(float dt)override {
		animation->Update(dt);
		this->pos.x += this->vx;
		this->pos.y += this->vy;
	}

	int GetCollisionDamage()override
	{
		return 3;
	}

	void OnCollision(Object* object, collisionOut* colOut)override
	{
		
	}

	bool OnRectCollided(Object* object, CollisionSide side)override
	{
		if (object->tag == Tag::PLAYER)
		{
			this->isCollidable = false;
			return true;
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