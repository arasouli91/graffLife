#include "stdafx.h"
#include "TextureManager.h"
#include <assert.h>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include<unordered_map>

std::unordered_map<std::string, SDL_Surface*> textureHash;

void onLoad(const char* file) {
	assert(0 != file);
}

void onFail(const char* file) {
	//assert(0 != file);
	//printf("wget error: onFail(url=%s)\n", file);
}

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
SDL_Texture* TextureManager::LoadTexture(const char* fileName)
{
	/**
	int w, h; char *data; SDL_Surface *image;

	data = emscripten_get_preloaded_image_data(fileName, &w, &h);
	if (data != NULL) {
		image = SDL_CreateRGBSurface(0, w, h, 32, 0xFF, 0xFF00, 0xFF0000, 0xFF000000);
		if (image != NULL) {
			memcpy(image->pixels, data, w * h * 4);
		}
		free(data);
		//return surf;
	}*/
	/**
	void load_image(void *image_data, int size)
	{
		SDL_RWops *a = SDL_RWFromConstMem(image_data, size);
		SDL_Surface *image = IMG_Load_RW(a, 1);

		ctx.img_tex = SDL_CreateTextureFromSurface(ctx.renderer, image);

		SDL_FreeSurface(image);
	}
	*/

#ifdef __EMSCRIPTEN__
	std::string s = "https://grafflife.000webhostapp.com/2/"; s += fileName;
	const char *URL = s.c_str();
	//emscripten_async_wget("assets", fileName, onLoad, onFail);//if we actually get the asset from heroku we get file exists error whether or not we preloaded it
	//emscripten_async_wget(URL, fileName, onLoad, onFail);
	//emscripten_wget(fileName, fileName);
#endif
	//EM_ASM_({FS.createDataFile("assets", PATH.basename(UTF8ToString($0)), 0, 1, 1, 1); }, fileName);
	auto hashedTexture = textureHash.find(fileName);
	//std::cout << "load:" << fileName << std::endl;
	SDL_Surface* image;
	if (hashedTexture != textureHash.end()) {
		image = hashedTexture->second;
	}
	else {

		image = IMG_Load(fileName);
		if (!image)
		{
			printf("IMG_Load: %s\n", IMG_GetError());
			return 0;
		}
	}
	SDL_RenderPresent(Game::renderer);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, image);
	//SDL_FreeSurface(image);

	textureHash.insert(std::make_pair(fileName, image));
	//std::cout << "load success?" << std::endl;
	return tex;
	////////////////hash the path to the texture



	/**
	int testImage(SDL_Surface* screen, const char* fileName) {
		SDL_Surface *image = IMG_Load(fileName);
		if (!image)
		{
			printf("IMG_Load: %s\n", IMG_GetError());
			return 0;
		}
		assert(image->format->BitsPerPixel == 32);
		assert(image->format->BytesPerPixel == 4);
		assert(image->pitch == 4 * image->w);
		int result = image->w;
		/////////////////////will blitting surface help???
		////////////////////will sprite component need to also hold a surface?
		SDL_BlitSurface(image, NULL, screen, NULL);

		int w, h;
		char *data = emscripten_get_preloaded_image_data(fileName, &w, &h);

		assert(data);
		assert(w == image->w);
		assert(h == image->h);

		SDL_FreeSurface(image);
		free(data);

		return result;
		*/
}

#ifdef __EMSCRIPTEN__
//This is for loading textures in emscripten's FS, instead of by URL.
//Doesn't matter what flag you put.
SDL_Texture* TextureManager::LoadTexture(const char* texture, bool flag)
{
	//std::cout << "Load " << texture<<std::endl;
		SDL_Surface* tempSurface = IMG_Load(texture);
		if (!tempSurface)
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

