﻿#include "PlayerDuckingPunchingState.h"

PlayerDuckingPunchingState::PlayerDuckingPunchingState() {
	this->state = State::DUCKING_PUNCHING;
}

void PlayerDuckingPunchingState::InputHandler() {
	auto player = Player::getInstance();
	auto keyboard = KeyboardManager::getInstance();
	if (player == NULL || keyboard == NULL)
		return;

	// khi đang ngồi nhưng phím đánh đang được giữ
	if (!keyboard->getKeyPressedOnce(PLAYER_ATTACK)) {
		player->ChangeState(State::DUCKING);
	}
}

void PlayerDuckingPunchingState::Update(float dt) {
	this->InputHandler();
}

void PlayerDuckingPunchingState::OnCollision(Object* object, collisionOut* collision) {

}