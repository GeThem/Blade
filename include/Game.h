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
	Player player;
	Platform arena[5];
	double dt;
	Uint32 frameStart, frameTime;
};

Game GameInit(int win_w, int win_h);

void GameFrameStartTime(Game&);

void GameHandleEvents(Game&);

void GameDelay(Game&);

void GameHandleArenaCollisions(Game&);

void GameUpdate(Game&);

void GameDraw(Game&);

void GameQuit();