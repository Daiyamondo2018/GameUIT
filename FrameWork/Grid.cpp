﻿#pragma once
#include "Grid.h"
#include "RedRocketRobotOneSided.h"
#include "RedRocketRobotTwoSided.h"
#include "RedRocketRobotTwoSidedNonLinear.h"
#include "RedRocketRobotOneSidedJumping.h"
#include "WhiteFlyingRobot.h"
#include "EvilBat.h"
#include "EletricBat.h"
#include "Canon.h"
#include "Door.h"
#include "Shockwave.h"
#include <unordered_set>
#include <set>
#include <map>
#include <fstream>
#include <sstream>

Grid::Grid(long _mapWidth, long _mapHeight, const char * spawnPosition, const char * staticMapObject)
{
	mapWidth = _mapWidth;
	mapHeight = _mapHeight;

	gridWidth = mapWidth / cellSizeWidth + 1;
	gridHeight = mapHeight / cellSizeHeight + 1;

	//Khởi tạo danh sách các cells.
	cells = new std::list<Object*>*[gridHeight]; //Khởi tạo theo chiều dọc trước.
	for (int i = 0; i < gridHeight; ++i)
	{
		cells[i] = new std::list<Object*>[gridWidth];
	}

	//Khởi tạo danh sách các cells chứa static objects.
	cellsOfStaticObjects = new std::list<MapStaticObject*>* [gridHeight];
	for (int i = 0; i < gridHeight; ++i)
	{
		cellsOfStaticObjects[i] = new std::list<MapStaticObject*>[gridWidth];
	}

	//Khởi tạo danh sách một mảng 2 chiều.
	objectIDPerPosition = new int *[mapHeight];
	for (int i = 0; i < mapHeight; ++i)
		objectIDPerPosition[i] = new int[mapWidth];

	//Khởi tạo danh sách một mảng 2 chiều.
	objectSpecialIDPerPosition = new int *[mapHeight];
	for (int i = 0; i < mapHeight; ++i)
		objectSpecialIDPerPosition[i] = new int[mapWidth];

	//Xoá sạch mảng (reset lại về -1).
	for (int i = 0; i < mapHeight; ++i)
	{
		for (int j = 0; j < mapWidth; ++j)
		{
			objectIDPerPosition[i][j] = -1;
		}
	}

	//Xoá sạch mảng (reset lại về -1).
	for (int i = 0; i < mapHeight; ++i)
	{
		for (int j = 0; j < mapWidth; ++j)
		{
			objectSpecialIDPerPosition[i][j] = -1;
		}
	}

	LoadSpawnPosition(spawnPosition);
	LoadMapObjects(staticMapObject);
}

Grid::~Grid()
{
	//Xoá danh sách các cells.
	for (int i = 0; i < gridHeight; ++i)
	{
		for (int j = 0; j < gridWidth; ++j)
		{
			for (auto object : cells[i][j])
			{
				Player* testPlayer = dynamic_cast<Player*> (object);
				Shield* testShield = dynamic_cast<Shield*> (object);
				//Chỉ xoá khi không phải là player, shield và bộ phận của player.
				if (testPlayer != nullptr && testShield != nullptr)
					delete object;
			}
			cells[i][j].clear();
		}
		delete[]cells[i];
	}
	delete cells;
	//Xoá các object tĩnh.
	//Lưu ý object tĩnh có thể trải dài trên nhiều cell. Vì vậy sẽ cần một cách để xoá không lặp.
	//Dùng set để lưu lại các object một cách không trùng lặp.
	std::set<MapStaticObject*> mapStaticObjects;
	for (int i = 0; i < gridHeight; ++i)
	{
		for (int j = 0; j < gridWidth; ++j)
		{
			for (auto object : cellsOfStaticObjects[i][j])
			{
				//Xoá hết khi object phủ trên nhiều cell cùng lúc.
				mapStaticObjects.insert(object);
			}
			cellsOfStaticObjects[i][j].clear();
		}
		delete[]cellsOfStaticObjects[i];
	}
	delete cellsOfStaticObjects;
	for (auto object : mapStaticObjects)
		delete object;

	//Xoá danh sách các object.
	for (int i = 0; i < mapHeight; ++i)
		delete[]objectIDPerPosition[i];
	delete objectIDPerPosition;

	//Xoá danh sách các object.
	for (int i = 0; i < mapHeight; ++i)
		delete[]objectSpecialIDPerPosition[i];
	delete objectSpecialIDPerPosition;
}

