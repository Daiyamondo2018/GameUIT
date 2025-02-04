﻿#pragma once
#include "Object.h"
#include "TextureManager.h"
#include "SpriteManager.h"
#include "Player.h"
#include "Delay.h"
class Shield : public Object
{

private:
	static Shield* instance;
	//std::vector<Sprite*>sprites;//chứa danh sách các sprite của shield
	Animation * animation;

	BOOL isActive;

	int numberOfRounds;
	int framePerRound;
	int restFrames;
	BOOL beginRound;
	bool canTurnOnOffLight;
	Delay *delay=new Delay(100);
	
	const float SHIELD_INITIAL_SPEED = 12;

public:

	enum ShieldState {
		Defense,
		Attack,
		Transparent,
		NotRender
	};

	enum MoveBehavior {
		FarFromPlayer,
		BackToPlayer,
		NotMove
	};



	ShieldState state;
	MoveBehavior moveBehave;


	static Shield* getInstance();
	void Update(float dt);
	void Render();
	void RenderInGrid() {}; //Cấm không cho Shield render trong grid.
	void InputHandler(float dt);

	void setFrameIndex(int index);
	void SetShieldState(ShieldState state);
	BoundingBox getBoundingBox() override;

	void SetNumberOfRounds(int numberOfRounds);
	void SetFramePerRound(int fpr);
	void Move();
	BOOL MoveOutFromPlayer(int fpr);
	BOOL MoveBackToPlayer(int fpr);

	void UpdatePositionVector();
	void ResetMoveStatus();

	Shield();
	~Shield();

	void DeactivateObjectInGrid() override {};
	void SetMoveDirection(MoveDirection moveDir) override;

	int GetCollisionDamage() override { return 2; }

	void ChangeState(State stateName) override {};
	void OnCollision(Object* object, collisionOut* out) override;
	void OnNotCollision(Object* object) override {};
	bool OnRectCollided(Object* object, CollisionSide side) override { return false; };
	void ShieldBackToPlayer();
};

