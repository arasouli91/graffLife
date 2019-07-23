#pragma once
#include "ECS.h"
#include "SDL.h"
#include "TextureManager.h"

using namespace std;

class TileComponent : public Component
{
public:
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;
	TileComponent() = default;
	TileComponent(int srcX, int srcY, int w, int h, int xpos, int ypos, const char* path)
	{
		texture = TextureManager::LoadTexture(path);
		srcRect.x = srcX;
		srcRect.y = srcY;
		destRect.x = xpos; destRect.y = ypos;
		destRect.w = w; destRect.h = h;
	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect);
	}

	~TileComponent()
	{
		SDL_DestroyTexture(texture);
	}


};