void Grid::StartEnemyBeatenCounter()
{
	isCountingEnemyBeaten = true;
	blueSoldierBeatenCounter = redRocketBeatenCounter = 0;
}

void Grid::StopEnemyBeatenCounter()
{
	isCountingEnemyBeaten = false;
}

void Grid::KillAllEnemyInActiveCells()
{
	//Duyệt từ dưới lên trước, sau đó duyệt từ trái qua phải.
	for (int i = bottomY; i <= topY; ++i)
	{
		int cellY = i;
		for (int j = leftX; j <= rightX; ++j)
		{
			int cellX = j;
			std::list<Object*>::iterator it = cells[i][j].begin();
			while (it != cells[i][j].end())
			{
				//Xét nếu là enemy thì mới giết nó.
				if ((*it)->tag == Tag::ENERMY || (*it)->type == Type::ENEMY || (*it)->tag == Tag::CANON)
					(*it)->KillObject();
				++it;
			}
		}
	}

}

void Grid::LoadMapObjects(const char * mapObjectFilePath)
{
	//Load tất cả các Map Object (Ground, Non-Ground,...).
	std::ifstream inFile;
	inFile.open(mapObjectFilePath);
	if (!inFile)
		PrintDebug("MapObject file not exist ! \n");
	std::string sInputString;

	std::getline(inFile, sInputString);

	std::istringstream iss(sInputString);

	int numOfObject;

	iss >> numOfObject;

	Type entityTag = Type::NONE;

	int objectID = -1;
	int objectTopLeftX = -1;
	int objectTopLeftY = -1;
	int objectWidth = -1;
	int objectHeight = -1;

	for (int i = 0; i < numOfObject; ++i)
	{
 		std::getline(inFile, sInputString);
		std::istringstream iss(sInputString);
		iss >> objectID >> objectTopLeftX >> objectTopLeftY >> objectWidth >> objectHeight;

		//Xét từng ID để xem entityTag của nó là gì.
		if (objectID == ObjectID::GROUND)
			entityTag = Type::GROUND;
		else if (objectID == ObjectID::SOLIDBOX)
			entityTag = Type::SOLIDBOX;
		else if (objectID == ObjectID::WATERRL)
			entityTag = Type::WATERRL;
		else if (objectID == ObjectID::ROPE)
			entityTag = Type::ROPE;
		else if (objectID == ObjectID::ONOFF)
			entityTag = Type::ONOFF;
		else if (objectID == ObjectID::DOOR)
			entityTag = Type::DOOR;
		else if (objectID == ObjectID::SPIKE)
			entityTag = Type::SPIKE;

		MapStaticObject* mapObject = new MapStaticObject();
		mapObject->type = entityTag;
		mapObject->tag = Tag::STATICOBJECT;
		mapObject->height = objectHeight;
		mapObject->width = objectWidth;
		mapObject->pos.x = objectTopLeftX + (float)objectWidth / 2;
		mapObject->pos.y = objectTopLeftY - (float)objectHeight / 2;
		AddStaticMapObjects(mapObject);
	}
}

