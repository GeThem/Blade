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
			self.maxHP = atof(val);
		else if (!strcmp(temp, "prbasespd"))
			self.projBaseSpd = atof(val);
		else if (!strcmp(temp, "attackcd"))
			self.attackCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "attackdur"))
			self.attackDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parrycd"))
			self.parryCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parrydur"))
			self.parryDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evadecd"))
			self.evadeCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evadedur"))
			self.evadeDur = atof(val) * 1000.0f;
	}
	fclose(file);
}

void PlayerReboot(Player& self)
{
	self.currHP = self.maxHP;
	self.currEvadeDur = self.evadeDur;
	self.currEvadeCD = self.evadeCD;
	self.currAttCD = self.attackCD;
	self.currAttDur = self.attackDur;
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.isAttacking = self.isThrowing = self.isEvading = self.isDismounting = self.dismountLock = self.isParrying = false;
	self.canAttack = self.canEvade = self.canParry = true;
	self.ent.currMS = self.ent.verMS = 0;
	self.ent.isInAir = self.ent.isMoving = false;
	self.ent.dir = 1;
	self.pressedCtrls = {};
	for (Projectile& projectile : self.projectiles)
	{
		projectile.ent.pos = { 0, 0 };
		projectile.ent.verMS = projectile.ent.currMS = 0;
		projectile.ent.rect = { 0, 0, 50, 50 };
		projectile.ent.vVel = 0.5;
		projectile.pickCD = projectile.currPickCD = 1000;
	}
}

void PlayerInput(Player& self)
{
	self.ent.isMoving = kb.state[self.ctrls.left] + kb.state[self.ctrls.right] == 1;
	if (self.ent.isMoving)
		self.ent.dir = -kb.state[self.ctrls.left] + kb.state[self.ctrls.right];

	if (OnKeyPress(self.ctrls.jump) && !self.ent.isInAir)
		self.ent.verMS = -self.ent.vVel * 17;

	self.pressedCtrls.attack = OnKeyPress(self.ctrls.attack);
	
	self.pressedCtrls.parry = OnKeyPress(self.ctrls.parry);
	
	self.pressedCtrls.evade = OnKeyPress(self.ctrls.evade);

	self.pressedCtrls.thrw = OnKeyPress(self.ctrls.thrw);

	if (!self.dismountLock)
		self.isDismounting = kb.state[self.ctrls.dismount];
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

void PlayerProcessAttack(Player& self, Uint16 dt)
{
	if (!self.pressedCtrls.attack && self.canAttack)
		return;
	self.canAttack = false;
	if (self.currAttDur >= 0)
	{
		self.isAttacking = true;
		self.currAttDur -= dt;
		self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
		self.attackBox.y = self.ent.pos.y;
		return;
	}
	self.isAttacking = false;
	self.currAttCD -= dt;
	if (self.currAttCD > 0)
		return;
	self.currAttCD = self.attackCD;
	self.currAttDur = self.attackDur;
	self.canAttack = true;
}

void PlayerProcessParry(Player& self, Uint16 dt)
{
	if (!self.pressedCtrls.parry && self.canParry)
		return;
	self.canParry = false;
	if (self.currParrDur >= 0)
	{
		self.isParrying = true;
		self.currParrDur -= dt;
		return;
	}
	self.isParrying = false;
	self.currParrCD -= dt;
	if (self.currParrCD > 0)
		return;
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.canParry = true;
}

void PlayerProcessEvade(Player& self, Uint16 dt)
{
	if (!self.pressedCtrls.evade && self.canEvade)
		return;
	self.canEvade = false;
	if (self.currEvadeDur >= 0)
	{
		self.isEvading = true;
		self.currEvadeDur -= dt;
		return;
	}
	self.isEvading = false;
	self.currEvadeCD -= dt;
	if (self.currEvadeCD > 0)
		return;
	self.currEvadeCD = self.evadeCD;
	self.currEvadeDur = self.evadeDur;
	self.canEvade = true;
}

void PlayerThrowProjectile(Player& self, Projectile& projectile)
{
	projectile.ent.slideDir = projectile.ent.dir = self.ent.dir;
	projectile.isPickable = self.pressedCtrls.thrw = self.isThrowing = false;
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

void PlayerProcessProjectiles(Player& self, Uint16 dt)
{
	for (Projectile& projectile : self.projectiles)
	{
		if (self.pressedCtrls.thrw && !projectile.wasThrown)
		{
			PlayerThrowProjectile(self, projectile);
			continue;
		}
		if (SDL_HasIntersection(&self.ent.rect, &projectile.ent.rect) && projectile.isPickable)
		{
			ProjectilePickUp(projectile);		
			continue;
		}
		ProjectileUpdate(projectile, dt);
	}
}

void PlayerAttackPenalty(Player& self, float duration)
{
	duration *= 1000.0f;
	self.canAttack = self.isAttacking = false;
	self.currAttCD = self.attackCD;
	self.currAttCD = duration;
}

void PlayerUpdate(Player& self, Uint16 dt)
{
	PlayerInput(self);
	EntityUpdate(self.ent);

	if (!self.isEvading && !self.isParrying || !self.canAttack)
		PlayerProcessAttack(self, dt);
	if (!self.isEvading && !self.isAttacking || !self.canParry)
		PlayerProcessParry(self, dt);
	if (!self.isAttacking && !self.isParrying || !self.canEvade)
		PlayerProcessEvade(self, dt);
	if (!self.isAttacking && !self.isEvading && !self.isParrying)
		PlayerProcessProjectiles(self, dt);
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