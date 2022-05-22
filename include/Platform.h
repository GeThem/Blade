#pragma once

#include "SDL_default.h"

struct Platform
{
	SDL_Rect rect;
	bool hCollCheck = false, vCollCheck = true;
};

void PlatformDraw(const Platform&);