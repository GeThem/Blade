#pragma once

#include <atlfile.h>
#include <random>
#include <time.h>

#include "SDL_default.h"
#include "Platform.h"
#include "Player.h"
#include "Platform.h"
#include "Projectile.h"
#include "vfx.h"
#include "List.h"

#define TOMENU 1

typedef struct Game
{
	Player players[2];
	Platform arena[10];
	VList texts;
} Game;

void GameLoadControls(Game&);

Game GameInit();

void GameRestart(Game&);

void GameHandleArenaCollisions(Game&);

Sint8 GameUpdate(Game&, const Uint16& dt);

void GameDraw(Game&);

void GameQuit();