void Grid::AddStaticMapObjects(MapStaticObject * object)
{
	//Ta sẽ thêm map object trong nhiều cell, trải dài theo chiều được quy định bởi from và to.
	int cellXFrom = (object->pos.x - object->width/2) / cellSizeWidth;

	if (cellXFrom < 0)
		cellXFrom = 0;
	else if (cellXFrom * cellSizeWidth > mapWidth)
		cellXFrom = mapWidth / cellSizeWidth;

	int cellXTo = cellXFrom;
	int cellYFrom = (object->pos.y + object->height/2) / cellSizeHeight;

	if (cellYFrom < 0)
		cellYFrom = 0;
	else if (cellYFrom * cellSizeHeight > mapHeight)
		cellYFrom = mapHeight / cellSizeHeight;

	int cellYTo = cellYFrom;
	while(cellXTo*cellSizeWidth <= (object->pos.x + object->width / 2))
	{
		++cellXTo;
	} 

	while (cellYTo * cellSizeHeight <= (object->pos.y + object->height / 2))
	{
		++cellYTo;
	} 

	//Chặn không cho object add cell ra ngoài.
	if (cellXTo > gridWidth)
		cellXTo = gridWidth;
	if (cellYTo > gridHeight)
		cellYTo = gridHeight;

	//Thêm object trên nhiều cell.
	for (int i = cellYFrom; i < cellYTo; ++i)
	{
		for (int j = cellXFrom; j < cellXTo; ++j)
		{
			cellsOfStaticObjects[i][j].push_front(object);
		}
	}
}


void Grid::LoadSpawnPosition(const char * spawnInfoFilePath)
{
	//Load tất cả các Dynamic Object (các object tương tác được).
	std::ifstream inFile;
	inFile.open(spawnInfoFilePath);
	if (!inFile)
		PrintDebug("Spawn Info File Path not found !");
	std::string sInputString;

	int objectID = -1;
	int objectTopLeftX = -1;
	int objectTopLeftY = -1;
	int objectWidth = -1;
	int objectHeight = -1;
	int objectDirection = -1;
	int objectSpecialID = -1;

	int numOfObject;

	std::getline(inFile, sInputString);
	std::istringstream iss(sInputString);
	iss >> numOfObject;

	for (int i = 0; i < numOfObject; ++i)
	{
		Object* object = nullptr;
		std::getline(inFile, sInputString);
		std::istringstream iss(sInputString);
		iss >> objectID >> objectTopLeftX >> objectTopLeftY >> objectWidth >> objectHeight >>objectDirection >> objectSpecialID;

		int midX = objectTopLeftX + objectWidth / 2;
		int midY = objectTopLeftY - objectHeight / 2;
		//Xét từng ID để xem nó là loại object động gì.
		objectIDPerPosition[midY][midX] = objectID;
		objectSpecialIDPerPosition[midY][midX] = objectSpecialID;
		//Add object vô grid nào.
		//Chỉ add nếu như là Container thôi.
		//Còn lại để tự Spawn.
		if (objectID == ObjectID::ITEMLOOTER)
			object = new Container((ItemType)objectSpecialID);
		else if (objectID == ObjectID::ITEMLOOTERMAP2)
			object = new Container((ItemType)objectSpecialID, true);
		else if (objectID == ObjectID::DISAPPEARING_PLATFORM)
			object = new DisappearingPlatform(midX, midY);
		else if (objectID == ObjectID::MOVING_PLATFORM)
		{
			switch (objectSpecialID)
			{
			case MovingPlatformType::CIRCLE:
				object = new MovingPlatformCircle(midX, midY);
				break;
			case MovingPlatformType::CROSSED:
				object = new MovingPlatformCrossed(midX, midY);
				break;
			case MovingPlatformType::HORIZONTAL:
				object = new MovingPlatformHorizontal(midX, midY);
				break;
			}
		}
		else if (objectID == ObjectID::DOOR)
		{
			object = new Door(midX, midY);
		}

		if (object == nullptr)
			continue;

		object->pos.x = midX;
		object->pos.y = midY;
		AddObjectAndIncreaseCounter(object);
	}
}

void Grid::KillAndDelAllObjectsInCell(int cellX, int cellY)
{
	std::list<Object*>& curList = cells[cellY][cellX];
	//Xoá tất cả các object hiện có trong cell này.
	//Đầu tiên ta phải vô hiệu hoá object.
	//Thực chất vô hiệu hoá object là việc xét xem object có được xoá khỏi grid hay không.

	//Nếu sau khi set Deactivated mà object vẫn còn Activated.
	//Chứng tỏ object còn đang hoạt động nên không delete.
	for (auto object : curList)
		object->DeactivateObjectInGrid();
	auto it = curList.begin();
	while(it != curList.end())
	{
		//Nếu object hiện tại vẫn đang được kích hoạt thì ta bỏ qua.
		if ((*it)->GetActivatedInGridStatus())
		{
			++it;
			continue;
		}
		DeleteObjectAndDecreaseCounter(*it);
		curList.erase(it++);
	}
}

