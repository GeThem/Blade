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
	int time = 9000;
	Player* drawPriority[2];
	Player players[2];
	Platform arena[10];
	VList texts;
	TTF_Font* playersInteractionsFont;
} Game;

void GameLoadControls(Game&);

void GameInit(Game&);

void GameRestart(Game&);

void GameHandleArenaCollisions(Game&);

Sint8 GameUpdate(Game&, const Uint16& dt);

void GameDraw(Game&);

void GameClose(Game&);