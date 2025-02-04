#pragma once
#include "Enemy.h"
#include "Animation.h"
#include "CanonBullet.h"
#include "math.h"
#include "SceneManager.h"

class Canon : public Enemy{
protected:
	Animation animation;
	int curFrameRotate;
	int curFrameFire;
	bool isRotate;

	const int TIME_FIRE = 45;
	const int BULLET_SPEED = 2;
	const int FRAME_PER_ROTATE = 50;

public:
	enum RotateDirection {
		Top,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left,
		TopLeft
	};
	Canon();
	Canon(RotateDirection direction);
	Canon(RotateDirection direction, D3DXVECTOR2 position);
	~Canon();

	int GetCollisionDamage() override;

	void Fire();
	void Rotate();

	bool IsRotate();
	bool IsExplode();
	bool IsExplodeDone();

	void SetCanonDirection(RotateDirection direction);

	bool OnRectCollided(Object* object, CollisionSide side) override;
	void Render() override;
	void Update(float dt) override;

	void OnStartRotate();
	void KillObject() override;
};