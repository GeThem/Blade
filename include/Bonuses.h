#pragma once
#include "SDL_default.h"
#include "Player.h"

typedef enum BonusType
{
	DOUBLE_DAMAGE
};

typedef struct Bonus
{
	Image img;
	int currDur, maxDur;
	int currCD, maxCD;
	Player* player;
	bool isAvailable;
	int type;
	void(*ApplyFunc)(Bonus&, Player&);
	void(*UpdateFunc)(Bonus&, Uint16 dt);
	void(*DrawFunc)(Bonus&);
} Bonus;

void DoubleDamageInit(Bonus&, const SDL_Point&, float dur, float cd);
void DoubleDamageApply(Bonus&, Player&);
void DoubleDamageUpdate(Bonus&, Uint16 dt);
void DoubleDamageDraw(Bonus&);

void BonusDeinit(Bonus&);