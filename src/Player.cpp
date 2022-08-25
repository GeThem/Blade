#include "Player.h"

void PlayerLoadCharacter(Player& self, const char* filename)
{
	char temp[100] = "data/characters/";
	strcat_s(temp, filename);
	strcat_s(temp, ".txt");

	FILE* file;
	if (fopen_s(&file, temp, "r"))
		exit(0);
	while (!feof(file))
	{
		fgets(temp, 100, file);
		char* val = StrSplitInTwo(temp, '=');

		if (!strcmp(temp, "sprite offset"))
		{
			self.spriteOffset.y = atoi(StrSplitInTwo(val, ' '));
			self.spriteOffset.x = atoi(val);
		}
		else if (!strcmp(temp, "hitbox"))
		{
			self.ent.rect.h = atoi(StrSplitInTwo(val, ' '));
			self.ent.rect.w = atoi(val);
		}
		else if (!strcmp(temp, "ms"))
		{
			self.ent.maxMS = atof(val);
			self.ent.hVel = self.ent.maxMS / 12.0f;
		}
		else if (!strcmp(temp, "weight"))
			self.ent.vVel = atof(val);
		else if (!strcmp(temp, "hp"))
			self.maxHP = atof(val);
		else if (!strcmp(temp, "projectile spd"))
			self.projBaseSpd = atof(val);
		else if (!strcmp(temp, "parry"))
		{
			self.parryCD = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.parryDur = atof(val) * 1000.0f;
		}
		else if (!strcmp(temp, "evade"))
		{
			self.evadeCD = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.evadeDur = atof(val) * 1000.0f;
		}
		else if (!strcmp(temp, "animations"))
		{
			int num = atoi(val);
			fgets(temp, 60, file);
			val = temp;
			for (int i = 0; i < num; i++)
			{
				char* val2 = StrSplitInTwo(val, ' ');
				self.anims[i].dur = atof(val) * 1000;
				val = val2;
			}
		}
		else if (!strcmp(temp, "atks"))
		{
			self.numberOfAttacks = atoi(val);
			self.atks = (Attack*)malloc(sizeof(Attack) * self.numberOfAttacks);
			for (int i = 0; i < self.numberOfAttacks; i++)
			{
				fgets(temp, 60, file);
				val = temp;
				char* val2 = StrSplitInTwo(val, ' ');
				self.atks[i].dmg = atof(val);
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].hitboxOffset.x = atoi(val);
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].hitboxOffset.y = atoi(val);
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].hitbox.w = atoi(val);
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].hitbox.h = atoi(val);
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].delay = atof(val) * 1000.0f;
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].dur = atof(val) * 1000.0f;
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].postAtkDur = atof(val) * 1000.0f;
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].CD = atof(val) * 1000.0f;
				val = val2;
				val2 = StrSplitInTwo(val, ' ');
				self.atks[i].stunDur = atof(val) * 1000.0f;
				self.atks[i].dur += self.atks[i].delay + self.atks[i].postAtkDur;
				self.atks[i].sprite.dur = self.atks[i].dur;
			}
			PlayerResetAttacks(self, self.numberOfAttacks);
		}
	}

	self.chargeAtk.CD = 0.5 * 1000;
	self.chargeAtk.dur = 0.3 * 1000;
	self.chargeAtk.delay = 0.1 * 1000;
	self.chargeAtk.dmg = 20;

	fclose(file);
}

void AnimationLoad(AnimatedSprite& anim, const char* path)
{
	anim.image = ImageLoad(path);
	anim.frameWdt = anim.frameHgt = anim.image.rect.h;
	anim.frameCount = anim.image.rect.w / anim.frameWdt;
	anim.frameTime = ceilf(float(anim.dur) / anim.frameCount);
	anim.currFrame = { 0, 0, anim.frameWdt, anim.frameHgt };
}

void PlayerLoadCharacterSprites(Player& self, const char* name)
{
	char path[100] = "data/characters/";
	strcat_s(path, name);
	strcat_s(path, "/Sprites/");
	for (int i = 0; i < self.numberOfAttacks; i++)
	{
		char buffer[14];
		sprintf_s(buffer, "Attack%i.png", i + 1);
		strcat_s(path, buffer);
		AnimationLoad(self.atks[i].sprite, path);
		StrReplace(path, buffer, "");
	}
	char files[][14] = { "Idle.png", "Run.png", "Jump.png", "Fall.png", "Take hit.png"};
	int currAnim = IDLE;
	for (const char* file : files)
	{
		strcat_s(path, file);
		AnimationLoad(self.anims[currAnim], path);
		StrReplace(path, file, "");
		currAnim++;
	}
	self.currSprite = self.anims + IDLE;
}

