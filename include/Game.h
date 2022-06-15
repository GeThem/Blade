#pragma once

#include "SDL_default.h"
#include "Entity.h"
#include "Platform.h"
#include "Player.h"
#include "Platform.h"
#include "Projectile.h"

#define TOMENU 1

typedef struct Game
{
	SDL_Event ev;
	Player players[2];
	Platform arena[5];
	Uint32 lastTime = 0, dt = 0, currTime = 0, lastRenderedTime = 0;
} Game;

void GameLoadControls(Game&);

Game GameInit();

void GameStart(Game&);

void GameFrameStartTime(Game&);

void GameHandleEvents(Game&);

void GameDelay(Game&);

void GameHandleArenaCollisions(Game&);

Sint8 GameUpdate(Game&);

void GameDraw(Game&);

void GameQuit();