#pragma once

#include "Platform.h"
#include "Bonuses.h"
#include "utils.h"

typedef struct Map
{
	SDL_FPoint p1Pos, p2Pos;
	Image bg;
	Image platformsLayer;
	Image fg;
	int platformsCount, bonusesCount;
	Platform* platforms;
	Bonus* bonuses;
} Map;

void MapLoad(Map&, const char* name);

void MapDrawBG(Map&);
void MapDrawPlatformsLayer(Map&);
void MapDrawFG(Map&);

void MapDeinit(Map&);