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
		char* symbolPos = strchr(temp, '=');
		char* val = symbolPos + 1;
		*symbolPos = '\0';
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
			self.ent.weight = atof(val);
		else if (!strcmp(temp, "hp"))
			self.currHP = self.maxHP = atof(val);
		else if (!strcmp(temp, "attackcd"))
			self.attackCD = self.currAttCD = atof(val) * 1000.0;
		else if (!strcmp(temp, "attackdur"))
			self.attackDur = self.currAttDur = atof(val) * 1000.0;
	}
	fclose(file);
}

void PlayerInput(Player& self)
{
	static const Uint8* kb_state = SDL_GetKeyboardState(NULL);
	self.ent.isMoving = false;
	if (kb_state[SDL_SCANCODE_A] + kb_state[SDL_SCANCODE_D] == 1)
	{
		self.ent.dir = -kb_state[SDL_SCANCODE_A] + kb_state[SDL_SCANCODE_D];
		self.ent.isMoving = true;
	}
	if (kb_state[SDL_SCANCODE_W] && !self.ent.isInAir)
	{
		self.ent.isInAir = true;
		self.ent.vVel = -self.ent.weight * 17;
	}
	if (kb_state[SDL_SCANCODE_F])
	{
		if (!self.pressedB.att)
		{
			self.pressedB.att = true;
			if (self.canAttack)
				self.isAttacking = true;
		}
	}
	else
	{
		self.pressedB.att = false;
	}
	if (kb_state[SDL_SCANCODE_Q])
	{
		if (!self.pressedB.thrw && self.projectilesCount)
			self.isThrowing = self.pressedB.thrw = true;
	}
	else
	{
		self.pressedB.thrw = false;
	}
}

void PlayerPlatformVerCollision(Player& self, Platform& platform)
{
	if (!platform.vCollCheck)
		return;
	if (self.ent.vVel < 0.0f ||
		self.ent.rect.y - platform.rect.y > self.ent.rect.h * -0.8 ||
		!SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	self.ent.isInAir = false;
	self.ent.vVel = 0;
	EntityMoveTo(self.ent, { (int)self.ent.pos.x, platform.rect.y - self.ent.rect.h });
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
	SDL_Point moveTo{ platform.rect.x, self.ent.rect.y };
	if (EntityGetHorMid(self.ent) >= intersection.x + intersection.w / 2)
		moveTo.x += platform.rect.w;
	else
		moveTo.x -= self.ent.rect.w;
	EntityMoveTo(self.ent, moveTo);
	return;
}

void PlayerAttack(Player& self)
{
	if (!self.isAttacking && self.canAttack)
		return;
	self.canAttack = false;
	if (self.isAttacking)
	{
		self.currAttDur -= FRAME_DELAY;
		if (self.currAttDur >= 0)
		{
			self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			self.attackBox.y = self.ent.pos.y;
		}
		else
		{
			self.isAttacking = false;
			self.currAttCD -= FRAME_DELAY;
		}
	}
	else 
	{
		self.currAttCD -= FRAME_DELAY;
		if (self.currAttCD <= 0)
		{
			self.currAttCD = self.attackCD;
			self.currAttDur = self.attackDur;
			self.canAttack = true;
		}
	}
}

void PlayerThrowProjectile(Player& self, Projectile& projectile)
{
	if (self.isAttacking)
		return;
	self.projectilesCount--;
	projectile.isPickable = self.isThrowing = false;
	projectile.wasThrown = projectile.ent.isMoving = true;
	projectile.ent.dir = self.ent.dir;
	projectile.ent.vVel = projectile.ent.weight * -5;
	SDL_Point moveTo{ self.ent.rect.x, self.ent.rect.y + self.ent.rect.h * 0.2 };
	if (self.ent.dir == 1)
		moveTo.x += self.ent.rect.w;
	else
		moveTo.x -= projectile.ent.rect.w;
	EntityMoveTo(projectile.ent, moveTo);
}

void PlayerProjectilesUpdate(Player& self)
{
	for (Projectile& projectile : self.projectiles)
	{
		if (self.isThrowing && !projectile.wasThrown)
		{
			PlayerThrowProjectile(self, projectile);
			continue;
		}
		ProjectileUpdate(projectile);
		if (SDL_HasIntersection(&self.ent.rect, &projectile.ent.rect) 
			&& projectile.isPickable
			&& self.projectilesCount < 5)
		{
			ProjectilePickUp(projectile);		
			self.projectilesCount++;
		}
	}
}

void PlayerAttackPenalty(Player& self, float duration)
{
	duration *= 1000.0;
	self.canAttack = self.isAttacking = false;
	self.currAttCD = self.attackCD;
	self.currAttCD = duration;
}

void PlayerUpdate(Player& self)
{
	PlayerInput(self);
	EntityUpdate(self.ent);
	PlayerAttack(self);
	PlayerProjectilesUpdate(self);
}

void PlayerDraw(const Player& self)
{
	SDL_SetRenderDrawColor(ren, 150, 0, 0, 255);
	SDL_RenderFillRect(ren, &self.ent.rect);
	if (self.isAttacking)
	{
		SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
		SDL_RenderFillRect(ren, &self.attackBox);
	}
}