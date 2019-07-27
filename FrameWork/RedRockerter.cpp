﻿#include"RedRockerter.h"
#include "SceneManager.h"

RedRockerter::RedRockerter()
{
	LoadAllAnimation();
	timeCurrentState = 0;
	this->vx = ENEMY_SPEED;
	ChangeState(State::RUNNING);
	canRun = true;
}

RedRockerter::~RedRockerter()
{
	
}


void RedRockerter::OnCollision(Object* object, collisionOut* colOut) {

}

void RedRockerter::Update(float dt)
{
	if (isDead)
		return;
	auto player = Player::getInstance();
	float deltax = this->pos.x - player->pos.x;
	if (deltax > 0)
		this->direction = Player::MoveDirection::RightToLeft;
	else
	{
		this->direction = Player::MoveDirection::LeftToRight;
	}
	this->pos.x += this->vx;
	this->pos.y += this->vy;
	// nếu đang ở trạng thái beaten và hết thời gian của trạng thái beaten thì blue solder bị chết
	if (stateName == State::BEATEN&&timeCurrentState >= ENEMY_BEATEN_TIME)
		isDead = true;
	auto shield = Shield::getInstance();
	//nếu bị shield đánh chuyển sang beaten---> test 
	if (Collision::getInstance()->IsCollide(shield->getBoundingBox(), this->getBoundingBox()) && shield->state == Shield::ShieldState::Attack)
		ChangeState(State::BEATEN);
	this->curentAnimation->Update(dt);
	switch (this->stateName)
	{	
	case State::STANDING:
		if (timeCurrentState < RED_ROCKERTER_STATE_TIME)
		{
			if (timeCurrentState < RED_ROCKERTER_STATE_TIME / 2 && timeCurrentState + dt >= RED_ROCKERTER_STATE_TIME / 2)
			{
				auto bullet = BulletManager::getInstance()->CreateBullet(Tag::REDROCKERTERBULLET);
				bullet->existTime = 0;
				bullet->animation->curframeindex = 0;
				bullet->SetActive(true);
				bullet->direction = this->direction;
				bullet->pos.y = this->getBoundingBox().top - 10;
				if (this->direction == Player::MoveDirection::LeftToRight)
					bullet->pos.x = this->pos.x + 5;
				else
				{
					bullet->pos.x = this->pos.x - 5;
				}
				SceneManager::getInstance()->AddObjectToCurrentScene(bullet);
			}
			timeCurrentState += dt;
		}
		else
		{
			if (canRun)
			{
				ChangeState(State::RUNNING);
				canRun = false;
			}
			else
			{
				ChangeState(State::DUCKING);
			}
		}
		break;
	case State::DUCKING:
		if (timeCurrentState < RED_ROCKERTER_STATE_TIME)
		{
			if (timeCurrentState < RED_ROCKERTER_STATE_TIME / 2 && timeCurrentState + dt >= RED_ROCKERTER_STATE_TIME / 2)
			{
				auto bullet2 = BulletManager::getInstance()->CreateBullet(Tag::REDROCKERTERBULLET);
				bullet2->existTime = 0;
				bullet2->animation->curframeindex = 0;
				bullet2->SetActive(true);
				bullet2->direction = this->direction;
				bullet2->pos.y = this->getBoundingBox().top - 6;
				if (this->direction == Player::MoveDirection::LeftToRight)
					bullet2->pos.x = this->pos.x + 5;
				else
				{
					bullet2->pos.x = this->pos.x - 5;
				}
				SceneManager::getInstance()->AddObjectToCurrentScene(bullet2);
			}
			timeCurrentState += dt;
		}
		else
		{
			ChangeState(State::STANDING);
		}
		break;
	case State::RUNNING:
		if (timeCurrentState < RED_ROCKERTER_STATE_TIME)
			timeCurrentState += dt;
		else
		{
			ChangeState(State::STANDING);
		}
		break;
	default:
		timeCurrentState += dt;
		break;
	}

}

void RedRockerter::Render()
{
	if (isDead)
		return;
	D3DXVECTOR3 pos = Camera::getCameraInstance()->convertWorldToViewPort(D3DXVECTOR3(this->pos));
	switch (this->direction)
	{
	case Player::MoveDirection::LeftToRight:
		this->curentAnimation->Render(D3DXVECTOR2(pos), TransformationMode::FlipHorizontal);
		break;
	case Player::MoveDirection::RightToLeft:
		this->curentAnimation->Render(pos);
		break;
	default:
		break;
	}
}


BoundingBox RedRockerter::getBoundingBox()
{
	switch (this->stateName)
	{
	case State::BEATEN:
	case State::RUNNING:
	case State::STANDING:
		return Object::getBoundingBox();
	case State::DUCKING:
	{
		BoundingBox box;
		box.vx = this->vx;
		box.vy = this->vy;
		box.top = this->pos.y + 8;
		box.bottom = this->pos.y - 23;
		box.left = this->pos.x - 12;
		box.right = this->pos.x + 12;
		return box;
	}
	default:
		break;
	}
}

void RedRockerter::LoadAllAnimation()
{
	animations[State::RUNNING] = new Animation(Tag::REDROCKERTER, 0, 3);// run
	animations[State::STANDING] = new Animation(Tag::REDROCKERTER, 0);// run
	animations[State::DUCKING] = new Animation(Tag::REDROCKERTER, 3);//duck
	animations[State::BEATEN] = new Animation(Tag::REDROCKERTER, 4);//beaten
}

void RedRockerter::ChangeState(State stateName)
{

	switch (stateName)
	{
	case State::RUNNING:
		this->curentAnimation = animations[State::RUNNING];
		if (this->direction == Player::MoveDirection::LeftToRight)
			this->vx = ENEMY_SPEED;
		else
			this->vx = -ENEMY_SPEED;
		isDead = false;
		break;
	case State::DUCKING:
		this->curentAnimation = animations[State::DUCKING];
		this->vx = this->vy = 0;
		isDead = false;
		break;
	case State::BEATEN:
		this->curentAnimation = animations[State::BEATEN];
		this->vx = this->vy = 0;
		if (this->direction == Player::MoveDirection::LeftToRight)
			this->pos.x -= 3;
		else
		{
			this->pos.x += 3;
		}
		break;
	case State::STANDING:
		this->curentAnimation = animations[State::STANDING];
		this->vx = this->vy = 0;
		isDead = false;
		break;
	default:
		break;
	}
	this->stateName = stateName;
	timeCurrentState = 0;
}
