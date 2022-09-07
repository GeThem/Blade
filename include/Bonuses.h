#pragma once
#include "SDL_default.h"
#include "Player.h"

#define BONUS_TYPES 2

typedef enum BonusType
{
	DOUBLE_DAMAGE=0x1,
	HASTE=0x2
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

void HasteInit(Bonus&, const SDL_Point&, float dur, float cd);
void HasteApply(Bonus&, Player&);
void HasteUpdate(Bonus&, Uint16 dt);
void HasteDraw(Bonus&);

void BonusDeinit(Bonus&);