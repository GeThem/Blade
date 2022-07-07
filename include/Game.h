#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Player.h"
#include "Platform.h"
#include "Projectile.h"

#define TOMENU 1

typedef struct Game
{
	Player players[2];
	Platform arena[10];
} Game;

void GameLoadControls(Game&);

Game GameInit();

void GameRestart(Game&);

void GameHandleArenaCollisions(Game&);

Sint8 GameUpdate(Game&, const Uint16& dt);

void GameDraw(const Game&);

void GameQuit();