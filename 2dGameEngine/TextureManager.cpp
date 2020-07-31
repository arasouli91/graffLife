#include "stdafx.h"
#include "TextureManager.h"
#include <assert.h>
#include<string>
void onLoad(const char* file) {
	assert(0 != file);
}

void onFail(const char* file) {
	assert(0 != file);
	printf("wget error: onFail(url=%s)\n", file);
}

SDL_Texture* TextureManager::LoadTexture(const char* texture)
{
	//If I am going to do hashing,(to prevent reloading same img)
	//I will need to store the textures, retrieve them from hash
	//On deletion I would have to delete from hash
	//Problem is where does the texture get deleted?
	//In sprite components or tile components?
	//And there can be multiple instances of the texture
	//So this seems undoable.
	//They would probably have to stay in the hash
	//And we would keep all this in the hash as we explored the world
	//Ok solution for short term optimization,
	//Bad solution for long term big picture
	//Unless I clear the contents of the hash map

	//But the sprite comp and tile comp destroy texture
	//We would have to stop them from destroying textures themselves
	//The texture manager would essentially hold all the textures
	//as pointers held in the hash, 
	//sprite/tile comps will have ptrs to the textures

	//Only problem is in the long run, this is a big drag
	//Each time you change a map you clear everything
	//and must reload many textures that wouldn't have needed to be cleared
	
	//On the other hand having the duplicate img loaded for the tiles
	//will only require these duplicates loaded for each current map
	//and wont have to reload everything else
	//Then on map switch it will destroy the previous map
	///This will probably have to be the way

	//if(pathsContained.find(texture)!=pathsContained.end())
	
#ifdef __EMSCRIPTEN__
	std::string s = "https://grafflife.000webhostapp.com/2/"; s+=texture;
	const char *URL = s.c_str();
	emscripten_async_wget(URL, texture, onLoad, onFail);
#endif
	SDL_Surface* tempSurface = IMG_Load(texture);
	if (!texture)
	{
		printf("IMG_Load: %s\n", IMG_GetError());
		return 0;
	}
	SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
	SDL_FreeSurface(tempSurface);
	return tex;
}

#ifdef __EMSCRIPTEN__
//This is for loading textures in emscripten's FS, instead of by URL.
//Doesn't matter what flag you put.
SDL_Texture* TextureManager::LoadTexture(const char* texture, bool flag)
{
	std::cout << "Load " << texture<<std::endl;
		SDL_Surface* tempSurface = IMG_Load(texture);
		if (!texture)
		{
			printf("IMG_Load: %s\n", IMG_GetError());
			return 0;
		}
		SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
		SDL_FreeSurface(tempSurface);
		return tex;
}
#endif

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(Game::renderer, tex, &src, &dest,0,NULL,flip);  
}

