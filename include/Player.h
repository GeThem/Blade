#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

struct Controls
{
	SDL_Scancode left, right, jump, dismount, evade;
	SDL_Scancode attack, thrw, parry;
};

struct Player
{
	Entity ent;
	float maxHP, currHP, projBaseSpd, currEvadeDur, evadeDur, evadeCD, currEvadeCD;
	float attackCD, currAttCD, attackDur, currAttDur, parryCD, currParrCD, parryDur, currParrDur;
	bool isAttacking = false, canAttack = true, isThrowing = false, isEvading = false, canEvade = true;
	bool isDismounting = false, dismountLock = false, isParrying = false, canParry = true;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 150 }, hpRect;
	Controls ctrls;
	SDL_Color color;
};

void PlayerLoadCharacter(Player&, const char* filename);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
void PlayerAttack(Player&);

void PlayerParry(Player&);

void PlayerEvade(Player&);

void PlayerProcessProjectiles(Player&);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerUpdate(Player&);

void PlayerDraw(const Player&);