#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

struct Controls
{
	SDL_Scancode left, right, jump, dismount;
	SDL_Scancode attack, thrw;
};

struct Player
{
	Entity ent;
	float maxHP, currHP, projBaseSpd;
	float attackCD, currAttCD, attackDur, currAttDur;
	bool isAttacking = false, canAttack = true, isThrowing = false;
	bool isDismounting = false, dismountLock = false;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 150 };
	Controls ctrls;
	SDL_Color color;
};

void PlayerLoadCharacter(Player&, const char* filename);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
void PlayerAttack(Player&);

void PlayerProcessProjectiles(Player&);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerUpdate(Player&);

void PlayerDraw(const Player&);