#pragma once
#include "Game.h"
#ifdef __EMSCRIPTEN__
#include "emscripten/emscripten.h"
#endif
#include <string>
#include<unordered_set>

class TextureManager
{
private:
	//static std::unordered_set<std::string> pathsContained;
public:
	static SDL_Texture* LoadTexture(const char* fileName);
#ifdef __EMSCRIPTEN__
	static SDL_Texture* LoadTexture(const char* fileName, bool flag);
#endif
	static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip = SDL_FLIP_NONE);
};