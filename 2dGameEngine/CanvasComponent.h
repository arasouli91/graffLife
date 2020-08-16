#pragma once
#include "Components.h"
#include <SDL2/SDL.h>
#include "TextureManager.h"
#include <string>
#include <vector>
#include <list>
#include <deque>
#include "Game.h"
#include <fstream>
#include <iostream>
#include <exception>
//using std::cout; using std::endl;
class CanvasComponent : public Component
{								
private:
	TransformComponent *transform;
	SDL_Rect srcRect, destRect;
	Vector2d position;
	bool initialStrokes = 0;
public:
	SDL_Texture *rCanvas;
	std::deque<SDL_Texture*> canvas;


	CanvasComponent() = default;

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		rCanvas = nullptr;

		destRect.x = position.x = transform->pos.x;
		destRect.y = position.y = transform->pos.y;

		srcRect.x = srcRect.y = 0;
		srcRect.w = transform->width;	srcRect.h = transform->height; 	
		destRect.w = static_cast<int>(transform->width * transform->scaleW);
		destRect.h = static_cast<int>(transform->height * transform->scaleH);
	}

	void update() override
	{
		destRect.x = position.x - Game::camera.x;
		destRect.y = position.y - Game::camera.y;
	}

	void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {
		SDL_Texture* target = SDL_GetRenderTarget(renderer);
		SDL_SetRenderTarget(renderer, texture);
		int width, height;
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);
		SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
		SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
		IMG_SavePNG(surface, file_name);
		SDL_FreeSurface(surface);
		SDL_SetRenderTarget(renderer, target);
	}

	void draw() override						
	{
		//cout << "draw" << endl;
		try {

			if (!canvas.empty())	//Draw back, because it is the last in canvas

				///////////////saving texture to file then reading/loading it////////////
			{
				//cout << "!canvas.empty()" << endl; cout << endl;
				auto texture = canvas.back();
				//cout << 1 << endl; cout << endl;

				// save texture as image to imgfile
				save_texture("imgfile", Game::renderer, texture);
				//cout << 12 << endl;

				// Read image from file
				char buffer[999999999];
				//cout << 13 << endl;

				std::ifstream infile;
				infile.open("imgfile", std::ios::binary);
				//cout << 14 << endl;


				//get length of file
				infile.seekg(0, std::ios::end);
				size_t length = infile.tellg();
				infile.seekg(0, std::ios::beg);

				//cout << 15 << endl;
				//don't overflow the buffer!
				if (length > sizeof(buffer))
				{
					length = sizeof(buffer);
				}

				//read file
				infile.read(buffer, length);
				//cout << 16 << endl;

				infile.close();
				//cout << 17 << endl;

				// load image
				auto opsStruct = SDL_RWFromMem(buffer, sizeof(length));/////////////sizeof buffer might be wrong?
				//cout << 18 << endl;

				// load surface & create texture
				SDL_Surface*  surf = IMG_Load_RW(opsStruct, 1);
				if (!surf) {
					printf("IMG_Load_RW: %s\n", IMG_GetError());
					// handle error
				}
				//cout << 19 << endl;
				SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, surf);
				//cout << 110 << endl;
				SDL_FreeSurface(surf);
				//cout << 111 << endl;

				// draw
				TextureManager::Draw(tex, srcRect, destRect);
				//cout << 112 << endl;

			}
			else if (rCanvas)
				TextureManager::Draw(rCanvas, srcRect, destRect);
		}
		catch (std::exception ex)
		{

		}
	}

	//Creates new canvas and sets it as render target
	void newCanvas()
	{
		//cout << "newCanvas()" << endl;
		SDL_Texture* t = SDL_CreateTexture(Game::renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, transform->width, transform->height);
		SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);	//New canvas has blending on
		SDL_SetRenderTarget(Game::renderer, t);
		//Copy over last in canvas to new canvas
		if (!canvas.empty()) {
			SDL_SetTextureBlendMode(canvas.back(), SDL_BLENDMODE_NONE);		//Old canvas has blending none. This ensures transparency consistency.
			SDL_RenderCopy(Game::renderer, canvas.back(),NULL,NULL);	//Copying last in canvas to new canvas
			SDL_SetTextureBlendMode(canvas.back(), SDL_BLENDMODE_BLEND);
		}
		else {
			SDL_SetTextureBlendMode(rCanvas, SDL_BLENDMODE_NONE);
			SDL_RenderCopy(Game::renderer, rCanvas, NULL, NULL);		//Copying reserve canvas to new canvas
		}
		canvas.push_back(t);
		if (canvas.size() > 5) {	//Replace reserve canvas with first in canvas.
			SDL_DestroyTexture(rCanvas);
			rCanvas = canvas.front();
			canvas.pop_front();
			initialStrokes = 1;
		}
	}

	void undo()
	{
		if ( ( !initialStrokes && !canvas.empty() )|| canvas.size()>1) {
			SDL_DestroyTexture(canvas.back());
			canvas.pop_back();
		}
	}

	~CanvasComponent()
	{
		while (!canvas.empty()) {
			SDL_DestroyTexture(canvas.back());
			canvas.pop_back();
		}
		if(rCanvas)
			SDL_DestroyTexture(rCanvas);
	}
};