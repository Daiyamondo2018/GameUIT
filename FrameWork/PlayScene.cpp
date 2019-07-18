﻿#include "PlayScene.h"
#include "Debug.h"
#include <dinput.h>

PlayScene::~PlayScene()
{
	if (world != nullptr)
		delete world;
	//Còn thiếu hàm release player.
}

PlayScene::PlayScene()
{
	camera = Camera::getCameraInstance();
	player = Player::getInstance();
}

void PlayScene::Draw()
{
	world->Draw();
	player->Render();
}

void PlayScene::ResetCamera()
{
	camera->SetMapProperties(world->getMapHeight(), world->getMapWidth()); //Set properties để camera biết khi nào thì dừng.
	camera->ResetCameraPosition();
}

void PlayScene::Update(double dt)
{
	//Update map trước.
	world->Update(dt);

	//Kiểm tra va chạm.
	CollisionProcess(dt);

	//Xử lý input và cập nhật (nếu có) cho các đối tượng mà PlayScene quản lý.
	//(Thông thường chỉ có player).
	//Sẵn tiện mình xét luôn trong hàm này xem player có ra khỏi map không.
	ProcessUpdates(dt);

	//Sau khi xử lý input và kiểm tra va chạm thì mình mới Update lại Camera dựa trên vị trí mới của player.
	UpdateCameraWithPlayerPos(dt); 

	//Thêm gia tốc (ngoại cảnh tác động nhân vật).
}

void PlayScene::UpdateCameraWithPlayerPos(double dt)
{
	D3DXVECTOR2 playerPos = player->pos;

	int playerWidth = 60;
	int playerHeight = 56;

	RECT cameraBoundBox = Camera::getCameraInstance()->getBoundingBox();
	RECT cameraBoundBoxAdvanced = Camera::getCameraInstance()->getBoundingBoxAdvanced();

	//Chuyển player từ world view sang view port để xét với các bound.
	D3DXVECTOR3 playerViewPort = Camera::getCameraInstance()->convertWorldToViewPort(D3DXVECTOR3(playerPos.x, playerPos.y, 0));

	int leftBound = cameraBoundBox.left;
	int rightBound = cameraBoundBox.right;
	int topBound = cameraBoundBox.top;
	int bottomBound = cameraBoundBox.bottom;

	int leftBoundAdvanced = cameraBoundBoxAdvanced.left;
	int rightBoundAdvanced = cameraBoundBoxAdvanced.right;
	int topBoundAdvanced = cameraBoundBoxAdvanced.top;
	int bottomBoundAdvanced = cameraBoundBoxAdvanced.bottom;

	//Chúng ta sẽ chỉnh lại Camera sao cho khi Player rời khỏi vị trí Critical Line, Camera sẽ bám theo.
	//Đầu tiên chúng ta xem player đang cách các cạnh của màn hình một khoảng bao nhiêu.
	int fromPlayerToTop = playerViewPort.y - playerHeight / 2;
	int fromPlayerToBottom = SCREEN_HEIGHT - playerViewPort.y + playerHeight / 2;
	int fromPlayerToLeft = playerViewPort.x - playerWidth / 2;
	int fromPlayerToRight = SCREEN_WIDTH - playerViewPort.x + playerWidth / 2;

	//Bám theo cơ bản (giữ player trong bounding box của camera).
	if (fromPlayerToTop < topBound)
		camera->MoveUp(topBound - fromPlayerToTop);
	else if (fromPlayerToBottom < bottomBound)
		camera->MoveDown(bottomBound - fromPlayerToBottom);

	if (fromPlayerToLeft < leftBound)
		camera->MoveLeft(leftBound - fromPlayerToLeft);
	else if (fromPlayerToRight < rightBound)
		camera->MoveRight(rightBound - fromPlayerToRight);

	//Bám theo nâng cao (giữ player trong bounding box nhưng đẹp hơn).
	//if (fromPlayerToTop < topBoundAdvanced)
	//	camera->MoveUp(SCREEN_HEIGHT - fromPlayerToTop - bottomBoundAdvanced);
	//else if (fromPlayerToBottom < bottomBoundAdvanced)
	//	camera->MoveDown(SCREEN_HEIGHT - fromPlayerToBottom - topBoundAdvanced);

	//if (fromPlayerToLeft < leftBoundAdvanced)
	//	camera->MoveLeft(leftBoundAdvanced - fromPlayerToLeft);
	//else if (fromPlayerToRight < rightBoundAdvanced)
	//	camera->MoveRight(rightBoundAdvanced - fromPlayerToRight);

}

void PlayScene::EnvironmentUpdate(double dt)
{

}

void PlayScene::CollisionProcess(double dt)
{
	//Kiểm tra collision với ground.
	for (int i = 0; i < mapStaticObject.size(); ++i)
	{
		//Lấy BoxRect của Player.
		BoundingBox playerBox = player->getBoundingBox();
		//Lấy BoxRect của MapObject.
		BoundingBox objectBox = mapStaticObject[i]->getStaticObjectBoundingBox();
		
		
		//if (Collision::getInstance()->IsCollide(playerBox, objectBox))
		//{
		//	int a = 1;
		//}
	
		collisionOut colOut = Collision::getInstance()->SweptAABB(playerBox, objectBox);

		//Gọi đến hàm xử lý va chạm của player.
		if (colOut.side != CollisionSide::none) {
			player->OnCollision(mapStaticObject[i], &colOut);
			if (mapStaticObject[i]->type == Type::GROUND)
				player->SetGroundCollision(new GroundCollision(mapStaticObject[i], colOut.side));
		}
		else {
			// trong trường hợp không còn chạm đất
			if (player->GetGroundCollision()->GetGround() == mapStaticObject[i]) {
				if (!Collision::getInstance()->IsCollide(playerBox, objectBox)) {
					if (player->GetOnAirState() == Player::OnAir::None) {
						player->SetAirState(Player::OnAir::Falling);
						player->ChangeState(State::JUMPING);
						player->SetGroundCollision(NULL);
					}
				}
			}
		}
	}
}

void PlayScene::ProcessUpdates(double dt)
{
	KeyboardManager* inputInstance = KeyboardManager::getInstance();
	player->Update(dt);

	//Kiểm tra nếu player ra khỏi map thì không cho đi tiếp.
	int mapWidth = world->getMapWidth();
	int mapHeight = world->getMapHeight();
	int playerWidth = player->getWidth();
	int playerHeight = player->getHeight();

	//Nếu player ra ngoài map theo hướng bên trái thì ta chỉnh lại.
	if (player->pos.x - playerWidth / 2 < 0)
		player->pos.x = playerWidth / 2 + 1;

	//Player ra ngoài theo hướng bên phải.
	else if (player->pos.x + playerWidth / 2 >= mapWidth)
		player->pos.x = mapWidth - playerWidth / 2 - 1;

	//Player ra ngoài theo hướng bên dưới.
	else if (player->pos.y - playerHeight / 2 < 0)
		player->pos.y = playerHeight / 2 + 1;

	//Player ra ngoài theo hướng bên trên.
	else if (player->pos.y + playerHeight / 2 >= mapHeight)
		player->pos.y = mapHeight - playerHeight / 2 - 1;
}