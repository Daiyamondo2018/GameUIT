#include "TileSet.h"
#include "Debug.h"

TileSet::TileSet(const char *tileSetFile, int _numOfTileType, Tag tag)
{
	TextureManager::getInstance()->addTexture(tag, tileSetFile);
	PrintDebug(tileSetFile);
	numOfTileType = _numOfTileType;

	tileSet = new Sprite*[numOfTileType];
	tileSize = TextureManager::getInstance()->getTextureHeight(tag);

	for (int i = 0; i < numOfTileType; ++i)
	{
		tileSet[i] = new Sprite(tag, 0, i*tileSize, tileSize, (i + 1)*tileSize);
	}
		
	
}

TileSet::~TileSet()
{
	for (int i = 0; i < numOfTileType; ++i)
	{
		delete tileSet[i];
	}
	delete tileSet;
}

void TileSet::DrawTile(int tileNum, D3DXVECTOR3 position)
{
	Sprite* selectedSprite = tileSet[tileNum];
	
	D3DXVECTOR2 position2(position.x, position.y);

	selectedSprite->Render(position2);
}

int TileSet::getTileSize()
{
	return tileSize;
}