#pragma once

#include "SDL_default.h"

typedef struct Entity
{
	SDL_FPoint pos;
	SDL_Rect rect;
	Sint8 dir = 1, slideDir = 1;
	float maxMS, currMS = 0, verMS = 0, vVel, hVel;
	bool isInAir = false, isMoving = false;
} Entity;

void EntityUpdate(Entity&);

void EntityMoveTo(Entity&, const SDL_FPoint& dest);

float EntityGetHorMid(const Entity&);

float EntityGetVerMid(const Entity&);