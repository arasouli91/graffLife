#pragma once
#include "Game.h"
#include "ECS.h"
#include "Components.h"

class KeyboardController : public Component
{
private:
	bool keydown = 0;
	const Uint8 *keys = SDL_GetKeyboardState(nullptr);
	//Entity* ent = nullptr;
public:
	//KeyboardController(&entity e) : ent{ a } {}
	TransformComponent *transform;
	SpriteComponent *sprite;
	int xMotion = 0;
	bool deviceMotion = false;
	bool isKey() { return keydown; }
	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		sprite = &entity->getComponent<SpriteComponent>();
	}
	void update() override
	{
		if (!deviceMotion && ((!keys[SDL_GetScancodeFromKey(SDLK_s)]) && (!keys[SDL_GetScancodeFromKey(SDLK_w)]))) {
			transform->velocity.y = 0;
			sprite->play("idle");
			keydown = 0;
		}
		if ( (!deviceMotion && (!keys[SDL_GetScancodeFromKey(SDLK_d)]) && (!keys[SDL_GetScancodeFromKey(SDLK_a)])) || abs(xMotion) < 2) {
			transform->velocity.x = 0;
			sprite->play("idle");
			keydown = 0;
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_s)]) {
			if (!keydown)
				sprite->play("walk");
			transform->velocity.y = 1;
			keydown = 1;
		}
		else if (keys[SDL_GetScancodeFromKey(SDLK_w)]) {
			if (!keydown)
				sprite->play("walk");
			transform->velocity.y = -1;
			keydown = 1;
		}
		if (keys[SDL_GetScancodeFromKey(SDLK_d)] || xMotion < -2) {
			if (!keydown)
				sprite->play("walk");
			sprite->spriteFlip = SDL_FLIP_HORIZONTAL;
			transform->velocity.x = 1;
			keydown = 1;
		}
		else if (keys[SDL_GetScancodeFromKey(SDLK_a)] || xMotion > 2) {
			if (!keydown)
				sprite->play("walk");
			sprite->spriteFlip = SDL_FLIP_NONE;
			transform->velocity.x = -1;
			keydown = 1;
		}

		/*if(Game::event.type==SDL_KEYDOWN){
		switch (Game::event.key.keysym.sym)
		{
		case SDLK_:
		transform->velocity.y = -1;
		break;
		case SDLK_d:
		transform->velocity.x = 1;
		break;
		case SDLK_s:
		transform->velocity.y = 1;
		break;
		case SDLK_a:
		transform->velocity.x = -1;
		default:
		break;
		}
		}*/
		if (Game::event.type == SDL_KEYUP)
		{
			switch (Game::event.key.keysym.sym)
			{
				/*case SDLK_w:
				transform->velocity.y = 0;
				break;
				case SDLK_d:
				transform->velocity.x = 0;
				sprite->setTex("Idle.png");
				break;
				case SDLK_s:
				transform->velocity.y = 0;
				break;
				case SDLK_a:
				transform->velocity.x = 0;
				sprite->setTex("Idle.png");		//And the paths need to be decoupled from this comp.
				*/
			case SDLK_ESCAPE:
				Game::isRunning = false;
			default:
				break;
			}
		}
	}
	void update_x(int x)
	{
		xMotion = x;
		deviceMotion = true;
	}
};