#pragma once

#include <atlfile.h>
#include <random>
#include <time.h>

#include "SDL_default.h"
#include "Platform.h"
#include "Player.h"
#include "Platform.h"
#include "vfx.h"
#include "List.h"
#include "Bonuses.h"

#define TOMENU 1

typedef struct Game
{
	int time = 9000;
	Player* drawPriority[2];
	Player players[2];
	Map map;
	VList vanishTexts;
	TTF_Font* playersInteractionsFont, * playersInteractionsFontOutline;
	Bonus ddbonuses[2];
} Game;

VanishText GameSpawnText(const SDL_FPoint& pos, const char* text, TTF_Font* font, int size,
	const SDL_Color& color, TTF_Font* outline = NULL);

void GameLoadControls(Game&);

void GameInit(Game&, const char* p1, const char* p2, const char* map);

void GameRestart(Game&);

void GameHandleArenaCollisions(Game&);

Sint8 GameUpdate(Game&, const Uint16& dt);

void GameDraw(Game&);

void GameClose(Game&);