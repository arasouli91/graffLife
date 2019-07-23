#include "stdafx.h"
#include "Map.h"
#include "TextureManager.h"
#include <fstream>


Map::Map()
{

}


void Map::LoadMap(std::string path)
{
	//std::fstream mapFile;
	//mapFile.open(path);
	//int srcX, srcY;
	Game::AddTile(0, -60, 1469, 684, "assets/tunnelWall.png");
	Game::AddTile(1469, -60, 1872, 200, "assets/nightSky.png");
	Game::AddTile(1469, 92, 624, 532, "assets/freewayWall.png");
	Game::AddTile(2093, 92, 624, 532, "assets/freewayWall.png");
	Game::AddTile(2717, 92, 624, 532, "assets/freewayWall.png");
	Game::AddTile(0, 624, 2000, 200, "assets/asphalt.png");
	Game::AddTile(2000, 624, 1341, 200, "assets/asphalt.png");
}

void Map::DrawMap()
{
 //	TextureManager::Draw(map, src, dest);
}

Map::~Map()
{
//	SDL_DestroyTexture(map);
}
