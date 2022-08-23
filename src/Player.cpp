#include "Player.h"

void PlayerLoadCharacter(Player& self, const char* filename)
{
	char temp[40] = "data/characters/";
	strcat_s(temp, filename);
	
	self.atks = (Attack*)malloc(sizeof(Attack));
	
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
			self.ent.hVel = self.ent.maxMS / 20.0f;
		}
		else if (!strcmp(temp, "weight"))
			self.ent.vVel = atof(val);
		else if (!strcmp(temp, "hp"))
			self.maxHP = atof(val);
		else if (!strcmp(temp, "projectile spd"))
			self.projBaseSpd = atof(val);
		else if (!strcmp(temp, "atk cd"))
			self.atks[0].CD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "full atk dur"))
			self.atks[0].dur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parry cd"))
			self.parryCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "parry dur"))
			self.parryDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evade cd"))
			self.evadeCD = atof(val) * 1000.0f;
		else if (!strcmp(temp, "evade dur"))
			self.evadeDur = atof(val) * 1000.0f;
		else if (!strcmp(temp, "atk delay"))
			self.atks[0].delay = atof(val) * 1000.0f;
		else if (!strcmp(temp, "atk"))
			self.atks[0].dmg = atof(val);
	}

	self.chargeAtk.CD = 0.5 * 1000;
	self.chargeAtk.dur = 0.3 * 1000;
	self.chargeAtk.delay = 0.1 * 1000;
	self.chargeAtk.dmg = 20;

	fclose(file);
}

void PlayerReboot(Player& self)
{
	self.currHP = self.maxHP;
	self.currEvadeDur = self.evadeDur;
	self.currEvadeCD = self.evadeCD;
	PlayerResetChargeAttack(self);
	PlayerResetAttacks(self, self.numberOfAttacks);
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
		projectile.wasThrown = false;
		projectile.ent.pos = { 0, 0 };
		projectile.ent.verMS = projectile.ent.currMS = 0;
		projectile.ent.rect = { 0, 0, 50, 50 };
		projectile.ent.vVel = 0.5;
		projectile.pickCD = projectile.currPickCD = 1000;
	}
}

void PlayerInput(Player& self)
{
	if (self.isDisabled)
		return;

	if (self.isBusy)
	{
		if (self.isChrgAttacking)
			self.isHoldingAtk = KeyHold(self.ctrls.chargeAtk);
		return;
	}

	self.ent.isMoving = kb.state[self.ctrls.left] + kb.state[self.ctrls.right] == 1;
	if (self.ent.isMoving)
	{
		self.ent.dir = -kb.state[self.ctrls.left] + kb.state[self.ctrls.right];
		//self.ent.dir < 0 ? replace(self.status, "right", "left") : replace(self.status, "left", "right");
	}
	if (OnKeyPress(self.ctrls.jump) && !self.ent.isInAir)
	{
		self.ent.verMS = -self.ent.vVel * 17;
		self.ent.isInAir = true;
		//strcat_s(self.status, "_jump");
	}
	if (OnKeyPress(self.ctrls.chargeAtk) && self.canAttack)
	{
		self.isBusy = self.isChrgAttacking = self.isHoldingAtk = true;
		self.canAttack = false;
		self.ent.isMoving = false;
		return;
	}
	if (OnKeyPress(self.ctrls.attack))
	{
		self.isBusy = self.isAttacking = true;
		self.canAttack = false;
		self.ent.isMoving = false;
		//strcat_s(self.status, "_attack");
		return;
	}
	if (OnKeyPress(self.ctrls.parry) && self.canParry)
	{
		self.isBusy = self.isParrying = true;
		self.canParry = false;
		return;
	}
	if (OnKeyPress(self.ctrls.evade) && self.canEvade)
	{
		self.isBusy = self.isEvading = true;
		self.canEvade = false;
		return;
	}
	self.pressedCtrls.thrw = OnKeyPress(self.ctrls.thrw);

	self.isDismounting = kb.state[self.ctrls.dismount];
}