void Grid::SpawnAllObjectsInCell(int cellX, int cellY)
{
	int fromX = cellX * cellSizeWidth;
	int toX = (cellX + 1)* cellSizeWidth;
	int fromY = cellY * cellSizeHeight;
	int toY = (cellY + 1) * cellSizeHeight;

	//Quét từ dưới lên, từ trái qua phải.
	for (int i = fromY; i < toY; ++i)
	{
		for (int j = fromX; j < toX; ++j)
		{
			//Nếu tại vị trí này không có object nào hết thì ta bỏ qua.
			if (objectIDPerPosition[i][j] < 0)
				continue;

			//Spawn (tạo mới) object dựa vào ID đã load sẵn.
			Object* newObject = nullptr;
			if (objectIDPerPosition[i][j] == ObjectID::ITEMLOOTER)
			{
				//ItemLooter không được sinh lần 2.
				//newObject = new Container(); //Tạo mới object dựa vào ID.
			}
			else if (objectIDPerPosition[i][j] == ObjectID::ITEMLOOTERMAP2)
			{
				//ItemLooter không được sinh lần 2.
				//newObject = new Container(); //Tạo mới object dựa vào ID.
			}
			else if (objectIDPerPosition[i][j] == ObjectID::BLUESOLDIER)
			{
				newObject = new Solder((RunType)objectSpecialIDPerPosition[i][j],j,i);
			}

			else if (objectIDPerPosition[i][j] == ObjectID::REDROCKET)
			{
				switch (objectSpecialIDPerPosition[i][j])
				{
				case RedRocketRobotType::ONESIDED:
					newObject = new RedRocketRobotOneSided(j, i);
					break;
				case RedRocketRobotType::TWOSIDED:
					newObject = new RedRocketRobotTwoSided(j, i);
					break;
				case RedRocketRobotType::TWOSIDEDNONLINEAR:
					newObject = new RedRocketRobotTwoSidedNonLinear(j, i);
					break;
				case RedRocketRobotType::ONESIDEDJUMPING:
					newObject = new RedRocketRobotOneSidedJumping(j, i);
				}
			}
			else if(objectIDPerPosition[i][j] == ObjectID::GREENSOLDIER)
			{
				newObject = new GreenSolder((RunType)objectSpecialIDPerPosition[i][j], j, i);
			}
			else if (objectIDPerPosition[i][j] == ObjectID::WHITEFLYINGROBOT)
			{
				newObject = new WhiteFlyingRobot(j, i);
			}
			else if (objectIDPerPosition[i][j] == ObjectID::CANNON)
			{
				switch (objectSpecialIDPerPosition[i][j])
				{
				case 0:
					newObject = new Canon(Canon::RotateDirection::Top, D3DXVECTOR2(j, i));
					break;
				case 90:
					newObject = new Canon(Canon::RotateDirection::Right, D3DXVECTOR2(j, i));
					break;
				case 180:
					newObject = new Canon(Canon::RotateDirection::Bottom, D3DXVECTOR2(j, i));
					break;
				case 270:
					newObject = new Canon(Canon::RotateDirection::Left, D3DXVECTOR2(j, i));
					break;
				}
			}
			else if (objectIDPerPosition[i][j] == ObjectID::EVIL_BAT)
			{
				newObject = new EvilBat(D3DXVECTOR2(j, i));
			}
			else if (objectIDPerPosition[i][j] == ObjectID::ELECTRIC_BAT)
			{
				newObject = new EletricBat(D3DXVECTOR2(j, i));
			}
			else if (objectIDPerPosition[i][j] == ObjectID::WAVE)
				newObject = new Shockwave(j, i);
			if (newObject == nullptr)
				continue;
			newObject->pos.x = j;
			newObject->pos.y = i;
			//Sau đó thêm nó vào GRID luôn.
			if (!AddObjectAndIncreaseCounter(newObject))
				delete newObject;
		}
	}
}

