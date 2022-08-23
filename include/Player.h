#pragma once

#include <string.h>
#include <atlfile.h>

#include "vfx.h"
#include "utils.h"
#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

typedef struct Controls
{
	SDL_Scancode left, right, jump, dismount, evade;
	SDL_Scancode attack, chargeAtk, thrw, parry;
} Controls;

typedef struct PressedControls
{
	bool left = false, right = false, jump = false, dismount = false, evade = false;
	bool attack = false, thrw = false, parry = false;
} PressedControls;

typedef struct Attack
{
	int CD, currCD, dur, currDur, delay;
	float dmg, staminaCost;
} Attack;

typedef struct chrgAtk
{
	int CD, currCD, dur, currDur, delay;
	int chargeTime = 0;
	float dmg, staminaCost;
} chrgAtk;

typedef enum Animations
{
	IDLE, MOVE, JUMP, FALL, HIT, EVADE
} Animations;

typedef struct AnimatedSprite
{
	SDL_Texture* texture = NULL;
	int frameTime, timeElapsed = 0, wdt, hgt, offsetX, offsetY;
	Uint8 currFrame, frameCount;
} AnimatedSprite;

typedef struct Player
{
	Entity ent;
	Uint8 numberOfAttacks = 1, currAtk = 0;
	Attack* atks;
	chrgAtk chargeAtk;
	bool isHoldingAtk = false;
	char status[50] = "right";
	float maxHP, currHP, projBaseSpd, currEvadeDur, evadeDur, evadeCD, currEvadeCD;
	int parryCD, currParrCD, parryDur, currParrDur;
	bool isAttacking = false, canAttack = true, isThrowing = false, isEvading = false, canEvade = true;
	bool isDismounting = false, dismountLock = false, isParrying = false, canParry = true, isDealingDmg = false;
	bool isDisabled = false;
	bool isBusy = false, isChrgAttacking;
	bool canDealDmg = true;
	PressedControls pressedCtrls;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 150 }, hpRect, staminaRect;
	//int atk;
	Controls ctrls;
	SDL_Color color;
} Player;

void PlayerLoadCharacter(Player&, const char* filename);
void PlayerReboot(Player&);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
void PlayerProcessAttack(Player&, Attack&, Uint16 dt);
void PlayerProcessParry(Player&, Uint16 dt);
void PlayerProcessEvade(Player&, Uint16 dt);

void PlayerResetAttacks(Player&, int number);
void PlayerResetChargeAttack(Player&);

void PlayerProcessProjectiles(Player&, Uint16 dt);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerUpdate(Player&, Uint16 dt);

void PlayerDraw(const Player&);

int PlayerTakeHit(Player&, int dmg, bool blockable = true);

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font*, int size, const SDL_Color&);