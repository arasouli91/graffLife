#pragma once
#include "Components.h"
#include "SDL.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include <list>
#include <deque>

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

	void draw() override						
	{
		if(!canvas.empty())	//Draw back, because it is the last in canvas
			TextureManager::Draw(canvas.back(), srcRect, destRect);
		else if(rCanvas)
			TextureManager::Draw(rCanvas, srcRect, destRect);
	}

	//Creates new canvas and sets it as render target
	void newCanvas()
	{
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