void Grid::Add(Object* objectToAdd)
{
	int cellX = objectToAdd->pos.x / cellSizeWidth;
	int cellY = objectToAdd->pos.y / cellSizeHeight;

	if (cellX < 0)
		cellX = 0;
	else if (cellX * cellSizeWidth > mapWidth)
		cellX = mapWidth / cellSizeWidth;
	
	if (cellY < 0)
		cellY = 0;
	else if (cellY * cellSizeHeight > mapHeight)
		cellY = mapHeight / cellSizeHeight;

	//Sắp xếp vị trí để shield luôn luôn hiển thị trước mọi thứ khác.
	cells[cellY][cellX].push_front(objectToAdd);
}

void Grid::ActivateCells()
{
	RECT cameraRECT = Camera::getCameraInstance()->getCameraRECT();

	//Ta cần xem độ dời của activated grid là bao nhiêu để xét Respawn.

	int nextTopY;
	int nextBottomY;
	int nextRightX;
	int nextLeftX;

	nextTopY = cameraRECT.top / cellSizeHeight + 1;
	if (nextTopY >= gridHeight)
		nextTopY = gridHeight - 1;

	nextBottomY = cameraRECT.bottom / cellSizeHeight - 1;
	if (nextBottomY < 0)
		nextBottomY = 0;

	nextLeftX = cameraRECT.left / cellSizeWidth - 1;
	if (nextLeftX < 0)
		nextLeftX = 0;

	nextRightX = cameraRECT.right / cellSizeWidth + 1;
	if (nextRightX >= gridWidth)
		nextRightX = gridWidth - 1;

	//Xét respawn.
	//Xét respawn bên trái.
	for (int i = nextLeftX; i < leftX; ++i)
	{
		//Xét từ dưới lên theo khoảng dời.
		for (int j = nextBottomY; j < nextTopY; ++j)
		{
			//Respawn tất cả các object trong cell này.
			SpawnAllObjectsInCell(i, j);
		}
	}
	//Xét respawn bên phải.
	for (int i = rightX; i < nextRightX; ++i)
	{
		//Xét từ dưới lên theo khoảng dời.
		for (int j = nextBottomY; j < nextTopY; ++j)
		{
			//Respawn tất cả các object trong cell này.
			SpawnAllObjectsInCell(i, j);
		}
	}
	//Xét respawn bên trên.
	for (int i = topY; i < nextTopY; ++i)
	{
		for (int j = nextLeftX; j < nextRightX; ++j)
		{
			//Respawn tất cả các object trong cell này.
			SpawnAllObjectsInCell(j, i);
		}
	}

	//Xét respawn bên dưới.
	for (int i = nextBottomY; i < bottomY; ++i)
	{
		for (int j = nextLeftX; j < nextRightX; ++j)
		{
			//Respawn tất cả các object trong cell này.
			SpawnAllObjectsInCell(j, i);
		}
	}

	//Xét việc kill object bên trái.
	for (int i = leftX; i < nextLeftX; ++i)
	{
		for (int j = bottomY; j < topY; ++j)
		{
			KillAndDelAllObjectsInCell(i, j);
		}
	}

	//Xét việc kill object bên phải.
	for (int i = nextRightX; i < rightX; ++i)
	{
		for (int j = bottomY; j < topY; ++j)
		{
			KillAndDelAllObjectsInCell(i, j);
		}
	}
	
	//Xét việc kill object bên trên.
	for (int i = nextTopY; i < topY; ++i)
	{
		for (int j = leftX; j < rightX; ++j)
		{
			KillAndDelAllObjectsInCell(j, i);
		}
	}

	//Xét việc kill object bên dưới.
	for (int i = bottomY; i < nextBottomY; ++i)
	{
		for (int j = leftX; j < rightX; ++j)
		{
			KillAndDelAllObjectsInCell(j, i);
		}
	}

	//Gán lại các giá trị Active Zone theo giá trị mới.
	topY = nextTopY;
	bottomY = nextBottomY;
	leftX = nextLeftX;
	rightX = nextRightX;
}

