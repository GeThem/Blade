#include <string.h>
#include <atlfile.h>

#include "Player.h"

void PlayerLoadCharacter(Player& self, const char* filename)
{
	char temp[40] = "data/characters/";
	strcat_s(temp, filename);
	FILE* file;
	if (fopen_s(&file, temp, "r"))
		exit(0);
	while (!feof(file))
	{
		fgets(temp, 40, file);
		char* val = strchr(temp, '=');
		*val++ = '\0';
		
		if (!strcmp(temp, "width"))
			self.ent.rect.w = atoi(val);
		else if (!strcmp(temp, "height"))
			self.ent.rect.h = atoi(val);
		else if (!strcmp(temp, "ms"))
		{
			self.ent.maxMS = atof(val);
			self.ent.hVel = self.ent.maxMS / 20;
		}
		else if (!strcmp(temp, "weight"))
			self.ent.vVel = atof(val);
		else if (!strcmp(temp, "hp"))
			self.currHP = self.maxHP = atof(val);
		else if (!strcmp(temp, "prbasespd"))
			self.projBaseSpd = atof(val);
		else if (!strcmp(temp, "attackcd"))
			self.attackCD = self.currAttCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "attackdur"))
			self.attackDur = self.currAttDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parrycd"))
			self.parryCD = self.currParrCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parrydur"))
			self.parryDur = self.currParrDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evadecd"))
			self.evadeCD = self.currEvadeCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evadedur"))
			self.evadeDur = self.currEvadeDur = atof(val) * 1000.0f;
	}
	fclose(file);
}

void PlayerInput(Player& self)
{
	if (self.ent.isMoving = kbState[self.ctrls.left] + kbState[self.ctrls.right] == 1)
		self.ent.dir = -kbState[self.ctrls.left] + kbState[self.ctrls.right];

	if (OnPress(self.ctrls.jump) && !self.ent.isInAir)
		self.ent.verMS = -self.ent.vVel * 17;

	if (OnPress(self.ctrls.attack))
		self.isAttacking = self.canAttack;

	if (OnPress(self.ctrls.parry))
		self.isParrying = self.canParry;

	if (OnPress(self.ctrls.evade))
		self.isEvading = self.canEvade;

	if (OnPress(self.ctrls.thrw))
		self.isThrowing = !self.isEvading && !self.isAttacking && !self.isParrying;

	if (!self.dismountLock)
		self.isDismounting = kbState[self.ctrls.dismount];
}