void PlayerReboot(Player& self)
{
	self.currHP = self.maxHP;
	self.currEvadeDur = self.evadeDur;
	self.currEvadeCD = self.evadeCD;
	PlayerResetChargeAttack(self);
	PlayerResetAttacks(self, self.numberOfAttacks);
	strcpy_s(self.status, "right");
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.isBusy = false;
	self.isThrowing = self.isDismounting = false;
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
	self.ent.dir < 0 ? StrReplace(self.status, "right", "left") : StrReplace(self.status, "left", "right");

	if (self.isDisabled)
		return;

	if (self.isBusy)
	{
		if (strstr(self.status, "_attack") && !self.isDealingDmg &&
			(OnKeyPress(self.ctrls.right) || OnKeyPress(self.ctrls.left)))
		{
			self.isBusy = false;
		}
		else
		{

			if (strstr(self.status, "chrgattack"))
			{
				self.isHoldingAtk = KeyHold(self.ctrls.chargeAtk);
			}
			return;
		}
	}

	if (OnKeyPress(self.ctrls.jump) && !self.ent.isInAir)
	{
		self.ent.verMS = -self.ent.vVel * 17;
		self.ent.isInAir = true;
		StrReplace(self.status, "_run", "");
		strcat_s(self.status, "_jump");
	}
	if (kb.state[self.ctrls.chargeAtk] && self.chargeAtk.currCD == self.chargeAtk.CD)
	{
		self.isBusy = self.isHoldingAtk = true;
		self.ent.isMoving = false;
		strcat_s(self.status, "_chrgattack");
		return;
	}
	if (OnKeyPress(self.ctrls.attack) && self.canAttack)
	{
		self.atks[self.currAtk].sprite.timeElapsed = 0;
		self.isBusy = true;
		self.canAttack = false;
		self.ent.isMoving = false;
		strcpy_s(self.status, self.ent.dir > 0 ? "right_attack" : "left_attack");
		return;
	}
	if (OnKeyPress(self.ctrls.parry) && self.canParry)
	{
		self.isBusy = true;
		self.canParry = false;
		strcat_s(self.status, "_parry");
		return;
	}
	if (OnKeyPress(self.ctrls.evade) && self.canEvade)
	{
		self.isBusy = true;
		self.canEvade = false;
		strcat_s(self.status, "_evade");
		return;
	}
	if ((!strstr(self.status, "postattack") || !KeyHold(self.ctrls.left) && !KeyHold(self.ctrls.right)))
	{
		self.ent.isMoving = kb.state[self.ctrls.left] + kb.state[self.ctrls.right] == 1;
			if (self.ent.isMoving)
			{
				StrReplace(self.status, "_attack", "");
				PlayerResetAttacks(self, self.numberOfAttacks);
				if (strstr(self.status, "_postattack"))
				{
					StrReplace(self.status, "_postattack", "");
					PlayerAttackPenalty(self, 1);
				}
				self.currAtk = 0;
				self.ent.dir = -kb.state[self.ctrls.left] + kb.state[self.ctrls.right];
				if (!self.ent.isInAir && !strstr(self.status, "run"))
					strcat_s(self.status, "_run");
			}
			else
				StrReplace(self.status, "_run", "");
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
	StrReplace(self.status, "_fall", "");
	EntityMoveTo(self.ent, { self.ent.pos.x, (float)platform.rect.y - self.ent.rect.h });
	return;
}

void PlayerPlatformHorCollision(Player& self, Platform& platform)
{
	self.ent.rect.w++;
	if (!platform.hCollCheck || !SDL_HasIntersection(&self.ent.rect, &platform.rect) || !self.ent.currMS)
	{
		self.ent.rect.w--;
		return;
	}
	self.ent.rect.w--;
	SDL_FPoint moveTo{ (float)platform.rect.x, self.ent.pos.y };
	if (self.ent.currMS > 0.0f)
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
	if (strstr(self.status, "_run"))
		StrReplace(self.status, "_run", "");
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
		self.atks[i].betweenHitsTime = 0;
		self.atks[i].sprite.timeElapsed = 0;
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
	self.canDealDmg = true;
	self.isDealingDmg = false;
}

void PlayerProcessAttack(Player& self, Attack& atk, Uint16 dt)
{
	StrReplace(self.status, "_postattack", "");
	atk.currDur -= dt;
	if (atk.currDur >= atk.postAtkDur)
	{
		if (atk.dur - atk.currDur > atk.delay)
		{
			self.isDealingDmg = true;
			atk.hitbox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w - atk.hitboxOffset.x : -atk.hitbox.w + atk.hitboxOffset.x);
			atk.hitbox.y = self.ent.pos.y - atk.hitboxOffset.y;
		}
		if (!self.ent.isInAir && atk.currDur == atk.dur)
			self.ent.currMS = 0;
		return;
	}
	self.canDealDmg = true;
	self.isBusy = self.isDealingDmg = false;
	StrReplace(self.status, "_attack", "_postattack");
	self.currAtk = (self.currAtk + 1) % self.numberOfAttacks;
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
	self.isBusy = self.isDealingDmg = false;
	StrReplace(self.status, "_chrgattack", "");
}

