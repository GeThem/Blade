#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

typedef struct Controls
{
	SDL_Scancode left, right, jump, dismount, evade;
	SDL_Scancode attack, thrw, parry;
} Controls;

typedef struct PressedControls
{
	bool left = false, right = false, jump = false, dismount = false, evade = false;
	bool attack = false, thrw = false, parry = false;
} PressedControls;

typedef struct Player
{
	Entity ent;
	float maxHP, currHP, projBaseSpd, currEvadeDur, evadeDur, evadeCD, currEvadeCD;
	int attackCD, currAttCD, attackDur, currAttDur, parryCD, currParrCD, parryDur, currParrDur;
	bool isAttacking = false, canAttack = true, isThrowing = false, isEvading = false, canEvade = true;
	bool isDismounting = false, dismountLock = false, isParrying = false, canParry = true;
	PressedControls pressedCtrls;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 150 }, hpRect;
	Controls ctrls;
	SDL_Color color;
} Player;

void PlayerLoadCharacter(Player&, const char* filename);
void PlayerReboot(Player&);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
void PlayerProcessAttack(Player&, Uint16 dt);
void PlayerProcessParry(Player&, Uint16 dt);
void PlayerProcessEvade(Player&, Uint16 dt);

void PlayerProcessProjectiles(Player&);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerUpdate(Player&, Uint16 dt);

void PlayerDraw(const Player&);