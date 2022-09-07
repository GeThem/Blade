#pragma once

#include "SDL_default.h"
#include "utils.h"

typedef struct Platform
{
	SDL_Rect rect;
	bool hCollCheck = false, vCollCheck = true, isDismountable = false;
} Platform;

typedef struct Map
{
	Image bg;
	Image platformsLayer;
	Image fg;
	int platformsCount;
	Platform* platforms;
};

void PlatformInit(Platform&, const SDL_Rect&, bool hColl, bool vColl, bool isDismountable);

void MapLoad(Map&, const char* name);

void MapDrawBG(Map&);
void MapDrawPlatformsLayer(Map&);
void MapDrawFG(Map&);

void MapDeinit(Map&);