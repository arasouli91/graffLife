#pragma once
#include "Game.h"
#include "ECS.h"
#include "Components.h"

class PaintController : public Component
{
private:
	bool leftButton, rightButton, canPaint = 1, leftClick = 1;
	int mouseX, mouseY, dist{ 0 }, fCount = 0;
	Uint32 lastTime = 0;
	Uint8 r{ 0 }, g{ 0 }, b{ 0 };
	KeyboardController *keyboard;
	CanvasComponent *canvas;
	TransformComponent *transform;
	SDL_Texture *texture;
	SDL_Rect srcRect, destRect;
	const Uint8 *keys = SDL_GetKeyboardState(nullptr);
public:
	PaintController() = default;
	PaintController(const char* path)
	{
		setTex(path);
	}

	void init() override
	{
		keyboard = &entity->getComponent<KeyboardController>();
		canvas = &entity->getComponent<CanvasComponent>();
		transform = &entity->getComponent<TransformComponent>();
	}


	~PaintController()
	{
		SDL_DestroyTexture(texture);
	}

	void setTex(const char* path)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void setButtonState(Uint8 button, bool state)
	{
		switch (button)
		{
		case SDL_BUTTON_LEFT:
			leftButton = state;
			break;
		case SDL_BUTTON_RIGHT:
			rightButton = state;
			break;
		}
	}

	void update() override
	{
		if (canPaint) {
			if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
				setButtonState(SDL_BUTTON_LEFT, true);
			else if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)) {
				setButtonState(SDL_BUTTON_RIGHT, true);
			}

			if ((!SDL_GetMouseState(NULL, NULL))&SDL_BUTTON(3)) {
				setButtonState(SDL_BUTTON_RIGHT, false);
			}

			if ((!SDL_GetMouseState(NULL, NULL))&SDL_BUTTON(1)) {
				if (leftButton)
				{
					leftClick = 1;	//Record the unpress action 
				}
				setButtonState(SDL_BUTTON_LEFT, false);
			}


			if (keys[SDL_GetScancodeFromKey(SDLK_r)]) {
				if ((keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) && r > 0)
					--r;
				else if (r < 255)
					++r;
			}
			else if (keys[SDL_GetScancodeFromKey(SDLK_g)]) {
				if ((keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) && g > 0)
					--g;
				else if (g < 255)
					++g;
			}
			if (keys[SDL_GetScancodeFromKey(SDLK_b)]) {
				if ((keys[SDL_GetScancodeFromKey(SDLK_DOWN)]) && b > 0)
					--b;
				else if (b < 255)
					++b;
			}
			if (keys[SDL_GetScancodeFromKey(SDLK_UP)]) {
				if (dist < 28) {
					++fCount;
					if (fCount % 4 > 1)
						++dist;
				}
			}
			else {
				dist = 0;
				fCount = 0;
			}

			if (!leftButton && keys[SDL_GetScancodeFromKey(SDLK_z)]) {
				if (SDL_GetTicks() - lastTime > 300) {
					canvas->undo();
					lastTime = SDL_GetTicks();

				}
			}

		}
	}

	void draw() override
	{
		int prevX = mouseX, prevY = mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		if (leftButton && Game::camera.y < 50)
		{
			int alpha = 92 - fCount * 2;
			if (alpha < 0)
				alpha = 3;
			SDL_SetTextureAlphaMod(texture, alpha);

			SDL_Rect rect = { mouseX - static_cast<int>(transform->pos.x) - (12 + dist) / 2 + Game::camera.x, 
						rect.y = mouseY - static_cast<int>(transform->pos.y) - (12 + dist) / 2 + Game::camera.y,
						12 + dist, 12 + dist };
			if (leftClick) {	//If mouse is pressed
				canvas->newCanvas();	//Begin painting on new canvas
				leftClick = 0;
			}
			else if (!canvas->canvas.empty())   //Mouse is held down
				SDL_SetRenderTarget(Game::renderer, canvas->canvas.back());	//Paint on latest canvas

			SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 0);
			SDL_SetTextureColorMod(texture, r, g, b);
			SDL_RenderCopy(Game::renderer, texture, NULL, &rect);

			int dX = mouseX - prevX, dY = mouseY - prevY;
			double avg = double(abs(dX) + abs(dY)) / 2;
			double posX = prevX, posY = prevY;
			double DX = dX / avg, DY = dY / avg;

			for (int i = 0; i < avg; ++i)
			{
				alpha = 92 - fCount * 2 - avg / 2;
				if (alpha < 0)
					alpha = 3;
				if (r > 80 || g > 80) {
					if (alpha > 15)
						alpha += 25;
				}
				SDL_SetTextureAlphaMod(texture, alpha);

				posX += DX; posY += DY;
				rect.x = int(posX) - transform->pos.x - (rect.w / 2) + Game::camera.x; rect.y = int(posY) - transform->pos.y - (rect.h / 2) + Game::camera.y;

				SDL_RenderCopy(Game::renderer, texture, NULL, &rect);
			}
		}
		SDL_SetRenderTarget(Game::renderer, NULL);	//Allows everything else to be rendered
	}
};




