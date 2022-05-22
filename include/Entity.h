#pragma once

#include <stdint.h>

#include "SDL_default.h"

struct Entity
{
	SDL_FPoint pos;
	SDL_Rect rect;
	int8_t dir = 0;
	float maxMS, currMS = 0, weight, hVel, vVel = 0;
	bool isInAir = false, isMoving = false;
};

void EntityUpdate(Entity&);

void EntityMoveTo(Entity&, const SDL_Point dest);

int EntityGetHorMid(const Entity&);

int EntityGetVerMid(const Entity&);