#pragma once

#include "SDL_default.h"

typedef struct Platform
{
	SDL_Rect rect;
	bool hCollCheck = false, vCollCheck = true, isDismountable = false;
} Platform;

void PlatformDraw(const Platform&);