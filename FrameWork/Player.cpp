﻿#include"Player.h"
#include"Camera.h"
#include"Debug.h"

Player* Player::instance = NULL;

Player::Player()
{
	LoadAllAnimations();
	LoadAllStates();

	this->pos.x = 30;
	this->pos.y = 80;
	this->tag = Tag::PLAYER;
	this->health = 100;
	this->energy = 0;
	this->vx = 0;
	this->vy = 0;
	this->hasShield = true;
	this->direction = MoveDirection::LeftToRight;

	this->state = State::JUMPING;
	this->playerstate = playerStates[State::JUMPING];
	this->curanimation = animations[this->state];
	this->prevState = NULL;
	this->collisionAffect = TRUE;

	this->groundCollision = new GroundCollision();
}

PlayerState* Player::GetPreviousState() {
	return this->prevState;
}

void Player::SetPreviousState(State stateName) {
	this->prevState = playerStates[stateName];
}

void Player::LoadAllAnimations() {
	animations[STANDING] = new Animation(PLAYER, 0, 1);
	animations[RUNNING] = new Animation(PLAYER, 1, 5);
	animations[JUMPING] = new Animation(PLAYER, 7, 8);
	animations[DUCKING]	= new Animation(PLAYER, 6, 7);
	animations[DUCKING_PUNCHING] = new Animation(PLAYER, 15, 17);
	animations[SHIELD_UP] = new Animation(PLAYER, 5, 6);
	animations[ROLLING] = new Animation(PLAYER, 8, 10);
	animations[KICKING] = new Animation(PLAYER, 10, 11);
	animations[DASHING] = new Animation(PLAYER, 17, 19);
	animations[FLOATING] = new Animation(PLAYER, 32, 40);
	animations[DIVING] = new Animation(PLAYER, 40, 46);
}

void Player::LoadAllStates() {
	this->playerStates[State::STANDING] = new PlayerStandingState();
	this->playerStates[State::RUNNING]  = new PlayerRunningState();
	this->playerStates[State::JUMPING]  = new PlayerJumpingState();
	this->playerStates[State::DUCKING]  = new PlayerSittingState();
	this->playerStates[State::DUCKING_PUNCHING] = new PlayerDuckingPunchingState();
	this->playerStates[State::SHIELD_UP] = new PlayerShieldUpState();
	this->playerStates[State::KICKING] = new PlayerKickingState();
	this->playerStates[State::ROLLING] = new PlayerRollingState();
	this->playerStates[State::DASHING] = new PlayerDashingState();
	this->playerStates[State::FLOATING] = new PlayerFloatingState();
	this->playerStates[State::DIVING] = new PlayerDivingState();
}




Player::~Player()
{

}


BoundingBox Player::getBoundingBox()
{
	return this->playerstate->getBoundingBox();
}

void Player::Update(float dt)
{
	this->playerstate->Update(dt);
	// Update lại vị trí của position sau mỗi frame
	this->UpdatePosition();
	// Update animation
	this->curanimation->Update(dt);
}

void Player::Render()
{
	D3DXVECTOR3 vectortoDraw = Camera::getCameraInstance()->convertWorldToViewPort(D3DXVECTOR3(this->pos.x,pos.y,0));
	if (this->direction == Player::MoveDirection::LeftToRight) {
		// move from left to right
		curanimation->Render(D3DXVECTOR2(vectortoDraw.x, vectortoDraw.y),TransformationMode::FlipHorizontal);
	}
	else {
		curanimation->Render(D3DXVECTOR2(vectortoDraw.x, vectortoDraw.y));
	}
}

void Player::HandleGroundCollision(Object* ground, collisionOut* colOut) {
	auto playerBox = this->getBoundingBox();
	auto groundBox = ground->getStaticObjectBoundingBox();
	this->SetGroundCollision(new GroundCollision(ground, colOut->side));
	this->ChangeState(State::STANDING);
	this->pos.y = ground->pos.y + this->getHeight() / 2;
}

void Player::OnCollision(Object* object, collisionOut* collisionOut) {
	if (object->pos.y == 44) {
		int a = 1;
	}
	this->playerstate->OnCollision(object, collisionOut);
}

Player* Player::getInstance()
{
	if (!instance)
		instance = new Player();
	return instance;
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
	if (this->state == stateName)
		return;

	InnerChangeState(stateName);

	// thay đổi riêng biệt cho từng loại state
	switch (stateName) {
		case State::DUCKING:
			this->hasShield = true;
		case State::STANDING: {
			this->hasShield = true;
			this->SetAirState(Player::OnAir::None);
			this->SetVx(0);
			break;
		}
		case State::RUNNING:
		{
			this->hasShield = true;
			this->SetAirState(Player::OnAir::None);
			if (this->direction == MoveDirection::LeftToRight)
				SetVx(PLAYER_NORMAL_SPEED);
			else {
				SetVx(-PLAYER_NORMAL_SPEED);
			}
			break;
		}
		case State::JUMPING: 
		{
			this->hasShield = true;
			// nếu trước đó đang falling tiếp tục là falling
			if (this->onAirState == OnAir::Falling || this->onAirState == OnAir::DropToWater) {
				return;
			}
			if (this->prevState->state == State::FLOATING) {
				this->SetAirState(OnAir::JumpFromWater);
				return;
			}
			this->SetAirState(OnAir::Jumping);
			break;
		}
		case State::DASHING:
		{
			this->hasShield = false;
			if (this->direction == MoveDirection::LeftToRight) {
				SetVx(PLAYER_DASH_SPEED);
			}
			else {
				SetVx(-PLAYER_DASH_SPEED);
			}
			break;
		}
		case State::DIVING: {
			this->hasShield = false;
			this->collisionAffect = FALSE;
		}
		case State::FLOATING: {
			this->hasShield = false;
			this->SetAirState(OnAir::None);
			this->vx = -WATER_SPEED;
			this->hasShield = FALSE;
			break;
		}
		case State::ROLLING:
		{
			this->hasShield = false;
			break;
		}
		case State::KICKING: 
		{
			this->hasShield = true;
			break;
		}
	}
}