void PlayerPlatformVerCollision(Player& self, Platform& platform)
{
	if (!platform.vCollCheck)
		return;
	if (self.ent.verMS < 0.0f ||
		self.ent.rect.y - platform.rect.y > self.ent.rect.h * -0.8f ||
		!SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	if (self.isDismounting && platform.isDismountable && self.ent.verMS > 0)
	{
		self.dismountLock = true;
		if (self.ent.verMS + self.ent.pos.y + self.ent.rect.h * 0.9f >= platform.rect.y + platform.rect.h)
			self.dismountLock = self.isDismounting = false;
		return;
	}
	self.ent.isInAir = false;
	self.ent.verMS = 0;
	EntityMoveTo(self.ent, { self.ent.pos.x, (float)platform.rect.y - self.ent.rect.h });
	return;
}

void PlayerPlatformHorCollision(Player& self, Platform& platform)
{
	if (!platform.hCollCheck)
		return;
	SDL_Rect intersection;
	if (!SDL_IntersectRect(&self.ent.rect, &platform.rect, &intersection))
		return;
	self.ent.isMoving = false;
	self.ent.currMS = 0;
	SDL_FPoint moveTo{ platform.rect.x, self.ent.pos.y };
	if (EntityGetHorMid(self.ent) >= intersection.x + intersection.w / 2)
		moveTo.x += platform.rect.w;
	else
		moveTo.x -= self.ent.rect.w;
	EntityMoveTo(self.ent, moveTo);
	return;
}

// TODO: attack parry and evasion aren't dependant, can penalted
void PlayerAttack(Player& self)
{
	if (!self.isAttacking && self.canAttack)
		return;
	self.canAttack = self.canEvade = self.canParry = false;
	if (self.isAttacking)
	{
		self.currAttDur -= FRAME_DELAY;
		if (self.currAttDur >= 0)
		{
			self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			self.attackBox.y = self.ent.pos.y;
			return;
		}
	}
	self.isAttacking = false;
	self.canEvade = self.canParry = true;
	self.currAttCD -= FRAME_DELAY;
	if (self.currAttCD > 0)
		return;
	self.currAttCD = self.attackCD;
	self.currAttDur = self.attackDur;
	self.canAttack = true;
}

void PlayerParry(Player& self)
{
	if (!self.isParrying && self.canParry)
		return;
	self.canAttack = self.canEvade = self.canParry = false;
	if (self.isParrying)
	{
		self.currParrDur -= FRAME_DELAY;
		if (self.currParrDur >= 0)
		{
			//self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			//self.attackBox.y = self.ent.pos.y;
			return;
		}
	}
	self.isParrying = false;
	self.canAttack = self.canEvade = true;
	self.currParrCD -= FRAME_DELAY;
	if (self.currParrCD > 0)
		return;
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.canParry = true;
}

void PlayerEvade(Player& self)
{
	if (!self.isEvading && self.canEvade)
		return;
	self.canAttack = self.canEvade = self.canParry = false;
	if (self.isEvading)
	{
		self.currEvadeDur -= FRAME_DELAY;
		if (self.currEvadeDur >= 0)
		{
			//self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			//self.attackBox.y = self.ent.pos.y;
			return;
		}
	}
	self.isEvading = false;
	self.canAttack = self.canParry = true;
	self.currEvadeCD -= FRAME_DELAY;
	if (self.currEvadeCD > 0)
		return;
	self.currEvadeCD = self.evadeCD;
	self.currEvadeDur = self.evadeDur;
	self.canEvade = true;
}

void PlayerThrowProjectile(Player& self, Projectile& projectile)
{
	if (self.isAttacking)
		return;
	projectile.isPickable = self.isThrowing = false;
	projectile.wasThrown = projectile.ent.isMoving = true;
	projectile.ent.hVel = 0;
	projectile.ent.currMS = self.ent.dir * self.projBaseSpd + self.ent.currMS;
	projectile.ent.maxMS = fabs(projectile.ent.currMS);
	projectile.ent.verMS = projectile.ent.vVel * -5;
	SDL_FPoint moveTo{ self.ent.pos.x, self.ent.rect.y + self.ent.rect.h * 0.2f };
	if (self.ent.dir == 1)
		moveTo.x += self.ent.rect.w;
	else
		moveTo.x -= projectile.ent.rect.w;
	EntityMoveTo(projectile.ent, moveTo);
}

void PlayerProcessProjectiles(Player& self)
{
	for (Projectile& projectile : self.projectiles)
	{
		if (self.isThrowing && !projectile.wasThrown)
		{
			PlayerThrowProjectile(self, projectile);
			continue;
		}
		if (SDL_HasIntersection(&self.ent.rect, &projectile.ent.rect) && projectile.isPickable)
		{
			ProjectilePickUp(projectile);		
			continue;
		}
		ProjectileUpdate(projectile);
	}
	self.isThrowing = false;
}

void PlayerAttackPenalty(Player& self, float duration)
{
	duration *= 1000.0f;
	self.canAttack = self.isAttacking = false;
	self.currAttCD = self.attackCD;
	self.currAttCD = duration;
}

void PlayerUpdate(Player& self)
{
	PlayerInput(self);
	EntityUpdate(self.ent);
	PlayerAttack(self);
	PlayerParry(self);
	PlayerEvade(self);
	PlayerProcessProjectiles(self);
}

void PlayerDraw(const Player& self)
{
	if (self.isParrying)
		SDL_SetRenderDrawColor(ren, 200, 200, 0, 255);
	else
		SDL_SetRenderDrawColor(ren, self.color.r, self.color.g, self.color.b, 255);
	if (!self.isEvading)
		SDL_RenderFillRect(ren, &self.ent.rect);
	if (self.isAttacking)
	{
		SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
		SDL_RenderFillRect(ren, &self.attackBox);
	}
	SDL_SetRenderDrawColor(ren, 200, 50, 50, 255);
	SDL_RenderFillRect(ren, &self.hpRect);
}