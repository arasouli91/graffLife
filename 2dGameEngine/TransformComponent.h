#pragma once
#include "Components.h"
#include "Vector2d.h"

struct TransformComponent : public Component
{
	Vector2d pos;
	Vector2d gpos;		//We can probably get rid of gpos. not sure. But pos is working fine for global positioning
	Vector2d velocity;

	//Default dimensions. For all practical purposes you will pass in dimensions.

	int height = 32;			
	int width = 32;			
	float scaleW = 1;
	float scaleH = 1;

	int speed = 2;
	bool controllable = 0;
	bool isStatic = 0;

	const Uint8* keys;

	//The SpriteComponent accesses and makes use of all these values. 
	TransformComponent(float scW, float scH, float x = 0.0f, float y = 0.0f, bool cont = 0) 
		: scaleW{ scW }, scaleH{ scH }, controllable{ cont } {
		pos.x = x; pos.y = y; gpos.x = x; gpos.y = y;
	}
	TransformComponent() { pos.Zero(); gpos.Zero(); }					
	TransformComponent(float x, float y, bool cont = 0) : controllable{ cont } 
	{
		pos.x = x; pos.y = y; gpos.x = x; gpos.y = y;
	}
	TransformComponent(float x, float y, int w, int h, float sw = 1, float sh = 1, bool cont = 0, bool isstatic = 0)
		: width{ w }, height{ h }, scaleW{ sw }, scaleH{ sh }, controllable{ cont }, isStatic{ isstatic } {
		pos.x = x; pos.y = y; gpos.x = x; gpos.y = y;
	}

	void init() override
	{
		velocity.Zero();
		keys = SDL_GetKeyboardState(nullptr);
	}

	void update() override
	{
		SDL_PumpEvents();

		if (controllable) {	
			////// WHY IS THIS COUPLED BS HERE?... pretty sure it fixed a bug. Let's see if it comes back.
			///// But commenting these 2 lines allows movement on mobile
			//if (!keys[SDL_GetScancodeFromKey(SDLK_a)] && !keys[SDL_GetScancodeFromKey(SDLK_d)])
			//	velocity.x = 0;
			///// These two lines were already commented
			//else if (keys[SDL_GetScancodeFromKey(SDLK_d)])
			//	velocity.x = 1;
		}
		if (!isStatic) {
			pos.x += velocity.x * speed;
			pos.y += velocity.y * speed;
		}
	}

	void setPos(float x, float y)
	{
		pos.x = x; pos.y = y;
	}
};