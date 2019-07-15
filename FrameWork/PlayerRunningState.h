#pragma once
#include "PlayerState.h"
#include"Player.h"
class PlayerRunningState :
	public PlayerState
{
public:
	void InputHandler();
	void Update(float dt);
	void OnCollision(Object* object, collisionOut* collision) override;
	PlayerRunningState();
	~PlayerRunningState();
};

