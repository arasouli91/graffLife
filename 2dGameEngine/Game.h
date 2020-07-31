#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <vector>

class ColliderComponent;

class Game
{
public:
	Game(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	~Game();
	void handleEvents();
	void update();
	void render();
	bool running() { return isRunning; }

	static void AddTile(int x, int y, int w, int h, const char* path);

	static SDL_Renderer* renderer;
	static SDL_Event event;
	static std::vector<ColliderComponent*> colliders;
	//static SDL_Texture* canvas;
	static bool isRunning;
	static SDL_Rect camera;
private:
	SDL_Rect screenRect;
	int cnt = 0;
	//int w, h;
	SDL_Window *window;
	Uint32 lastEvent = 0;
};

