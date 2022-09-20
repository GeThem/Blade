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
	DISABLED = 0x8,
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

	EVADE = 0x4000,
	ISBUSY = 0x8000,
	ISDISMOUNTING = 0x10000,
	ISDEALINGDMG = 0x20000,
	CANMOVE = 0x40000,
	CANATTACK = 0x80000,
	CANDEALDMG = 0x100000,
	CANCHARGE = 0x200000,
	CANPLUNGE = 0x400000,
	CANPARRY = 0x800000,
	CANEVADE = 0x1000000,
	ISHOLDINGATK = 0x2000000, 
	ISHOLDINGPARRY = 0x4000000,

	ALLFLAGS = 0xFFFFC000
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
	Uint32 status;
	Uint8 activeBonuses;
	DrawOrder dOrder;
	Entity ent;
	Uint8 numberOfAttacks;
	Sint8 currAtkIndex = 0;
	Attack* atks, * currAtk;
	SDL_Point spriteOffset;
	AnimatedSprite anims[7], * currSprite;
	ChargeAtk chargeAtk[2];
	float baseCritRate, currCritRate, baseCritDmg;
	float currStamina, stmRecharge = 0.6, currStmRecharge = 0.6;
	int currEvadeDur, evadeDur, evadeCD, currEvadeCD, currStaminaCD;
	int maxHP, currHP, maxStamina, baseDmg, currDmg;
	int parryCD, currParrCD, parryDur, currParrDur, disableDur;
	SDL_Rect plungeRect, hpBar, staminaBar;
	Controls ctrls;
	SDL_Color color;
} Player;

Uint32 PlayerGetStatus(Player&);
void PlayerToggleStatus(Player&, Status);
void PlayerSetStatus(Player&, Status);
Uint32 GetAnim(Uint32 status);

void PlayerLoadCharacter(Player&, const char* name);
void PlayerLoadCharacterSprites(Player&, const char* name);
void PlayerReboot(Player&, Sint8 dir);

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