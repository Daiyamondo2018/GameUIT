﻿#include "PlayerDyingState.h"
#include "SceneManager.h"

PlayerDyingState::PlayerDyingState() {
	this->state = State::DEAD;
	this->currentFrame = 0;
}

void PlayerDyingState::InputHandler() {
	auto keyboard = KeyboardManager::getInstance();
	auto player = Player::getInstance();

	
	// nếu chết nhưng nhấn phím 1 thì hồi đầy máu và trở về trạng thái trước đó
	if (keyboard->isKeyDown(DIK_1)) {
		player->SetHeart(3);
		player->ChangeState(player->GetPreviousState()->state);
	}
}

void PlayerDyingState::Update(float dt) {
	auto player = Player::getInstance();
	InputHandler();

	if (++this->currentFrame > ANIMATION_FRAME) {
		this->currentFrame = 0;
		SceneManager::getInstance()->RestartCurrentScene();
	}
}

void PlayerDyingState::OnCollision(Object* object, collisionOut* collision) {
}