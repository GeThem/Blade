#pragma once

#include "SDL_default.h"
#include "Entity.h"
#include "Platform.h"
#include "Player.h"
#include "Platform.h"
#include "Projectile.h"

struct Game
{
	SDL_Event ev;
	Player players[2];
	Platform arena[5];
	Uint32 frameStart, frameTime;
};

void GameLoadControls(Game&);

Game GameInit(int winW, int winH);

void GameFrameStartTime(Game&);

void GameHandleEvents(Game&);

void GameDelay(Game&);

void GameHandleArenaCollisions(Game&);

void GameUpdate(Game&);

void GameDraw(Game&);

void GameQuit();