BOOL Player::IsOnMainGround() {
	return this->groundCollision->GetGround()->pos.y == 44;
}

BOOL Player::IsReachMaxJump() {
	return this->vy <= 0 && this->onAirState == OnAir::Jumping;
}

void Player::Float(MoveDirection moveDir) {
	// nếu không ở trạng thái nổi thì thoát
	if (this->playerstate->state != State::FLOATING)
		return;

	this->direction = moveDir;

	if (this->direction == MoveDirection::LeftToRight) {
		// bơi từ trái qua phải thì trừ đi v của nước
		this->vx = PLAYER_NORMAL_SPEED - WATER_SPEED;
	}
	else {
		this->vx = -PLAYER_NORMAL_SPEED - WATER_SPEED;
	}

}

void Player::InnerChangeState(State stateName) {
	SetPreviousState(this->state);
	this->state = stateName;
	this->playerstate = playerStates[stateName];
	curanimation = animations[stateName];
}

void Player::SetVx(float vx) {
	this->vx = vx;
	if (vx == 0)
		return;
	if (this->direction == MoveDirection::LeftToRight) {
		if (vx < 0) {
			this->direction = MoveDirection::RightToLeft;
		}
	}
	else {
		if (vx > 0) {
			this->direction = MoveDirection::LeftToRight;
		}
	}
}

void Player::SetVy(float vy) {
	this->vy = vy;
	if (this->vy <= 0) {
		// Nếu vận tốc <= 0 khi đang ở trạng thái nhảy thì rơi xuống lại
		if (this->onAirState == OnAir::Jumping)
			SetAirState(OnAir::Falling);
	}
}


void Player::SetVelocity(D3DXVECTOR2 veloc) {
	this->vx = veloc.x;
	this->vy = veloc.y;
}

void Player::AddPosX() {
	this->vx += this->accelerate.x;
	this->pos.x += this->vx;
}

void Player::AddPosY() {

	this->SetVy(this->vy + this->accelerate.y);
	this->pos.y += this->vy;
}

void Player::UpdatePosition() {
	this->accelerate.x = 0;
	AddPosX();
	AddPosY();
}

void Player::SetMoveDirection(MoveDirection moveDir) {
	auto curMoveDir = this->direction;
	this->direction = moveDir;
	if (this->direction != curMoveDir) {
		this->vx *= -1;
	}
}

void Player::SetAccelerate(D3DXVECTOR2 accelerate) {
	this->accelerate.x = accelerate.x;
	this->accelerate.y = accelerate.y;
}

void Player::SetGroundCollision(GroundCollision* groundCollision) {
	if (this->groundCollision != groundCollision) {
		delete this->groundCollision;
		this->groundCollision = groundCollision;
	}
}

GroundCollision* Player::GetGroundCollision() {
	return this->groundCollision;
}

Player::OnAir Player::GetOnAirState() {
	return this->onAirState;
}

void Player::SetAirState(OnAir onAirState) {
	if (this->onAirState == onAirState)
		return;
	auto oldState = this->onAirState;
	this->preOnAir = oldState;
	this->onAirState = onAirState;

	switch (this->onAirState) {
		case OnAir::None:
		{
			this->accelerate.y = 0;
			this->vy = 0;
			return;
		}
		case OnAir::Jumping: {
			this->accelerate.y = -GROUND_GRAVITY;
			this->vy = PLAYER_JUMP_SPEED;
			return;
		}
		case OnAir::DropToWater: {
			this->vy = -2;
			return;
		}
		case OnAir::Falling: {
			this->accelerate.y = -GROUND_GRAVITY;
			return;
		}
		case OnAir::JumpFromWater: {
			this->vy = 5;
			this->accelerate.y = -0.25;
			return;
		}
	}
}

void Player::HandleWaterCollision(Object* water, collisionOut* colOut) {
	this->ChangeState(State::FLOATING);
	auto objBox = water->getStaticObjectBoundingBox();
	this->pos.y = (objBox.top - objBox.bottom) / 2;
}


int Player::getWidth()
{
	BoundingBox box = this->playerstate->getBoundingBox();
	return box.right - box.left;
}

int Player::getHeight()
{
	BoundingBox box = this->playerstate->getBoundingBox();
	return box.top - box.bottom;
}