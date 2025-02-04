﻿#include "PlaySceneCharles.h"
#include "BulletWhiteFlyingRocketer.h"
#include "WhiteFlyingRobot.h"
#include "BulletRedRocket.h"
#include "RedRocketRobotOneSided.h"
#include "RedRocketRobotTwoSided.h"
#include "RedRocketRobotTwoSidedNonLinear.h"
#include "RedRocketRobotNonShooting.h"
#include "WhiteRocketRobotNonShooting.h"
#include "MovingPlatformCircle.h"
#include "MovingPlatformCrossed.h"
#include "MovingPlatformHorizontal.h"
#include "DisappearingPlatform.h"
#include "PlatformFire.h"
#include "Door.h"
#include "Shockwave.h"
#include "Camera.h"	
#include "Solder.h"
#include "GreenSolder.h"
#include "Canon.h"
#include"GreenSolder.h"

PlaySceneCharles::PlaySceneCharles()
{
	//Các singleton đã được tự động load khi gọi hàm khởi tạo tại class cha.
	//Vì vậy chúng ta không cần thực hiện lại việc load singleton.

	world = new GameMap(world01tile, world01map, world01MapObject, MapName::CHARLES);
	world->SetCamera(camera);
	camera->SetMapProperties(world->getMapHeight(), world->getMapWidth());
	//mapStaticObject = world->getStaticObject(); //Lấy entity của tất cả các object có trong map.

	//Xét tạo Grid.
	grid = new Grid(world->getMapWidth(), world->getMapHeight(), world01SpawnLocation, world01MapObject);
}

PlaySceneCharles::~PlaySceneCharles()
{

}

void PlaySceneCharles::ResetPlayerPosition()
{
	//Reset lại player luôn.
	Player * player = Player::getInstance();
	player->pos.x = 0;
	player->pos.y = SCREEN_HEIGHT + 20;
	player->ChangeState(State::JUMPING);
	player->SetOnAirState(Player::OnAir::Falling);

	//Update lại camera sau khi set.
	UpdateCameraWithPlayerPos();
}

void PlaySceneCharles::Update(double dt)
{
	PlayScene::Update(dt);
	CameraInfo camInfo = Camera::getCameraInstance()->GetCurrentCameraPosition();
	//Nếu chưa từng khoá camera thì tới đúng thời điểm ta sẽ khoá.
	if (!isCameraAlreadyLockedOnce)
	{
		//Xét nếu player nằm trong vùng lock thì lock camera lại.
		auto inRange = [](int inputValue, int valueToCompare, int range = 10) {return ( (valueToCompare - range) < inputValue) && (inputValue < (valueToCompare + range)); };
		if (inRange(camInfo.topLeftX + (double)SCREEN_WIDTH / 2, 385) && inRange(player->pos.y, 50, 100))
		{
			camera->LockCamera();
			isCameraAlreadyLockedOnce = true;
			this->KillAllEnemyInActiveCells();
			grid->StartEnemyBeatenCounter();
			SoundManager::getinstance()->stop(SoundManager::SoundName::main_theme);
			SoundManager::getinstance()->play(SoundManager::SoundName::action_theme);
		}
	}

	//Xét nếu đang bị khoá camera thì ta sẽ spawn liên tục cho đến khi giết đủ số lượng enemy.
	if (camera->GetCameraLockState())
	{
		//Thêm vào con robot đỏ bên phải.
		if (!isAddSoldier)
		{
			Object* redRobot = new RedRocketRobotNonShooting(550, 65);
			if (!this->AddObjectToPlayScene(redRobot))
				delete redRobot;
			else
				isAddSoldier = true;
		}
		else
		{
			Object* blueSoldier = new Solder(RunType::CANRUN, 250, 85);
			if (!this->AddObjectToPlayScene(blueSoldier))
				delete blueSoldier;
			else
				isAddSoldier = false;
		}
		//Trường hợp thoát ra khỏi locking.
		int blueSoldierBeaten = grid->GetBlueSoldierBeatenCounter();
		int redRocketBeaten = grid->GetRedRocketRobotBeatenCounter();
		if (blueSoldierBeaten > blueSoldierKillReq && redRocketBeaten > redRocketKillReq)
		{
			grid->StopEnemyBeatenCounter();
			camera->UnlockCamera();
			isUsingVirtualBound = true;
			leftVirtualCameraBound = -1;
			rightVirtualCameraBound = -1;
			SoundManager::getinstance()->stop(SoundManager::SoundName::action_theme);
			SoundManager::getinstance()->play(SoundManager::SoundName::main_theme);
		}
	}

	//Xét thêm điều kiện hoàn thành là khi player đi đến tận cùng phải của map.
	if (player->pos.x >= world->getMapWidth() - 100)
	{
		if (player->CanGoNextScene())
		{
			GoToNextScene();
		}
	}

	if (KeyboardManager::getInstance()->getKeyPressedOnce(DIK_Q))
	{
		GoToNextScene();
	}
}

void PlaySceneCharles::GoToNextScene()
{
	Done = true;
	ReplaceToThisMap = MapName::CHARLESBOSSLIGHT;
}