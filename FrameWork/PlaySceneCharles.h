﻿#pragma once
#include "PlayScene.h"
#include "Canon.h"

class PlaySceneCharles : public PlayScene
{
public:
	//Hàm này dùng để cập nhật Scene (VD Scene có các vật thể chuyển động theo thời gian,...).
	//Đồng thời cũng dùng để cập nhật biến trạng thái hoàn thành.
	void Update(double dt) override;

	void ResetPlayerPosition() override;

	void GoToNextScene() override;

	PlaySceneCharles();
	~PlaySceneCharles();

private:

	//Hằng số dùng để lưu địa chỉ đến các file.
	const char * world01tile = (char*)".\\..\\Resources\\Images\\Maps\\Charles\\charles_tile.png";
	const char * world01map = (char*)".\\..\\Resources\\Images\\Maps\\Charles\\charles_map.txt";
	const char * world01MapObject = (char*) ".\\..\\Resources\\Images\\Maps\\Charles\\charles_map_object.txt";
	const char * world01SpawnLocation = (char*)".\\..\\Resources\\Images\\Maps\\Charles\\charles_map_spawn.txt";

	//Số lượng enemy cần phải giết trước khi cho ra khỏi lock camera.
	const int blueSoldierKillReq = 3;
	const int redRocketKillReq = 3;

	bool isCameraAlreadyLockedOnce = false;

	bool isAddSoldier = false;
};