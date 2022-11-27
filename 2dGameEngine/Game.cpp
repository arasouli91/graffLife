/*
All the entitty creation and component adding will probably need to be taken care of in a separate function
Initially the game must load up appropriate starting information
Based on this information the appropriate entities and components will be created
Then reloading occurs when transistioning levels
*/
#pragma region Initial
#include "stdafx.h"
#include "Game.h"
#include "TextureManager.h"
#include "Map.h"
#include "Components.h"
#include "Collision.h"
#include <emscripten/html5.h>
#include <emscripten/emscripten.h>
#include <emscripten/websocket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
//using std::cout;
using std::endl;

//EM_BOOL motionCallback(int eventType, const EmscriptenDeviceMotionEvent *e, void *userData);

Manager manager;

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
//SDL_Texture* Game::canvas = nullptr;

SDL_Rect Game::camera = { 0,0,3200,640 };//////1366

const char* mapfile = "";

enum groupLabels : std::size_t
{
	groupMap, groupPlayer, groupColliders
};

std::vector<ColliderComponent*> Game::colliders;
auto& topC(manager.addEntity(&manager));
auto& botC(manager.addEntity(&manager));
auto& rightC(manager.addEntity(&manager));
auto& leftC(manager.addEntity(&manager));

auto& sky(manager.addEntity(&manager));
auto& map(manager.addEntity(&manager));

auto& player(manager.addEntity(&manager));

auto& mapGroup(manager.getGroup(groupMap));
auto& playerGroup(manager.getGroup(groupPlayer));
auto& collidersGroup(manager.getGroup(groupColliders));

// Player Entity objects hashed to their server given uid
std::unordered_map<std::string, Entity*> players;

bool Game::isRunning = true;

EMSCRIPTEN_WEBSOCKET_T socket;
bool socketOpen = 0;

#pragma endregion

