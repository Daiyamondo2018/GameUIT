#pragma once
#include "Enemy.h""
#include"Shield.h"
#include"BulletManager.h"

class Solder : public Enemy {
private:
public:
	Solder(RunType runType, float x, float y);
	~Solder();
	bool canJump;
	State stateName;
	RunType runType;
	float timeCurrentState;
	float deltaY = 0;
	float PosToBottom();
	float delayBeaten;
	float delayToDead;
	const float maxDelayBeaten=2000;
	std::unordered_map<State, Animation*> animations;
	void LoadAllAnimation();
	void ChangeState(State state)override; 
	void OnCollision(Object* object, collisionOut* colOut) override;
	bool OnRectCollided(Object* object, CollisionSide side)override;
	void OnNotCollision(Object* oject)override;
	void Update(float dt) override;
	void Render();
	float getHeight()override;
	float getWidth()override;
	BoundingBox getBoundingBox()override;
};