void Grid::CollisionProcess()
{
	//Đầu tiên là ta xét collision với các object động với nhau.
	//Duyệt các cell.
	for (int i = bottomY; i <= topY; ++i)
	{
		for (int j = leftX; j <= rightX; ++j)
		{
			//Xét va chạm như trên trang web.

			//Chính mình.
			CollisionProcessCellToCell(j, i, j, i);

			//Cell trên đầu.
			if (i + 1 <= topY)
				CollisionProcessCellToCell(j, i, j, i + 1);
			//Cell góc trên trái.
			if (i + 1 <= topY && j - 1 >= 0)
				CollisionProcessCellToCell(j, i, j - 1, i + 1);
			//Cell góc bên trái.
			if (j - 1 >= 0)
				CollisionProcessCellToCell(j, i, j - 1, i);
			//Cell góc dưới trái.
			if (i - 1 >= 0 && j - 1 >= 0)
				CollisionProcessCellToCell(j, i, j - 1, i - 1);
		}
	}
}

void Grid::CollisionProcessOfDynamicObjects(Object* obj1, Object* obj2)
{
	if (obj1 == obj2)
		return;
	BoundingBox obj1BoundingBox = obj1->getBoundingBox();
	BoundingBox obj2BoundingBox = obj2->getBoundingBox();

	collisionOut obj1CollisionSide = Collision::getInstance()->SweptAABB(obj1BoundingBox, obj2BoundingBox);
	collisionOut obj2CollisionSide = Collision::getInstance()->GetOppositeSide(obj1CollisionSide);
	//Nếu có va chạm Swept xảy ra.
	if (obj1CollisionSide.side != CollisionSide::none)
	{
		//Xử lý va chạm giữa 2 object với nhau.
		obj1->OnCollision(obj2, &obj1CollisionSide);
		obj2->OnCollision(obj1, &obj2CollisionSide);
	}
	else {
		CollisionSide obj1CollisionSide;
		if (Collision::getInstance()->IsCollide(obj1BoundingBox, obj2BoundingBox, &obj1CollisionSide))
		{
			collisionOut obj1CollisionOut;
			obj1CollisionOut.collisionTime = 0;
			obj1CollisionOut.side = obj1CollisionSide;
			collisionOut obj2CollisionOut = Collision::getInstance()->GetOppositeSide(obj1CollisionOut);
			obj1->OnRectCollided(obj2, obj1CollisionSide);
			obj2->OnRectCollided(obj1, obj2CollisionOut.side);
		}
		else
		{
			obj1->OnNotCollision(obj2);
			obj2->OnNotCollision(obj1);
		}
	}
}

bool Grid::CollisionProcessOfStaticObject(MapStaticObject* staticObject, Object* object)
{
	BoundingBox staticObjBoundingBox = staticObject->getBoundingBox();
	BoundingBox objBoundingBox = object->getBoundingBox();
	collisionOut colOut = Collision::getInstance()->SweptAABB(objBoundingBox, staticObjBoundingBox);
	//Nếu có va chạm Swept xảy ra.
	if (colOut.side != CollisionSide::none)
	{
		//Xử lý va chạm giữa 2 object với nhau.
		object->OnCollision(staticObject, &colOut);
		return true;
	}
	else {
		CollisionSide side;
		if (Collision::getInstance()->IsCollide(objBoundingBox, staticObjBoundingBox, &side))
		{
			if (object->tag == Tag::PLAYER && staticObject->type == Type::GROUND) {
				if (((Player*)object)->GetOnAirState() == Player::OnAir::Falling) {
					int a = 1;
				}
			}
			staticObject->OnRectCollided(object, side);
			bool collided = object->OnRectCollided(staticObject, side);
			return collided;
		}
		else
		{
			staticObject->OnNotCollision(object);
			object->OnNotCollision(staticObject);
		}
		return false;
	}
	
}

