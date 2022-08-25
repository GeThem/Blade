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

typedef enum Animations
{
	IDLE, RUN, JUMP, FALL, HIT, SLIDE, EVADE
} Animations;

typedef struct AnimatedSprite
{
	Image image;
	int frameTime, timeElapsed = 0, frameWdt, frameHgt, dur;
	Uint8 frameCount;
	SDL_Rect currFrame;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
} AnimatedSprite;

typedef struct Attack
{
	AnimatedSprite sprite;
	SDL_Rect hitbox;
	SDL_Point hitboxOffset;
	int CD, currCD, dur, currDur, delay, postAtkDur, betweenHitsTime = 0, stunDur;
	float dmg, staminaCost;
} Attack;

typedef struct chrgAtk
{
	int CD, currCD, dur, currDur, delay;
	int chargeTime = 0;
	float dmg, staminaCost;
} chrgAtk;

typedef struct Player
{
	Entity ent;
	Uint8 numberOfAttacks;
	Sint8  currAtk = 0;
	Attack* atks;
	SDL_Point spriteOffset;
	AnimatedSprite* currSprite;
	AnimatedSprite anims[6];
	chrgAtk chargeAtk;
	bool isHoldingAtk = false;
	char status[80] = "right";
	float maxHP, currHP, projBaseSpd, currEvadeDur, evadeDur, evadeCD, currEvadeCD;
	int parryCD, currParrCD, parryDur, currParrDur, disableDur;
	bool isBusy = false;
	bool canAttack = true, canDealDmg = true, isDealingDmg = false, isThrowing = false, canEvade = true;
	bool isDismounting = false, canParry = true;
	bool isDisabled = false, isStunned = false;
	PressedControls pressedCtrls;
	Projectile projectiles[5];
	SDL_Rect attackBox{ 0, 0, 120, 140 }, hpRect, staminaRect;
	//int atk;
	Controls ctrls;
	SDL_Color color;
} Player;

void PlayerLoadCharacter(Player&, const char* filename);
void PlayerLoadCharacterSprites(Player&, const char* name);
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

void PlayerDisableElapse(Player&, Uint16 dt);

void PlayerUpdate(Player&, Uint16 dt);

void PlayerDraw(const Player&);

int PlayerTakeHit(Player&, Attack&, int dir);

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font*, int size, const SDL_Color&);
void PlayerAnimate(Player&, Uint16 dt);

void PlayerClear(Player&);