void PlayerProcessParry(Player& self, Uint16 dt)
{
	self.currParrDur -= dt;
	if (self.currParrDur > 0)
		return;
	self.isBusy = false;
	self.currParrDur = self.parryDur;
	StrReplace(self.status, "_parry", "");
}

void PlayerProcessEvade(Player& self, Uint16 dt)
{
	self.currEvadeDur -= dt;
	if (self.currEvadeDur > 0)
		return;
	self.isBusy = false;
	self.currEvadeDur = self.evadeDur;
	StrReplace(self.status, "_evade", "");
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
	PlayerResetAttacks(self, self.numberOfAttacks);
	self.canAttack = self.isBusy = self.isDealingDmg = false;
	self.atks[self.numberOfAttacks - 1].currCD = duration;
	self.atks[self.numberOfAttacks - 1].currDur = 0;
	self.currAtk = 0;
	StrReplace(self.status, "_attack", "");
}

bool PlayerAttackCooldown(Player& self, Attack& atk, Uint16 dt)
{
	if (atk.currDur >= atk.postAtkDur)
	{
		if (!strstr(self.status, "_attack"))
		{
			atk.betweenHitsTime += dt;
			if (atk.betweenHitsTime >= self.atks[((self.currAtk - 1) < 0 ? self.numberOfAttacks - 1 : self.currAtk - 1)].postAtkDur)
			{
				self.currAtk = 0;
				PlayerResetAttacks(self, self.numberOfAttacks);
				StrReplace(self.status, "_postattack", "");
				//PlayerAttackPenalty(self, 0.3);
			}
		}
		return false;
	}
	atk.currCD -= dt;
	if (atk.currCD > 0)
	{
		if (strstr(self.status, "postattack"))
		{
			atk.betweenHitsTime += dt;
			if (atk.betweenHitsTime >= self.atks[((self.currAtk - 1) < 0 ? self.numberOfAttacks - 1 : self.currAtk - 1)].postAtkDur)
			{
				self.currAtk = 0;
				PlayerResetAttacks(self, self.numberOfAttacks);
				StrReplace(self.status, "_postattack", "");
				//PlayerAttackPenalty(self, 0.3);
			}
		}
		return false;
	}
	atk.currDur = atk.dur;
	atk.betweenHitsTime = 0;
	atk.currCD = atk.CD;
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
	self.canEvade = true;
}

void PlayerDisableElapse(Player& self, Uint16 dt)
{
	self.disableDur -= dt;
	if (self.disableDur <= 0)
	{
		self.isDisabled = false;
		StrReplace(self.status, "_hit", "");
	}
}

void PlayerUpdate(Player& self, Uint16 dt)
{
	if (self.isDisabled)
	{
		PlayerDisableElapse(self, dt);
	}
	else if (self.isBusy)
	{
		if (strstr(self.status, "_attack"))
			PlayerProcessAttack(self, self.atks[self.currAtk], dt);
		else if (strstr(self.status, "chrgattack"))
			PlayerProcessChargeAttack(self, dt);
		else if (strstr(self.status, "parry"))
			PlayerProcessParry(self, dt);
		else if (strstr(self.status, "evade"))
			PlayerProcessEvade(self, dt);
	}
	PlayerProcessProjectiles(self, dt);
	PlayerAttackCooldown(self, self.atks[((self.currAtk - 1) < 0 ? self.numberOfAttacks - 1 : self.currAtk - 1)], dt);
	PlayerChrgAttackCooldown(self, dt);
	PlayerParryCooldown(self, dt);
	PlayerEvadeCooldown(self, dt);
	if (!self.isBusy && self.ent.isInAir && !strstr(self.status, "fall"))
	{
		if (self.ent.verMS > 0 && !strstr(self.status, "jump"))
		{
			StrReplace(self.status, "_run", "");
			strcat_s(self.status, "_jump");
		}
		
		if (self.ent.verMS > 20)
		{
			StrReplace(self.status, "_jump", "");
			strcat_s(self.status, "_fall");
			return;
		}
	}
}

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font* font, int size, const SDL_Color& color)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.15, 0.09, 0.7);
	VanishTextSetPos(txt, { (int)EntityGetHorMid(self.ent) + RandInt(-self.ent.rect.w * 0.8, self.ent.rect.w * 0.8),
		int(EntityGetVerMid(self.ent) - 20) + RandInt(-40, 20) });
	return txt;
}

