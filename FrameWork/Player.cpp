﻿#include"Player.h"
#include"Camera.h"

Player* Player::instance = NULL;

Player::Player()
{
	animations[STANDING] = new Animation(PLAYER, 0);
	animations[RUNNING] = new Animation(PLAYER, 0, 3, TIME_PER_FRAME / 4);
	this->state = STANDING;
	this->pos.x = 0;
	this->pos.y = 100;
	this->tag = Tag::PLAYER;
	this->health = 100;
	this->energy = 0;
	this->vx = 0;
	this->vy = 0;
	this->direction = MoveDirection::LeftToRight;
	curanimation = animations[this->state];

	LoadAllStates();
	this->playerstate = standingState;
}

void Player::LoadAllStates() {
	this->runningState = new PlayerRunningState();
	this->standingState = new PlayerStandingState();
}


Player::~Player()
{

}


void Player::Update(float dt)
{
	this->playerstate->Update(dt);
	
}

void Player::Render()
{
	D3DXVECTOR3 vectortoDraw = Camera::getCameraInstance()->convertWorldToViewPort(D3DXVECTOR3(this->pos.x,pos.y,0));
	if (this->direction == Player::MoveDirection::LeftToRight) {
		// move from right to left
		curanimation->Render(D3DXVECTOR2(vectortoDraw.x, vectortoDraw.y));
	}
	else {
		curanimation->Render(D3DXVECTOR2(vectortoDraw.x, vectortoDraw.y), TransformationMode::FlipHorizontal);
	}
}

void Player::OnCollision(Object* object, collisionOut* collisionOut) {
	this->playerstate->OnCollision(object, collisionOut);
}

Player* Player::getInstance()
{
	if (!instance)
		instance = new Player();
	return instance;
}




void Player::KeyDown()
{


	if (KeyboardManager::getInstance()->isKeyDown(DIK_UP))
	{
		
	}

	if (KeyboardManager::getInstance()->isKeyDown(DIK_DOWN))
	{
		
	}

}

void Player::ChangeState(PlayerState* newplayerstate)
{
	delete playerstate;
	playerstate = newplayerstate;
	playerstate->state = newplayerstate->state;
	curanimation = animations[playerstate->state];
	this->state = playerstate->state;
}

void Player::ChangeState(State stateName) {
	switch (stateName) {
	case State::STANDING: InnerChangeState(standingState);break;
	case State::RUNNING:  InnerChangeState(runningState);break;
	}
}

void Player::InnerChangeState(PlayerState* state) {
	this->state = state->state;
	playerstate = state;
	curanimation = animations[playerstate->state];
}

int Player::getWidth()
{
	return this->curanimation->getWidth();
}

int Player::getHeight()
{
	return this->curanimation->getHeight();
}