void PlayerPlatformVerCollision(Player& self, Platform& platform)
{
	if (!platform.vCollCheck)
		return;
	if (self.ent.verMS < 0.0f || !SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	if (self.isDismounting && platform.isDismountable)
	{
		self.isDismounting = false;
		return;
	}
	if (int(self.ent.pos.y + self.ent.rect.h - self.ent.verMS) > platform.rect.y)
		return;
	self.ent.isInAir = false;
	self.ent.verMS = 0;
	StrReplace(self.status, "_jump", "");
	EntityMoveTo(self.ent, { self.ent.pos.x, (float)platform.rect.y - self.ent.rect.h });
	return;
}

void PlayerPlatformHorCollision(Player& self, Platform& platform)
{
	if (!platform.hCollCheck || !self.ent.currMS || !SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	SDL_FPoint moveTo{ (float)platform.rect.x, self.ent.pos.y };
	if (self.ent.currMS > 0)
	{
		if (int(self.ent.pos.x + self.ent.rect.w - self.ent.currMS) > platform.rect.x)
			return;
		moveTo.x -= self.ent.rect.w;
	}
	else
	{
		if (int(self.ent.pos.x - self.ent.currMS) < platform.rect.x + platform.rect.w)
			return;
		moveTo.x += platform.rect.w;
	}
	self.ent.currMS = 0;
	self.ent.isMoving = false;
	EntityMoveTo(self.ent, moveTo);
	return;
}

void PlayerResetAttacks(Player& self, int number)
{
	for (int i = 0; i < number; i++)
	{
		self.atks[i].currCD = self.atks[i].CD;
		self.atks[i].currDur = self.atks[i].dur;
	}
	self.canAttack = true;
	self.canDealDmg = true;
	self.isDealingDmg = false;
}

void PlayerResetChargeAttack(Player& self)
{
	self.chargeAtk.currCD = self.chargeAtk.CD;
	self.chargeAtk.currDur = self.chargeAtk.dur;
	self.chargeAtk.chargeTime = 0;
	self.canAttack = true;
	self.canDealDmg = true;
	self.isDealingDmg = false;
}

void PlayerProcessAttack(Player& self, Attack& atk, Uint16 dt)
{
	atk.currDur -= dt;
	if (atk.currDur > 0)
	{
		if (atk.dur - atk.currDur > atk.delay)
		{
			self.isDealingDmg = true;
			self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			self.attackBox.y = self.ent.pos.y;
		}
		if (!self.ent.isInAir && atk.currDur == atk.dur)
			self.ent.currMS = 0;
		return;
	}
	self.canDealDmg = true;
	self.isAttacking = self.isBusy = self.isDealingDmg = false;
	//StrReplace(self.status, "_attack", "");
}

void PlayerProcessChargeAttack(Player& self, Uint16 dt)
{
	chrgAtk& atk = self.chargeAtk;
	if (self.isHoldingAtk && atk.currDur == atk.dur)
	{
		atk.chargeTime += dt;
		return;
	}
	atk.currDur -= dt;
	if (atk.currDur > 0)
	{
		if (atk.dur - atk.currDur > atk.delay)
		{
			self.isDealingDmg = true;
			self.attackBox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w : -self.attackBox.w);
			self.attackBox.y = self.ent.pos.y;
		}
		if (!self.ent.isInAir && atk.currDur == atk.dur)
			self.ent.currMS = 0;
		return;
	}
	self.canDealDmg = true;
	self.isChrgAttacking = self.isBusy = self.isDealingDmg = false;
}

void PlayerProcessParry(Player& self, Uint16 dt)
{
	self.currParrDur -= dt;
	if (self.currParrDur > 0)
		return;
	self.isParrying = self.isBusy = false;
}

void PlayerProcessEvade(Player& self, Uint16 dt)
{
	self.currEvadeDur -= dt;
	if (self.currEvadeDur > 0)
		return;
	self.isEvading = self.isBusy = false;
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
	self.canAttack = self.isAttacking = self.isBusy = self.isDealingDmg = false;
	self.atks[0].currDur = 0;
	self.atks[0].currCD = duration;
	//replace(self.status, "_attack", "");
}

bool PlayerAttackCooldown(Player& self, Attack& atk, Uint16 dt)
{
	if (atk.currDur > 0)
		return false;
	atk.currCD -= dt;
	if (atk.currCD > 0)
		return false;
	atk.currCD = atk.CD;
	atk.currDur = atk.dur;
	self.canAttack = true;
	return true;
}

void PlayerChrgAttackCooldown(Player& self, Uint16 dt)
{
	chrgAtk& atk = self.chargeAtk;
	if (atk.currDur > 0)
		return;
	atk.currCD -= dt;
	if (atk.currCD > 0)
		return;
	atk.currCD = atk.CD;
	atk.currDur = atk.dur;
	atk.chargeTime = 0;
	self.canAttack = true;
	return;
}

void PlayerParryCooldown(Player& self, Uint16 dt)
{
	if (self.currParrDur > 0)
		return;
	self.currParrCD -= dt;
	if (self.currParrCD > 0)
		return;
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.canParry = true;
}

void PlayerEvadeCooldown(Player& self, Uint16 dt)
{
	if (self.currEvadeDur > 0)
		return;
	self.currEvadeCD -= dt;
	if (self.currEvadeCD > 0)
		return;
	self.currEvadeCD = self.evadeCD;
	self.currEvadeDur = self.evadeDur;
	self.canEvade = true;
}

void PlayerUpdate(Player& self, Uint16 dt)
{
	if (!self.isDisabled && self.isBusy)
	{
		if (self.isAttacking)
			PlayerProcessAttack(self, self.atks[self.currAtk], dt);
		else if (self.isChrgAttacking)
			PlayerProcessChargeAttack(self, dt);
		else if (self.isParrying)
			PlayerProcessParry(self, dt);
		else if (self.isEvading)
			PlayerProcessEvade(self, dt);
	}
	PlayerProcessProjectiles(self, dt);
	
	if (PlayerAttackCooldown(self, self.atks[self.currAtk], dt))
		self.currAtk = (self.currAtk + 1) % self.numberOfAttacks;
	PlayerChrgAttackCooldown(self, dt);
	PlayerParryCooldown(self, dt);
	PlayerEvadeCooldown(self, dt);
}

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font* font, int size, const SDL_Color& color)
{
	TTF_SetFontSize(font, size);
	VanishText txt = VanishTextGenerate(
		text, font, { 200, 200, 200 },
		RandFloat(0.5, 0.7), RandFloat(-10, -12), RandFloat(-7, 7), 0, 0.3, 0.3
	);
	VanishTextSetPos(
		txt,
		{ (int)EntityGetHorMid(self.ent) - txt.txtImg.rect.w / 2 + RandInt(-10, 10),
		int(EntityGetVerMid(self.ent) - 100 * scale) + RandInt(-10, 10) }
	);

	return txt;
}

int PlayerTakeHit(Player& self, int dmg, bool blockable)
{
	int takenDmg = dmg;
	self.currHP -= takenDmg;
	return takenDmg;
}

void PlayerDraw(const Player& self)
{
	if (self.isParrying)
		SDL_SetRenderDrawColor(ren, 200, 200, 0, 255);
	else
		SDL_SetRenderDrawColor(ren, self.color.r, self.color.g, self.color.b, 255);
	SDL_Rect drawRect;
	if (!self.isEvading)
	{
		drawRect = RectTransformForCurrWin(self.ent.rect);
		SDL_RenderFillRect(ren, &drawRect);
	}
	
	if (self.isDealingDmg)
	{
		SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
		drawRect = RectTransformForCurrWin(self.attackBox);
		SDL_RenderFillRect(ren, &drawRect);
	}
	SDL_SetRenderDrawColor(ren, 200, 50, 50, 255);
	drawRect = RectTransformForCurrWin(self.hpRect);
	SDL_RenderFillRect(ren, &drawRect);
}