int PlayerTakeHit(Player& self, Attack& atk, int dir)
{
	strcpy_s(self.status, self.ent.dir > 0 ? "right_hit" : "left_hit");
	self.ent.dir = -dir;
	self.ent.currMS = dir * atk.dmg * 0.3;
	self.anims[HIT].timeElapsed = 0;
	self.isDisabled = true;
	self.disableDur = max(atk.stunDur, self.disableDur);
	self.ent.isMoving = self.isBusy = false;
	int takenDmg = atk.dmg;
	self.currHP -= takenDmg;
	return takenDmg;
}

void PlayerDraw(const Player& self)
{
	if (strstr(self.status, "parry"))
		SDL_SetRenderDrawColor(ren, 200, 200, 0, 255);
	else
		SDL_SetRenderDrawColor(ren, self.color.r, self.color.g, self.color.b, 255);
	SDL_Rect drawRect;
	if (!strstr(self.status, "evade"))
	{
		//CHANGE IT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		drawRect = RectTransformForCurrWin(
			{
				self.ent.rect.x - self.spriteOffset.x, self.ent.rect.y - self.spriteOffset.y,
				self.currSprite->frameWdt, self.currSprite->frameHgt
			}
		);
		//SDL_Rect hb = RectTransformForCurrWin(self.ent.rect);
		//SDL_RenderFillRect(ren, &hb);
		SDL_RenderCopyEx(ren, self.currSprite->image.texture, &self.currSprite->currFrame, &drawRect,
			0, NULL, self.currSprite->flip);
	}
	//if (self.isDealingDmg)
	//{
	//	SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
	//	drawRect = RectTransformForCurrWin(self.atks[self.currAtk].hitbox);
	//	SDL_RenderFillRect(ren, &drawRect);
	//}
	SDL_SetRenderDrawColor(ren, 200, 50, 50, 255);
	drawRect = RectTransformForCurrWin(self.hpRect);
	SDL_RenderFillRect(ren, &drawRect);
}

void PlayerAnimate(Player& self, Uint16 dt)
{
	SDL_RendererFlip flip = strstr(self.status, "right") ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	if (true)
	{
		AnimatedSprite* anim = self.anims + FALL;
		if (strstr(self.status, "_attack"))
		{
			anim = &self.atks[self.currAtk].sprite;
		}
		else if (strstr(self.status, "postattack"))
		{
			anim = &self.atks[((self.currAtk - 1) < 0 ? self.numberOfAttacks - 1 : self.currAtk - 1)].sprite;
		}
		else if (!strchr(self.status, '_'))
		{
			anim = self.anims + IDLE;
		}
		else if (strstr(self.status, "run"))
		{
			dt *= abs(self.ent.currMS / self.ent.maxMS);
			anim = self.anims + RUN;
		}
		else if (strstr(self.status, "fall"))
			anim = self.anims + FALL;
		else if (strstr(self.status, "jump"))
			anim = self.anims + JUMP;
		else if (strstr(self.status, "hit"))
			anim = self.anims + HIT;

		anim->flip = flip;
		anim->timeElapsed = (anim->timeElapsed + dt) % anim->dur;
		anim->currFrame.x = (anim->timeElapsed / anim->frameTime) * anim->frameWdt;
		self.currSprite = anim;
		return;
	}
}

void PlayerClear(Player& self)
{
	for (AnimatedSprite& anim : self.anims)
	{
		ImageDestroy(anim.image);
	}
	for (int i = 0; i < self.numberOfAttacks; i++)
	{
		ImageDestroy(self.atks[i].sprite.image);
	}
	free(self.atks);
}