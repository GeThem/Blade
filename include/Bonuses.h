#pragma once
#include "SDL_default.h"
#include "Player.h"

typedef struct DoubleDamage
{
	Image img;
	int currDur, maxDur;
	int currCD, maxCD;
	Player* player;
	bool isAvailable;
} DoubleDamage;

void DoubleDamageInit(DoubleDamage&, const SDL_Point&, float dur, float cd);
void DoubleDamageApply(DoubleDamage&, Player&);
void DoubleDamageUpdate(DoubleDamage&, Uint16 dt);
void DoubleDamageDraw(DoubleDamage&);