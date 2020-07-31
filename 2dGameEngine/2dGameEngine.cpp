// 2dGameEngine.cpp : Defines the entry point for the console application.
//
#ifdef __EMSCRIPTEN__
#include "emscripten/emscripten.h"
#endif
#include <iostream>
#include "stdafx.h"
#include "Game.h"
#include <chrono>
using namespace std::chrono;
Game *game = nullptr;

const int FPS = 60;					//60 is closest to refresh rate of monitors
const int frameDelay = 1000 / FPS;	//Max time between frames

// The "main loop" function.
void one_iter() {
	//TODO: should put if game running?


#ifdef __EMSCRIPTEN__
	steady_clock::time_point frameStart = steady_clock::now();
#endif

	game->handleEvents();
	game->update();
	game->render();

#ifdef __EMSCRIPTEN__
	steady_clock::duration timeSpan = steady_clock::now() - frameStart;

	/// I don't really know if this math is correct, but I am trying to convert to milliseconds.
	/// The numbers I am getting make sense. Frames are taking < 3 milli?seconds
	// There is most likely extra shit happening here. num/den is stead_clock's period ratio, 
	double frameTime = double(timeSpan.count()) * steady_clock::period::num / steady_clock::period::den;
	frameTime *= 1000;

	if (frameTime < frameDelay) {

	// This while loop was? causing serious lag
	// Well we don't really need this, but it might be good code?

		while (frameTime < frameDelay) {
			timeSpan = steady_clock::now() - frameStart;
			frameTime = double(timeSpan.count()) * steady_clock::period::num / steady_clock::period::den;
			frameTime *= 1000;
		}
	}
//else
//std::cout << frameTime<<std::endl;
	///////
	// WHY IS FRAMETIME COMING OUT AS IN AN INTEGER?
	/*

	if (frameDelay > frameTime)	// Tried this
	{
		SDL_Delay(frameDelay - frameTime);
	}*/
	// Supposed to help framerate? Supposed to free up resources after the frame?
	SDL_Delay(0);
#endif
}

int main(int argc, char * argv[])
{
	Uint32 frameStart, frameTime;	

	game = new Game("2dGameEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1366, 640, false);

	///////////////////
	std::cout << "*******I AM DEBUGGING THIS RIGHT NOW, EXPECT ANOMALIES 11/09/2018*********\n\n";

	std::cout	<< "GRAFF LIFE - ALPHA STAGE\n*************CONTROLS*************\nPAINT : LEFT MOUSE   (MUST BE NEAR WALL)"
				<< std::endl << "MOVEMENT : W-A-S-D" << std::endl << "FLARE(TILT UP) CAN : HOLD UP KEY";
	std::cout	<< std::endl << "INCREASE RED COLOR : HOLD R  -  DECREASE RED COLOR : HOLD R + HOLD DOWN KEY"
				<< std::endl << "INCREASE GREEN COLOR : HOLD G  -  DECREASE GREEN COLOR : HOLD G + HOLD DOWN KEY";
	std::cout	<< std::endl << "INCREASE BLUE COLOR : HOLD B  -  DECREASE BLUE COLOR : HOLD B + HOLD DOWN KEY\n\n\n\n";
	///////////////////
	std::cout << "*******I AM DEBUGGING THIS RIGHT NOW, EXPECT ANOMALIES 10/19/2018*****0.2***\n\n";
#ifdef __EMSCRIPTEN__
		// void emscripten_set_main_loop(em_callback_func func, int fps, int simulate_infinite_loop);
		emscripten_set_main_loop(one_iter, 0, 1);
#endif
	

#ifndef __EMSCRIPTEN__
		while (game->running())
		{
			frameStart = SDL_GetTicks();

			one_iter();

			//How many ms, to handle out events, update objects, and render
			frameTime = SDL_GetTicks() - frameStart;
			if (frameDelay > frameTime)
			{
				SDL_Delay(frameDelay - frameTime);
			}
		}
#endif
		return 0;
}

///
/*
MAYBE THERE'S NO POINT AT ALL USING SDL_GetTicks() for desktop build. This C++11 soln may be the most accurate anyways.
*/
