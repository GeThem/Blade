#pragma once

#include <string.h>
#include <atlfile.h>

#include "vfx.h"
#include "utils.h"
#include "SDL_default.h"
#include "Platform.h"
#include "Entity.h"

#define MIN_PARRY_DUR 170
#define MIN_CHARGE_TIME 200
#define STAMINA_RECHARGE_CD 700
#define STAMINA_RECHARGE_RATE 0.6

typedef enum DrawOrder
{
	PLATFORMS,
	FOREGROUND
} DrawOrder;

typedef struct Controls
{
	SDL_Scancode left, right, jump, dismount, evade;
	SDL_Scancode attack, chargeAtk, thrw, parry;
} Controls;

typedef enum Status
{
	IDLE = 0x0,
	RUN = 0x1,
	JUMP = 0x2,
	FALL = 0x4,
	HIT = 0x8,
	DEAD = 0x10,
	PARRY = 0x20,
	ATTACK = 0x40,
	POSTATTACK = 0x80,
	CHRG = 0X100,
	CHRGATK = 0x200,
	CHRGPOSTATK = 0x400,
	PLUNGE = 0x800,
	PLUNGEATK = 0x1000,
	PLUNGEPOSTATK = 0x2000,
	EVADE = 0x4000
} Status;

typedef struct AnimatedSprite
{
	Image image;
	int frameTime, timeElapsed = 0, frameWdt, dur;
	Uint8 frameCount;
	SDL_Rect currFrame;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
} AnimatedSprite;

typedef enum AttackDir
{
	BACKWARD = -1,
	FORWARD = 1,
	BOTH = 0
} AttackDir;

typedef struct Attack
{
	Sint8 dir;
	AnimatedSprite sprite;
	SDL_Rect hitbox;
	SDL_Point hitboxOffset;
	int CD, currCD, dur, currDur, delay, postAtkDur, stunDur;
	float dmg, staminaCost;
} Attack;

typedef struct ChargeAtk
{
	Attack atk;
	AnimatedSprite chrgSprite;
	int preChrgTime = 0;
	int chargeTime = 0;
} ChargeAtk;

typedef struct Player
{
	DrawOrder dOrder;
	Entity ent;
	Uint8 numberOfAttacks;
	Sint8 currAtkIndex = 0;
	Attack* currAtk;
	Attack* atks;
	SDL_Point spriteOffset;
	AnimatedSprite* currSprite;
	AnimatedSprite anims[7];
	ChargeAtk chargeAtk[2];
	int baseDmg, currDmg;
	float baseCritRate = 15, currCritRate = 15;
	bool isHoldingAtk = false, isHoldingParry = false;
	Uint16 status = IDLE;
	int currEvadeDur, evadeDur, evadeCD, currEvadeCD;
	float currStamina;
	int maxHP, currHP, maxStamina;
	int parryCD, currParrCD, parryDur, currParrDur, disableDur;
	bool isBusy = false;
	bool isDealingDmg = false, canEvade = true;
	bool isDismounting = false, canParry = true;
	bool isDisabled = false, isStunned = false;
	bool canMove = true, canAttack = true, canDealDmg = true, canCharge = true, canPlunge = false;
	int currStaminaCD = 0;
	SDL_Rect plungeRect;
	SDL_Rect hpBar, staminaBar;
	Controls ctrls;
	SDL_Color color;
} Player;

Uint16 PlayerGetStatus(Player&);
void PlayerToggleStatus(Player&, Status);
void PlayerSetStatus(Player&, Status);
Uint16 GetAnim(Uint16 status);

void PlayerLoadCharacter(Player&, const char* name);
void PlayerLoadCharacterSprites(Player&, const char* name);
void PlayerReboot(Player&);

void PlayerInput(Player&);
//Duration in secs
void PlayerAttackPenalty(Player&, float duration);
bool PlayerProcessAttack(Player&, Attack&, Uint16 dt);
void PlayerProcessParry(Player&, Uint16 dt);
void PlayerProcessEvade(Player&, Uint16 dt);

void PlayerResetAttacks(Player&, int number);
void PlayerResetChargeAttack(Player&, int type);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerDisableElapse(Player&, Uint16 dt);
void PlayerDecreaseStamina(Player&, float amount);
void PlayerStaminaRecharge(Player&, Uint16 dt);

void PlayerPhysics(Player&);
void PlayerUpdate(Player&, Uint16 dt);

void PlayerDraw(const Player&);

VanishText PlayerAttack(Player& self, Player& target, TTF_Font* font, TTF_Font* outline=NULL);
int PlayerTakeHit(Player&, int dmg, int stunDur, int dir, int pushPower);

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font*, int size, const SDL_Color&, TTF_Font* outline=NULL);
void PlayerAnimate(Player&, Uint16 dt);

void PlayerDisable(Player&, float dur);

void PlayerCancelParry(Player&);

void PlayerClear(Player&);