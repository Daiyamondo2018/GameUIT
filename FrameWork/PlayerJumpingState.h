#pragma once
#include "PlayerState.h"
#include "Player.h"
#include "Global.h"


class PlayerJumpingState : public PlayerState{
public:
	PlayerJumpingState();
	~PlayerJumpingState();
	void InputHandler() override;
	void Update(float dt) override;
	void OnCollision(Object* object, collisionOut* collision) override;
};