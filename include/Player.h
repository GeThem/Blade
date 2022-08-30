#pragma once

#include <string.h>
#include <atlfile.h>

#include "vfx.h"
#include "utils.h"
#include "SDL_default.h"
#include "Platform.h"
#include "Projectile.h"
#include "Entity.h"

#define MIN_PARRY_DUR 170
#define MIN_CHARGE_TIME 100
#define STAMINA_RECHARGE_CD 700
#define STAMINA_RECHARGE_RATE 0.6

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
	IDLE, RUN, JUMP, FALL, HIT, DEATH
} Animations;

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
};

typedef struct Attack
{
	Sint8 dir;
	AnimatedSprite sprite;
	SDL_Rect hitbox;
	SDL_Point hitboxOffset;
	int CD, currCD, dur, currDur, delay, postAtkDur, stunDur;
	float dmg, staminaCost;
} Attack;

typedef struct chrgAtk
{
	Attack atk;
	AnimatedSprite chrgSprite;
	int prePlungeFrameCount = 0;
	int chargeTime = 0;
} chrgAtk;

typedef struct Player
{
	Entity ent;
	Uint8 numberOfAttacks;
	Sint8 currAtkIndex = 0;
	Attack* currAtk;
	Attack* atks;
	SDL_Point spriteOffset;
	AnimatedSprite* currSprite;
	AnimatedSprite anims[6];
	chrgAtk chargeAtk[2];
	int critRate = 25;
	bool isHoldingAtk = false, isHoldingParry = false;
	char status[80] = "right";
	float projBaseSpd, currEvadeDur, evadeDur, evadeCD, currEvadeCD, currStamina;
	int maxHP, currHP, maxStamina;
	int parryCD, currParrCD, parryDur, currParrDur, disableDur;
	bool isBusy = false;
	bool isDealingDmg = false, isThrowing = false, canEvade = true;
	bool isDismounting = false, canParry = true;
	bool isDisabled = false, isStunned = false;
	bool canMove = true, canAttack = true, canDealDmg = true, canCharge = true;
	int currStaminaCD = 0;
	SDL_Rect plungeRect;
	PressedControls pressedCtrls;
	Projectile projectiles[5];
	SDL_Rect hpBar, staminaBar;
	//int atk;
	Controls ctrls;
	SDL_Color color;
} Player;

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

void PlayerProcessProjectiles(Player&, Uint16 dt);

void PlayerPlatformVerCollision(Player&, Platform&);
void PlayerPlatformHorCollision(Player&, Platform&);

void PlayerDisableElapse(Player&, Uint16 dt);
void PlayerDecreaseStamina(Player&, float amount);
void PlayerStaminaRecharge(Player&, Uint16 dt);

void PlayerUpdate(Player&, Uint16 dt);

void PlayerDraw(const Player&);

VanishText PlayerAttack(Player& self, Player& target, TTF_Font* font);
int PlayerTakeHit(Player&, int dmg, int stunDur, int dir, int pushPower);

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font*, int size, const SDL_Color&);
void PlayerAnimate(Player&, Uint16 dt);

void PlayerDisable(Player&, float dur);

void PlayerCancelParry(Player&);

void PlayerClear(Player&);