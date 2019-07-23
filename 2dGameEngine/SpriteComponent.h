///Backwards walking still happens... does it?

#pragma once
#include "Components.h"
#include "SDL.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include "Animation.h"
#include <map>

//DBG
#include <iostream>

class SpriteComponent : public Component
{													//Maybe all this animation functionality belongs in a derived class
private:
	TransformComponent *transform=nullptr;
	SDL_Texture *texture;
	SDL_Rect srcRect, destRect;
	Vector2d position;
	bool animated = 0, isMap = 0;
	int frames = 0, speed = 100;
public:
	int animNdx = 0;
	std::map<const char*, Animation> animations;

	SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

	SpriteComponent() = default;
	SpriteComponent(const char* path, bool ismap, bool isAnimated)
		: isMap{ismap}
	{
		setTex(path);
	}
	//Animated constructor, currently tailored for walkSheet
	SpriteComponent(const char* path, bool isAnimated) : animated{isAnimated}
	{
		Animation idle = Animation(0, 1, 1);
		Animation walk = Animation(1, 10, 100);

		animations.emplace("walk", walk);
		animations.emplace("idle", idle);

		play("idle");

		
#ifdef __EMSCRIPTEN__
			texture = TextureManager::LoadTexture(path, 1);
#else
		setTex(path);
#endif // __EMSCRIPTEN__

		srcRect.w = 0;
	}

	///We can probably revert all this shit since we probably don't need shared transform component anymore?
	SpriteComponent(const char* path, TransformComponent* address, int w, int h)
		: transform{ address }	//allows shared transform comp
	{
		setTex(path);
		srcRect.w = w; srcRect.h = h;
	}

