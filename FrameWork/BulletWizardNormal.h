#pragma once
#include "Bullet.h"

class BulletWizardNormal : public Bullet
{
	
public:
	
	Animation* animation1 = new Animation(Tag::BOSSWIZARDBULLET, 3, 4);
	Animation* animation2 = new Animation(Tag::BOSSWIZARDBULLET, 4, 5);
	Animation* animation3 = new Animation(Tag::BOSSWIZARDBULLET, 5, 6);
	BulletWizardNormal()
	{
		this->tag = Tag::BOSSWIZARDBULLET;
	}
	~BulletWizardNormal()
	{
		
	}

	void Update(float dt)override {
		if (animation->curframeindex == 2)
			DeactivateObjectInGrid();
		animation->Update(dt);
		this->pos.x += this->vx;
		this->pos.y += this->vy;
	}

	void OnCollision(Object* object, collisionOut* colOut)override
	{
		if (object->tag == this->tag)
			return;
		switch (object->type)
		{
		case Type::SOLIDBOX:
		case Type::GROUND:
			this->animation = animationExplode;
			this->vx = this->vy = 0;
			break;
		default:
			break;
		}
		if (object->type == Type::NONE)
		{
			if (object->tag == Tag::PLAYER)
			{
				this->animation = animationExplode;
				this->vx = this->vy = 0;
			}
		}
		switch (colOut->side)
		{
		case CollisionSide::top:
		case CollisionSide::bottom:
			this->pos.y += this->vy;
			break;
		case CollisionSide::left:
		case CollisionSide::right:
			this->pos.y += this->vx;
			break;
		default:
			break;
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