void Grid::CollisionProcessCellToCell(int firstCellX, int firstCellY, int secondCellX, int secondCellY)
{
	std::list<Object*> &firstCell = cells[firstCellY][firstCellX];
	std::list<Object*> &secondCell = cells[secondCellY][secondCellX];

	std::list<MapStaticObject*> &firstStaticCell = cellsOfStaticObjects[firstCellY][firstCellX];
	std::list<MapStaticObject*> &secondStaticCell = cellsOfStaticObjects[secondCellY][secondCellX];

	//Duyệt qua các phần tử của cell đầu tiên.
	//Xét va chạm với object static.
	std::unordered_set<Object*> isChecked;
	for (auto object : firstCell)
	{
		//Nếu object không được activated thì ta không xét va chạm luôn.
		if (!object->GetActivatedInGridStatus())
			continue;
		if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
			continue;
		for (auto staticObject : firstStaticCell)
		{
			if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
				break;
			if (CollisionProcessOfStaticObject(staticObject, object))
				isChecked.insert(object);
		}
		for (auto staticObject : secondStaticCell)
		{
			if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
				break;
			if (CollisionProcessOfStaticObject(staticObject, object))
				isChecked.insert(object);
		}
	}
	for (auto object : secondCell)
	{
		//Nếu object không được activated thì ta không xét va chạm luôn.
		if (!object->GetActivatedInGridStatus())
			continue;
		if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
			continue;
		for (auto staticObject : firstStaticCell)
		{
			if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
				break;
			if (CollisionProcessOfStaticObject(staticObject, object))
				isChecked.insert(object);
		}
		for (auto staticObject : secondStaticCell)
		{
			if (isChecked.find(object) != isChecked.end()) //Đã kiểm tra trước đó.
				break;
			if (CollisionProcessOfStaticObject(staticObject, object))
				isChecked.insert(object);
		}
	}

	//Xét va chạm object động.
	for (auto firstObj : firstCell)
	{
		//Nếu object không được activated thì ta không xét va chạm luôn.
		if (!firstObj->GetActivatedInGridStatus())
			continue;
		for (auto secondObj : secondCell)
		{
			//Nếu object không được activated thì ta không xét va chạm luôn.
			if (!secondObj->GetActivatedInGridStatus())
				continue;
			//Nếu object không xét được va chạm thì không xét va chạm luôn.
			if (!firstObj->IsCollidable() || !secondObj->IsCollidable())
			{
				if (firstObj->tag != Tag::STATICOBJECT && firstObj->tag != Tag::ITEM && secondObj->tag != Tag::STATICOBJECT && secondObj->tag != Tag::ITEM)
				continue;
			}
			//Xét việc va chạm 2 dynamic object với nhau.
			CollisionProcessOfDynamicObjects(firstObj, secondObj);
		}
	}
}

void Grid::UpdateActivatedCells(double dt)
{
	//Duyệt từ dưới lên trước, sau đó duyệt từ trái qua phải.
	for (int i = bottomY; i <= topY; ++i)
	{
		int cellY = i;
		for (int j = leftX; j <= rightX; ++j)
		{
			int cellX = j;
			std::list<Object*>::iterator it = cells[i][j].begin();
			while (it != cells[i][j].end())
			{
				//Duyệt hết các phần tử có trong cells này.
				//Kiểm tra xem có sự thay đổi cells của các object hay không.
				//Lưu lại thông tin của object hiện tại.

				//Nếu object đang không được activated thì mình bỏ qua không update và xoá khỏi Grid luôn.
				if (!(*it)->GetActivatedInGridStatus())
				{
					//Xét có cần đếm số enemy đã chết không.
					if (isCountingEnemyBeaten)
					{
						if ((*it)->tag == Tag::BLUESOLDIER || (*it)->tag == Tag::GREENSOLDIER)
							++blueSoldierBeatenCounter;
						else if ((*it)->tag == Tag::REDROCKERTER || (*it)->tag == Tag::WHITEROCKERTER)
							++redRocketBeatenCounter;
					}
					DeleteObjectAndDecreaseCounter(*it);
					cells[i][j].erase(it++);
					continue;
				}
				(*it)->Update(dt);
				it = MoveObjectAndIncrementIterator(cellX, cellY, it, (*it)); //Lúc này là đã di chuyển luôn iterator rồi.
			}
		}
	}
	
}