/*

#pragma once
#include "Game.h"
#include "ECS.h"
#include "Components.h"
///I want to disable painting after flare is flared out

class PaintController : public Component
{
private:
bool leftButton, rightButton, canPaint = 1, leftClick = 1;
int mouseX, mouseY, dist{ 0 }, fCount = 0;
Uint32 lastTime=0;
Uint8 r{ 0 }, g{ 0 }, b{ 0 };
KeyboardController *keyboard;
CanvasComponent *canvas;
TransformComponent *transform;
SDL_Texture *texture;
SDL_Rect srcRect, destRect;
const Uint8 *keys = SDL_GetKeyboardState(nullptr);
public:
//bool isButton() { return (rightButton || leftButton); }
PaintController() = default;
PaintController(const char* path)
{
setTex(path);
}

void init() override
{
keyboard = &entity->getComponent<KeyboardController>();
canvas = &entity->getComponent<CanvasComponent>();
transform = &entity->getComponent<TransformComponent>();
}


~PaintController()
{
SDL_DestroyTexture(texture);
}

void setTex(const char* path)
{
texture = TextureManager::LoadTexture(path);
}

void setButtonState(Uint8 button, bool state)
{
switch (button)
{
case SDL_BUTTON_LEFT:
leftButton = state;
break;
case SDL_BUTTON_RIGHT:
rightButton = state;
break;
}
}

void update() override
{
if (canPaint) {
if(SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1))
setButtonState(SDL_BUTTON_LEFT, true);
else if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)) {
setButtonState(SDL_BUTTON_RIGHT, true);
}

///This is insane, it just decided that it wont recognize the right button unpressed anymore
if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(3)) {
setButtonState(SDL_BUTTON_RIGHT, false);
}

if (!SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1)) {
if (leftButton)
{
leftClick = 1;	//Record the unpress action
}
setButtonState(SDL_BUTTON_LEFT, false);
}


if (keys[SDL_GetScancodeFromKey(SDLK_r)]) {
if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)] && r > 0)
--r;
else if ( r < 255)
++r;
}
else if (keys[SDL_GetScancodeFromKey(SDLK_g)]) {
if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)] && g > 0)
--g;
else if( g < 255)
++g;
}
if (keys[SDL_GetScancodeFromKey(SDLK_b)]) {
if (keys[SDL_GetScancodeFromKey(SDLK_DOWN)] && b > 0)
--b;
else if( b < 255)
++b;
}
if (keys[SDL_GetScancodeFromKey(SDLK_UP)]) {
if (dist < 28) {
++fCount;
if (fCount % 4 > 1)
++dist;
}
}else {
dist = 0;
fCount = 0;
}

if (!leftButton && keys[SDL_GetScancodeFromKey(SDLK_z)]) {
if (SDL_GetTicks() - lastTime > 300) {
canvas->undo();
lastTime = SDL_GetTicks();

}
}

}
}

void draw() override
{

///What if we used multiple renderings of the spray randomly scattered nearby to create a real spray effect
//if (!keyboard->isKey()) {
//if (SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(1) || SDL_GetMouseState(NULL, NULL)&SDL_BUTTON(2)) {
//std::cout << "cvsCount = " << cvsCount << std::endl;
int prevX = mouseX, prevY = mouseY;
SDL_GetMouseState(&mouseX, &mouseY);
if (leftButton && Game::camera.y <50)
{
int alpha = 92 - fCount * 2;
if (alpha < 0)
alpha = 3;
SDL_SetTextureAlphaMod(texture, alpha);
//std::cout << prevX << " " << prevY << "     " << mouseX << " " << mouseY << std::endl;

SDL_Rect rect={ mouseX - transform->pos.x - (12+dist)/2 + Game::camera.x, rect.y = mouseY - transform->pos.y - (12+dist)/2 + Game::camera.y, 12+dist, 12+dist};
//std::cout <<"RGB: "<<int(r)<<" " <<int(g) <<" "<<int(b)<< std::endl;
if (leftClick) {	//If mouse is pressed
canvas->newCanvas();	//Begin painting on new canvas
leftClick = 0;
}
else if(!canvas->canvas.empty())   //Mouse is held down
SDL_SetRenderTarget(Game::renderer, canvas->canvas.back());	//Paint on latest canvas

SDL_SetRenderDrawColor(Game::renderer, 0, 0, 0, 0);
SDL_SetTextureColorMod(texture, r, g, b);


SDL_RenderCopy(Game::renderer, texture, NULL, &rect);
//SDL_RenderPresent(Game::renderer);
int dX = mouseX - prevX, dY = mouseY - prevY;
double avg = double(abs(dX) + abs(dY)) / 2;
double posX = prevX, posY = prevY;
double DX = dX / avg, DY = dY / avg;
//std::cout << avg << " " << DX << " " << DY << std::endl;
for (int i = 0; i < avg; ++i)
{
alpha = 92 - fCount * 2 - avg/2;
if (alpha < 0)
alpha = 3;
if (r > 80 || g > 80) {
if (alpha > 15)
alpha += 25;
}
SDL_SetTextureAlphaMod(texture, alpha);

posX += DX; posY += DY;
rect.x = int(posX) - transform->pos.x - (rect.w / 2) + Game::camera.x; rect.y = int(posY) - transform->pos.y - (rect.h / 2) + Game::camera.y;

SDL_RenderCopy(Game::renderer, texture, NULL, &rect);
}
}
SDL_SetRenderTarget(Game::renderer, NULL);	//Allows everything else to be rendered
}
//}
//}


};





/*
//Initialize renderer color
SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

//Initialize PNG load
int imgFlags = IMG_INIT_PNG;
if (!(IMG_Init(imgFlags) & imgFlags))
{
printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
success = false;
}
}
}
}
return success;
}
*/