std::vector<std::string> splitStr(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void sendUpdateToServer(bool playerFaceRight = false)
{
	std::stringstream ss;
	ss << int(player.getComponent<TransformComponent>().pos.x) << ","
		<< int(player.getComponent<TransformComponent>().pos.y)
		<< (playerFaceRight ? ":1" : ":0");

	std::string str = ss.str();
	const char * data = str.c_str();
	//emscripten_websocket_send_utf8_text(socket, data);/////////////////////////////DBG... why is this not a function? we need this
}

#pragma region WebSocket
int WebSocketOpen(int eventType, const EmscriptenWebSocketOpenEvent *e, void *userData)
{
	printf("open(eventType=%d, userData=%d)\n", eventType, (int)userData);

	socketOpen = true;

	sendUpdateToServer();

	//emscripten_websocket_close(e->socket, 0, 0);
	return 0;
}

int WebSocketClose(int eventType, const EmscriptenWebSocketCloseEvent *e, void *userData)
{
	printf("close(eventType=%d, wasClean=%d, code=%d, reason=%s, userData=%d)\n", eventType, e->wasClean, e->code, e->reason, (int)userData);
	return 0;
}

int WebSocketError(int eventType, const EmscriptenWebSocketErrorEvent *e, void *userData)
{
	printf("error(eventType=%d, userData=%d)\n", eventType, (int)userData);
	return 0;
}

// Receives message from server
// Updates and adds players to players hash
int WebSocketMessage(int eventType, const EmscriptenWebSocketMessageEvent *e, void *userData)
{
	//printf("message(eventType=%d, userData=%d, data=%p, numBytes=%d, isText=%d)\n", eventType, (int)userData, e->data, e->numBytes, e->isText);
	if (e->isText) {
		std::unordered_set<std::string> updatedPlayers;
		//printf("text data: \"%s\"\n", e->data);
		// Extract position data
		std::string dataStr{ (char*)e->data };		// data from server
		
			auto positionData = splitStr(dataStr, '|');	// id:posVec:optionalDirFlag|id:posVec|id:posVec|...
			// Update players list
			for (auto const &it : positionData) {			// iterate id:posVec pairs
				auto dataSplit = splitStr(it, ':');				// split id and posVec and flag
				if (dataSplit.size() < 2) continue;				// avoid exceptions
				auto posVecStrs = splitStr(dataSplit[1], ',');	// split posVec
				if (posVecStrs.size() < 2) continue;			// avoid exceptions
				auto playerId = dataSplit[0];
				auto entityIt = players.find(playerId);		// search players hash for id

				/**
				std::cout << "msg to client:" << dataStr << endl;
				for (auto it : dataSplit) cout << it << endl;
				for (auto it : posVecStrs) cout << it << endl;
				cout << endl << endl;*//////////////////DBG

				if (entityIt == players.end()) {			// if player id not in hash, must create player
					// create player
					auto& newPlayer(manager.addEntity(&manager));
					newPlayer.addComponent<TransformComponent>(0, 0, 73, 206, 1, 1, 1);
					// Set player at position received from server
					newPlayer.getComponent<TransformComponent>().setPos(stoi(posVecStrs[0]), stoi(posVecStrs[1]));
					newPlayer.addComponent<SpriteComponent>("walkSheet.png", true);
					newPlayer.addComponent<ColliderComponent>(playerId);
					newPlayer.addGroup(groupPlayer);
					if (dataSplit.size() > 2) {
						SDL_RendererFlip faceRight = stoi(dataSplit[2]) == 1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
						newPlayer.getComponent<SpriteComponent>().setSpriteFlip(faceRight);
					}
					players.insert(std::make_pair(playerId, &newPlayer));		// insert to hash
				}
				else // Player exists, update it
				{
					if (dataSplit.size() > 2) {
						SDL_RendererFlip faceRight = stoi(dataSplit[2]) == 1 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
						entityIt->second->getComponent<SpriteComponent>().setSpriteFlip(faceRight);
					}
					entityIt->second->getComponent<TransformComponent>().setPos(stoi(posVecStrs[0]), stoi(posVecStrs[1]));
					updatedPlayers.insert(playerId);
				}
			} 
		// Update local animations
		/**
		for (auto& it : players) {
			// Find players without updates
			if (updatedPlayers.find(it.first) == updatedPlayers.end()) {
				// Set their animation to idle
				it.second->getComponent<SpriteComponent>().play("idle");
			}else
				it.second->getComponent<SpriteComponent>().play("walk");
		}*/
		////////WON'T WORK: seems some initially spawned players already walk
		/////more importantly, if nobody is sending updates, then some idlers are still animated
	}
	else
	{
		printf("binary data:");
		for (int i = 0; i < e->numBytes; ++i)
			printf(" %02X", e->data[i]);
		printf("\n");

		//emscripten_websocket_delete(e->socket);
		//exit(0);
	}
	return 0;
}
#pragma endregion WebSocket


Game::Game(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	screenRect.w = width; screenRect.h = height;
#pragma region SDL Init
	int flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	if (fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == 0)//SDL_INIT_EVERYTHING == 0)) //& ~(SDL_INIT_HAPTIC)) == 0)
	{
		//std::cout << "Subsystems Initialized.." << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		if (window)
		{
			//std::cout << "Window created." << std::endl;
		}
		else {			//SDL_GetError() returns any error from any SDL function.
			//std::cout << "Window cannot be created: "<<SDL_GetError() << std::endl;
		}

		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			//std::cout << "Renderer created." << std::endl;
		}
		isRunning = true;
	}
	else {
		printf("SDL could not init! SDL_Error: %s\n", SDL_GetError());
		isRunning = false;
	}
#pragma endregion
#pragma region Websocket Init
	EmscriptenWebSocketCreateAttributes attr;
	emscripten_websocket_init_create_attributes(&attr);

	attr.url = "wss://grafflifetest.herokuapp.com";

	socket = emscripten_websocket_new(&attr);
	if (socket <= 0)
	{
		printf("WebSocket creation failed, error code %d!\n", (int)socket);
		exit(1);
	}

	emscripten_websocket_set_onopen_callback(socket, (void*)42, WebSocketOpen);
	emscripten_websocket_set_onclose_callback(socket, (void*)43, WebSocketClose);
	emscripten_websocket_set_onerror_callback(socket, (void*)44, WebSocketError);
	emscripten_websocket_set_onmessage_callback(socket, (void*)45, WebSocketMessage);

#pragma endregion
#pragma region Entities
	//The order that the components are added matters, and order entities made matter:
	//A component can't contain a member pointer to another until that other comp has been added. or perhaps can't contain a pointer to it in an function

	//We should make sure the actual sprite dimensions are very trimmed, otherwise we will have to trim in here and it only trims in one direction


	Map mapLoader;
	mapLoader.LoadMap("");

	///This should just be called canvas
	map.addComponent<TransformComponent>(0, 93, 3341, 372, 1, 1, 0);
	map.addComponent<CanvasComponent>();
	map.addComponent<PaintController>("assets/spray.png");
	//map.addComponent<ColliderComponent>("map");
	map.addGroup(groupMap);

	
	player.addComponent<TransformComponent>(0, 0, 73, 206, 1, 1, 1);
	player.getComponent<TransformComponent>().setPos(562, 330);
	lastX = 562; lastY = 330;
	player.addComponent<SpriteComponent>("walkSheet.png", true);
	player.addComponent<KeyboardController>();
	player.addComponent<ColliderComponent>("player");
	player.addGroup(groupPlayer);


	// Register device motion event callback
	//emscripten_set_devicemotion_callback( &(map.getComponent<PaintController>()), 1, motionCallback);

	topC.addComponent<TransformComponent>(0.0f, 254.0f, 6000, 10);
	topC.addComponent<ColliderComponent>("topC");
	botC.addComponent<TransformComponent>(0.0f, 824.0f, 6000, 10);
	botC.addComponent<ColliderComponent>("botC");
	leftC.addComponent<TransformComponent>(-30.0f, 0.0f, 10, 800);
	leftC.addComponent<ColliderComponent>("leftC");
	rightC.addComponent<TransformComponent>(4588.0f, 0.0f, 10, 800);
	rightC.addComponent<ColliderComponent>("rightC");
	topC.addGroup(groupColliders);
	botC.addGroup(groupColliders);
	leftC.addGroup(groupColliders);
	rightC.addGroup(groupColliders);
#pragma endregion
}

