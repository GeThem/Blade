#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

struct PressedButtons
{
	bool att = false, thrw = false;
};

struct Player
{
	Entity ent;
	float maxHP, currHP;
	float attackCD, currAttCD, attackDur, currAttDur;
	bool isAttacking = false, canAttack = true, isThrowing = false;
	int projectilesCount = 5;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 150 };
	PressedButtons pressedB;
	SDL_Scancode controls;
};

void PlayerLoadCharacter(Player&, const char* filename);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
void PlayerAttack(Player&);

void PlayerProjectilesUpdate(Player&);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerUpdate(Player&);

void PlayerDraw(const Player&);