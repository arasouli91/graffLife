#include "stdafx.h"
#include "GameObject.h"
#include "TextureManager.h"

GameObject::GameObject(const char* texturesheet, int x,int y)
{
	objTexture = TextureManager::LoadTexture(texturesheet);
	xpos = x; ypos = y;
}
void GameObject::Update()
{
	xpos++; ypos++;
	srcRect.h = 542; srcRect.w = 474;
	srcRect.x = 0; srcRect.y = 0;
	
	destRect.x = xpos; destRect.y = ypos;
	destRect.w = 128; destRect.h = 128;

}
void GameObject::Render()
{
	//Draw texture
	//RenderCopy takes 4 args, the last two are: 
	//src rect: part of texture to draw, dstrect: where to draw it on the screen.
	SDL_RenderCopy(Game::renderer, objTexture, &srcRect, &destRect);
}
GameObject::~GameObject()
{
}
