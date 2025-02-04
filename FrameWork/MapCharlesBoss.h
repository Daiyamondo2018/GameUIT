#pragma once
#include "TileMap.h"

class MapCharlesBoss : public TileMap
{
public:
	MapCharlesBoss(const char * imgPath, const char * txtPath, Tag tag);
	~MapCharlesBoss();
	void Draw(Camera * camera) override;
	void Update(double dt) override;
};