	void setTex(const char* path)
	{
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{
		if(!transform)
			transform = &entity->getComponent<TransformComponent>();

		destRect.x = position.x = transform->pos.x;
		destRect.y = position.y = transform->pos.y;

		srcRect.x = srcRect.y = 0;
		if (srcRect.w == 0) {
			srcRect.w = transform->width;  srcRect.h = transform->height;
		}
		destRect.w = static_cast<int>(transform->width * transform->scaleW);
		destRect.h = static_cast<int>(transform->height * transform->scaleH);

	}

	void update() override
	{
		if (isMap)
		{
			destRect.x = position.x - Game::camera.x;
			destRect.y = position.y - Game::camera.y;
		}
		else
		{
			destRect.x = static_cast<int>(transform->pos.x) - Game::camera.x;
			destRect.y = static_cast<int>(transform->pos.y) - Game::camera.y;
			//destRect.w = static_cast<int>(transform->width * transform->scaleW); //uncomment if need to dynamically change size
			//destRect.h = static_cast<int>(transform->height * transform->scaleH);
		}
		if (animated)
		{
			//if(transform->velocity.x==1)
				srcRect.x = (srcRect.w) * static_cast<int>((SDL_GetTicks() / speed) % frames);
			//else if (transform->velocity.x == -1)
			//	srcRect.x = (srcRect.w) * static_cast<int>((SDL_GetTicks() / speed) % frames);
		}
		srcRect.y = animNdx*transform->height;//Y pos for different sets of animations
	}

	void draw() override						
	{
		TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
	}

	void play(const char* animName)
	{
		frames = animations[animName].frames;
		speed = animations[animName].speed;
		animNdx = animations[animName].index;
	}

	~SpriteComponent()
	{
		SDL_DestroyTexture(texture);
	}
};
























/*
///If we just wait for this guy to make more videos we can have this done the right way
///We assume there are no anispritecomps that need shared transforms
class AniSpriteComponent : public SpriteComponent
{													
private:
	TransformComponent *transform, *transform2 = nullptr;
	SDL_Texture *texture;
	SDL_Rect srcRect, destRect;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	unsigned rMove, lMove;
	std::vector<std::string> paths;
public:
	AniSpriteComponent() = default;

	//constructor... call parent constructor?
	AniSpriteComponent(const char* path)
	{
		setTex(path);
	}
	//allows shared transform comp, but sprite uses passed in rect vals
	AniSpriteComponent(const char* path, TransformComponent* address) : transform{ address }	
	{
		setTex(path);
		transform2 = address;
	}

	void setTex(const char* path) 
	{
		texture = TextureManager::LoadTexture(path);
	}

	void init() override
	{
		transform = &entity->getComponent<TransformComponent>();
		TransformComponent* trans = transform;
		rMove = lMove = 0;
		//if (transform2)								//Uses transform2 if it was passed in....
			//trans = transform2;							//Actually I think we always want to just use transform1
		destRect.x = trans->pos.x;
		destRect.y = trans->pos.y;

		srcRect.w = trans->width;  srcRect.h = trans->height;
		destRect.w = static_cast<int>(trans->width * trans->scaleW); 
		destRect.h = static_cast<int>(trans->height * trans->scaleH);
	}

	void update() override
	{
		TransformComponent* trans = transform;
		if (transform2)
			trans = transform2;
		///Problem occurs, sometimes gets stuck backward
		//Count how many times it has moved in a given direction to send in the ani func
		//Once moved in opp, dir reset count
		//Hold sprite paths in an array, count corresponds to index, wrap index
		//(Pitfall: if anything else causes a change in pos, other than key press, the animation will still update according to the movement): 
		int posx = static_cast<int>(trans->pos.x), posy = static_cast<int>(trans->pos.y);
		if (trans->velocity.x<0) {								//Rightward movement occured
			++rMove;												//increment right movement frame index
			if (lMove) {
				lMove = 0;												//reset left movement frame index
				flip = SDL_FLIP_NONE;
			}
			playAni();												//Update sprite frame
			//destRect.x = posx;										//Update sprite pos.
		}
		else if (trans->velocity.x>0) {								//Leftward movement occured
			++lMove;	
			if (rMove) {
				rMove = 0;												//reset right movement frame index
				flip = SDL_FLIP_HORIZONTAL;
			}
			playAni();
			//destRect.x = posx;
		}///We have to figure out how to avoid this call
		else if ((rMove || lMove) &&trans->velocity.x == 0)		//If movement ceased
		{
			if (rMove) {	///This is essentially making idle at pos. 2 of path array, can't do much about that
				rMove = 2;
			}
			else
				lMove = 2;
			playAni();
			lMove = rMove = 0;
		}
		//destRect.y = posy;//We aren't moving this animated sprite around anymore, because player stays fixed, might want to create a different ani sprite comp for movable objects, or use a lot of logic, bcuz obviously other players and npcs will need to move around you, somehow when networked on clientside other players will have to be maybe a different type of entity or something, because they will have to move

		//Why would we have to continue to update these? Works fine.
	//destRect.w = transform->width * transform->scaleW; 
	//destRect.h = transform->height * transform->scaleH;

	}

	void draw() override
	{
		TextureManager::Draw(texture, srcRect, destRect, flip);
	}

	void loadPaths(const char* path)
	{
		paths.push_back(path);
	}

	//Each call iterates through ani sequence
	void playAni()
	{
		unsigned* nDx;
		//Add to iterator ptr,
		//Idle is at begin(paths) but if we are animating we will always be addings to begin(paths) and thus avoiding idle
		if (lMove) {
			flip = SDL_FLIP_HORIZONTAL;
			nDx = &lMove;
		}
		else
			nDx = &rMove;
		*nDx %= paths.size()-1;
		std::string s = *(std::begin(paths) + *nDx);
		//std::cout << paths.size() << " nDx : " << *nDx << " "<< s.c_str()<<std::endl;
		setTex( s.c_str() );
	}
	~AniSpriteComponent()
	{
		SDL_DestroyTexture(texture);
	}
};











*/



/*


class SpriteComponent : public Component
{													//Maybe all this animation functionality belongs in a derived class
private:
///Wait we could just have an actualy ptr to a transform comp and have the address get passed in here

TransformComponent *transform = nullptr;
TransformComponentPtr *transformPtr = nullptr;	//This allows for shared transform components
SDL_Texture *texture;
SDL_Rect srcRect, destRect;
public:
SpriteComponent() = default;
SpriteComponent(const char* path)
{
setTex(path);
}

void setTex(const char* path)
{
texture = TextureManager::LoadTexture(path);
}

void init() override
{
if (entity->hasComponent<TransformComponent>())
transform = &entity->getComponent<TransformComponent>();
else
transformPtr = &entity->getComponent<TransformComponentPtr>();
srcRect.x = srcRect.y = 0;
if(transform)
srcRect.w = transform->width;	srcRect.h = transform->height;
else
srcRect.w = transformPtr->ptr->width;	srcRect.h = transformPtr->ptr->height;
destRect.w = srcRect.w;			destRect.h = srcRect.h;
}

void update() override
{
int posx, posy;
if (transform) {	///If has trans comp, else call on ptr
posx = static_cast<int>(transform->pos.x); posy = static_cast<int>(transform->pos.y);
}
else {
posx = static_cast<int>(transformPtr->ptr->pos.x); posy = static_cast<int>(transformPtr->ptr->pos.y);
destRect.x = static_cast<int>(transform->pos.x);
destRect.y = static_cast<int>(transform->pos.y);

}

void draw() override
{
TextureManager::Draw(texture, srcRect, destRect);
}
~SpriteComponent()
{
SDL_DestroyTexture(texture);
}
};


*/