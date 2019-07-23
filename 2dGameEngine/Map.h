#pragma once
#include "Game.h"
class Map
{
public:
	Map();
	~Map();
	void LoadMap(std::string path);
	void DrawMap();
private:
	SDL_Rect src, dest;
	//SDL_Texture* map;
};

