﻿#pragma once
#include <list>
#include "Object.h"
#include "MapStaticObject.h"
#include "Collision.h"
#include "Camera.h"
#include "Player.h"
#include "Container.h"
#include "Debug.h"
#include "DrawDebug.h"

//Phải gọi hàm ActivateCells() trước khi làm bất kỳ thứ gì khác.
class Grid // lam singleton
{
	const int cellSize = 80;

	//Danh sách các object được tổ chức dưới dạng 2 chiều (từng cell) và trong các cell chứa các Unit.
	std::list<Object*> ** cells;
	
	//Danh sách các loại object sẽ được sinh ra tại vị trí từng cell.
	int ** objectIDPerPosition;

	long gridWidth;
	long gridHeight;

	long mapWidth;
	long mapHeight;

	//Vùng mà grid được activated.
	int topY;
	int bottomY;
	int leftX;
	int rightX;

public:
	Grid(long mapWidth, long mapHeight, const char * spawnPosition, const char * staticMapObject);
	~Grid();

	void Add(Object *); //Thêm object vào grid dựa vào toạ độ của object.
	//Đây là hàm thêm đặc biệt, nó sẽ trải static object ra trên nhiều cell thay vì chỉ 1 cell như hàm trên.
	void AddStaticMapObjects(Object *); 

	void ActivateCells(); //Activate những vùng sẽ được xử lý bởi Grid (vùng được khoanh bởi Camera).
	//Nhớ kiểm tra va chạm trước khi Update.
	void CollisionProcess();
	void UpdateActivatedCells(double dt);
	void RenderActivatedCells();
private:
	//Hàm dùng để di chuyển một object từ cell này sang cell khác.
	std::list<Object*>::iterator MoveObjectAndIncrementIterator(int currentCellX, int currentCellY, std::list<Object*>::iterator, Object*);
	void CollisionProcessCellToCell(int firstCellX, int firstCellY, int secondCellX, int secondCellY);
	void LoadSpawnPosition(const char *);
	void LoadMapObjects(const char *);
	void SpawnAllObjectsInCell(int cellX, int cellY);
	void KillAndDelAllObjectsInCell(int cellX, int cellY);
	void DrawDebugObject();
};