std::list<Object*>::iterator Grid::MoveObjectAndIncrementIterator(int cellX, int cellY, std::list<Object*>::iterator it, Object* object)
{
	//Nếu là static object thì không có việc move.
	if (!object->IsMovableInGrid())
		return (++it);

	//Kiểm tra xem object sau khi di chuyển còn thuộc cell hiện tại hay không.
	int nextCellX = object->pos.x / cellSizeWidth;
	int nextCellY = object->pos.y / cellSizeHeight;
	if (cellX == nextCellX && cellY == nextCellY)
		return (++it);

	//Thực hiện việc di chuyển qua cell khác.

	//Xoá object ở cell cũ.
	if (it != cells[cellY][cellX].end())
		cells[cellY][cellX].erase(it++);
	else
		cells[cellY][cellX].erase(it--);

	//Kiểm tra nếu object còn nằm trong active zone thì mới thêm lại vào Grid.
	if (nextCellY >= bottomY && nextCellY <= topY && nextCellX >= leftX && nextCellX <= rightX)
	{
		//Thêm object vào cell mới.
		Add(object);
	}
	
	//Nếu object không nằm trong Active Zone thì ta delete nếu không còn được active.
	else
	{
		object->DeactivateObjectInGrid();
		//Exception : Nếu object hiện là Shield thì luôn thêm lại vào grid.
		if (object->tag == Tag::SHIELD)
		{
			//Xét xem nó có ra ngoài hem. Có thì chỉnh lại toạ độ để nó vào trong Active Zone.
			if (nextCellY > topY)
				nextCellY = topY;
			else if (nextCellY < bottomY)
				nextCellY = bottomY;

			//Xét xem nó có ra ngoài hem. Có thì chỉnh lại toạ độ để nó vào trong Active Zone.
			if (nextCellX < leftX)
				nextCellX = leftX;
			else if (nextCellX > rightX)
				nextCellX = rightX;
			cells[nextCellY][nextCellX].push_front(object);
		}
		//Nếu không nằm trong Active Zone mà Object không thể bị Deactivate, ta thêm vào Grid.
		else if (object->GetActivatedInGridStatus())
			Add(object);
		else
		{
			DeleteObjectAndDecreaseCounter(object);
		}
	}
	return it;
}

void Grid::RenderActivatedCells()
{
	auto cmp = [](Object* a, Object* b) { return (a->RenderWeight() < b->RenderWeight()); };
	std::multiset<Object*, decltype(cmp)> orderOfRenders(cmp);

	//Vẽ tất cả các cells được activated.
	for (int i = bottomY; i <= topY; ++i)
	{
		for (int j = leftX; j <= rightX; ++j)
		{
			//Vẽ tất cả các object có trong cell.
			for (auto object : cells[i][j])
			{
				//Nếu object không được activated thì ta không thực hiện render.
				if (!object->GetActivatedInGridStatus())
					continue;
				//Thêm object vào set chuẩn bị vẽ.
				orderOfRenders.insert(object);
				
			}
			////Vẽ debug các object tĩnh.
			//for (auto object : cellsOfStaticObjects[i][j])
			//{
			//	//DrawDebug::DrawBoundingBox(object->getBoundingBox(), Tag::TESTMAPOBJECTBLUE);
			//}
		}
	}
	////Vẽ theo thứ tự.
	for (auto object : orderOfRenders)
	{
		//DrawDebug::DrawBoundingBox(object->getBoundingBox(), Tag::TESTMAPOBJECTRED);
		object->Render();
		
	}
	//DrawDebugObject();
}

void Grid::DeleteObjectAndDecreaseCounter(Object * object)
{
	if (object->tag == Tag::ITEM)
		--currentItemNumber;
	else if ((object->tag != Tag::CANON)&&(object->tag == Tag::ENERMY || object->type == Type::ENEMY))
		--currentEnemyNumber;
	delete object;
}

bool Grid::AddObjectAndIncreaseCounter(Object * object)
{
	if (object->tag == Tag::ITEM)
	{
		if (currentItemNumber >= maxItemAtOnce)
			return false;
		++currentItemNumber;
	}
		
	else if ((object->tag != Tag::CANON)&&(object->tag == Tag::ENERMY || object->type == Type::ENEMY))
	{
		if (currentEnemyNumber >= maxEnemyAtOnce)
			return false;
		++currentEnemyNumber;
	}
	Add(object);
	return true;
}

void Grid::DrawDebugObject()
{

}

