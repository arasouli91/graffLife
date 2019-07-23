/*
All the entitty creation and component adding will probably need to be taken care of in a separate function
Initially the game must load up appropriate starting information
Based on this information the appropriate entities and components will be created
Then reloading occurs when transistioning levels
*/

#include "stdafx.h"
#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "Components.h"
#include "Collision.h"
#include "emscripten/html5.h"
using std::cout; using std::endl;


//EM_BOOL motionCallback(int eventType, const EmscriptenDeviceMotionEvent *e, void *userData);

Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
//SDL_Texture* Game::canvas = nullptr;

SDL_Rect Game::camera = { 0,0,1366,640 };

const char* mapfile = "";

enum groupLabels : std::size_t
{
	groupMap, groupPlayer, groupColliders
};

std::vector<ColliderComponent*> Game::colliders;
auto& topC(manager.addEntity());
auto& botC(manager.addEntity());
auto& rightC(manager.addEntity());
auto& leftC(manager.addEntity());

auto& sky(manager.addEntity());
auto& map(manager.addEntity());

auto& player(manager.addEntity());

auto& mapGroup(manager.getGroup(groupMap));
auto& playerGroup(manager.getGroup(groupPlayer));
auto& collidersGroup(manager.getGroup(groupColliders));

bool Game::isRunning = true;

Game::Game(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	screenRect.w = width; screenRect.h = height;

	int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING & ~(SDL_INIT_HAPTIC)) == 0)
	{
		std::cout << "Subsystems Initialized.." << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			std::cout << "Window created." << std::endl;
		}
		else {			//SDL_GetError() returns any error from any SDL function.
			std::cout << "Window cannot be created: "<<SDL_GetError() << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Renderer created." << std::endl;
		}
		isRunning = true;
	}
	else {
		printf("SDL could not init! SDL_Error: %s\n", SDL_GetError());
		isRunning = false;
	}



	//The order that the components are added matters, and order entities made matter:
	//A component can't contain a member pointer to another until that other comp has been added. or perhaps can't contain a pointer to it in an function

	//We should make sure the actual sprite dimensions are very trimmed, otherwise we will have to trim in here and it only trims in one direction


	Map mapLoader;
	mapLoader.LoadMap("");

	///ANOTHER MAJOR PROBLEM:
	///WE NEED CHARACTER TO NOT PHYSICALLY MOVE, YET WE NEED TO RECORD HIS STEPS FOR GLOBAL POSITIONING
	///BUT WE NEED MAP TO MOVE
	///Seems we have to give keyboard controller and a separate transform comp to map
	///Player can have a different type of keyboard controller to handle combat and anything else, might be able to use normal events for combat if simple key presses
	///Problem is map needs to be made first, but we depend on transform comp of play
	//map_.addComponent<SpriteComponent>("map.png", &player.getComponent<TransformComponent>(), 1600, 640);

	///This should just be called canvas
	map.addComponent<TransformComponent>(0, 93, 3341, 372, 1, 1, 0);
	map.addComponent<CanvasComponent>();
	map.addComponent<PaintController>("assets/spray.png");
	//map.addComponent<ColliderComponent>("map");
	map.addGroup(groupMap);

	
	player.addComponent<TransformComponent>(0, 0, 73, 206, 1, 1, 1);
	player.getComponent<TransformComponent>().setPos(562, 330);
	player.addComponent<SpriteComponent>("walkSheet.png", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayer);

	// Register device motion event callback
	//emscripten_set_devicemotion_callback( &(map.getComponent<PaintController>()), 1, motionCallback);

	topC.addComponent<TransformComponent>(0.0f, 254.0f,3000, 10);
	topC.addComponent<ColliderComponent>("topC");
	botC.addComponent<TransformComponent>(0.0f, 824.0f, 3000, 10);
	botC.addComponent<ColliderComponent>("botC");
	leftC.addComponent<TransformComponent>(-30.0f, 0.0f, 10, 800);
	leftC.addComponent<ColliderComponent>("leftC");
	rightC.addComponent<TransformComponent>(2738.0f, 0.0f, 10, 800);
	rightC.addComponent<ColliderComponent>("rightC");
	topC.addGroup(groupColliders);
	botC.addGroup(groupColliders);
	leftC.addGroup(groupColliders);
	rightC.addGroup(groupColliders);
}

// We have to notify the player's paintController of the android touch event

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	/*while (SDL_PollEvent(&event)) {
		if (event.type != lastEvent || lastEvent == SDL_FINGERMOTION) {
			lastEvent = event.type;
			switch (event.type) {
			case SDL_QUIT: isRunning = false;
				break;
			case SDL_FINGERMOTION:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
				//map.getComponent<PaintController>().update_touch(event.type, event.tfinger.x * float(screenRect.w), event.tfinger.y* float(screenRect.h));
				break;
			case SDL_MOUSEBUTTONDOWN:
				//std::cout << "Inside Mouse event switch case " << event.type << std::endl;
			default:
				break;
			}
		}
	}
*/
}
/*
			map.getComponent<PaintController>()
				.update_motion(
					event.tfinger.x * float(screenRect.w), event.tfinger.y* float(screenRect.h),
					event.tfinger.dx * float(screenRect.w), event.tfinger.dy* float(screenRect.h)
				);
			std::cout << "Finger motion: x,y dx,dy: " << event.tfinger.x<<" "<<event.tfinger.y<<" "<<event.tfinger.dx<<" "<<event.tfinger.dy<<" "  << std::endl;
			break;
*/

void Game::update()
{
	manager.refresh();
	manager.update();

	camera.x = player.getComponent<TransformComponent>().pos.x - 683;
	camera.y = player.getComponent<TransformComponent>().pos.y - 320;

	for (auto cc : colliders) {
		if (cc->tag != player.getComponent<ColliderComponent>().tag && Collision::AABB(*cc, player.getComponent<ColliderComponent>()))
		{
			if (cc->tag == "topC")
				player.getComponent<TransformComponent>().pos.y += 6;
			else if (cc->tag == "botC")
				player.getComponent<TransformComponent>().pos.y -= 6;
			if (cc->tag == "leftC")
				player.getComponent<TransformComponent>().pos.x += 6;
			else if (cc->tag == "rightC")
				player.getComponent<TransformComponent>().pos.x -= 6;
		}
	}
	if (camera.x > camera.w)
		camera.x = camera.w;
	else if(camera.x < 0)
		camera.x = 0;

	if (camera.y < -56)
		camera.y = -56;
	else if (camera.y > 170)
		camera.y = 170;
}

//Once you have alot of groups just put them in a vector to get rid of copy paste for loops below
void Game::render()
{
	SDL_RenderClear(renderer);
	for (auto& t : mapGroup) t->draw();
	for (auto& t : playerGroup) t->draw();
	for (auto& t : collidersGroup) t->draw();

	//manager.draw();
	SDL_RenderPresent(renderer);
}

void Game::AddTile(int x, int y, int w, int h, const char* path)
{
	auto& tile(manager.addEntity());
	tile.addComponent<TransformComponent>(x, y, w, h);
	tile.addComponent<SpriteComponent>(path, 1, 0);
	tile.addGroup(groupMap);
}

Game::~Game()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Game cleaned." << std::endl;
}
/**
EM_BOOL motionCallback(int eventType, const EmscriptenDeviceMotionEvent * e, void * userData)
{
	//std::cout<< e->accelerationIncludingGravityX<<"  "<< e->accelerationIncludingGravityY << std::endl;
	int x = e->accelerationIncludingGravityX;
	player.getComponent<KeyboardController>().update_x(x);
	return 1;
}
*/