// We have to notify the player's paintController of the android touch event

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	while (SDL_PollEvent(&event)) {
		if (event.type != lastEvent || lastEvent == SDL_FINGERMOTION) {
			lastEvent = event.type;
			switch (event.type) {
			case SDL_QUIT: isRunning = false;
				break;
	/*		case SDL_FINGERMOTION:
			case SDL_FINGERDOWN:
			case SDL_FINGERUP:
				//map.getComponent<PaintController>().update_touch(event.type, event.tfinger.x * float(screenRect.w), event.tfinger.y* float(screenRect.h));
				break;
			case SDL_MOUSEBUTTONDOWN:
				//std::cout << "Inside Mouse event switch case " << event.type << std::endl;
		*/	default:
				break;
			}
		}
	}
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
/**
const char* Game::getPaintUpdate()
{
	std::stringstream ss;
	ss << map.getComponent<PaintController>().getPaintUsage();
	std::string s{ ss.str() };
	return s.c_str();
}
*/
void Game::update()
{
	manager.refresh();
	manager.update();

	float posX = player.getComponent<TransformComponent>().pos.x;
	float posY = player.getComponent<TransformComponent>().pos.y;

	camera.x = posX - 683;
	camera.y = posY - 320;


	// Updates to server
	if (socketOpen) {
		if (posX != lastX || posY != lastY) {	// position changed
			lastX = posX; lastY = posY;
			sendUpdateToServer(player.getComponent<SpriteComponent>().spriteFlip != SDL_FLIP_NONE);
		}
	}

	/**
	if (++sendPaintInfoTimer % 5000 == 0) {////////////////////////////
		std::stringstream ss;
		ss << map.getComponent<PaintController>().getPaintUsage();

		std::string s{ ss.str() }; 
		EM_ASM_({ Module["recv"] = UTF8ToString($0); }, s.c_str());
	}*/

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
	auto& tile(manager.addEntity(&manager));
	tile.addComponent<TransformComponent>(x, y, w, h);
	tile.addComponent<SpriteComponent>(path, 1, 0);
	tile.addGroup(groupMap);
}

Game::~Game()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	//std::cout << "Game cleaned." << std::endl;
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
