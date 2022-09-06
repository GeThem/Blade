#include "Player.h"

void PlayerLoadCharacter(Player& self, const char* name)
{
	const Uint8 tempSize = 100;
	char temp[tempSize] = "data/characters/";
	strcat_s(temp, name);
	strcat_s(temp, "/stats.txt");

	FILE* file;
	if (fopen_s(&file, temp, "r"))
		exit(0);
	while (!feof(file))
	{
		fgets(temp, tempSize, file);
		char* val = temp;
		StrSplitInTwo(val, '=');

		if (!strcmp(temp, "sprite offset"))
		{
			self.spriteOffset.x = atoi(StrSplitInTwo(val, ' '));
			self.spriteOffset.y = atoi(val);
		}
		else if (!strcmp(temp, "hitbox"))
		{
			self.ent.rect.w = self.plungeRect.w = atoi(StrSplitInTwo(val, ' '));
			self.ent.rect.h = atoi(val);
			self.plungeRect.h = self.ent.rect.h * 1.2;
		}
		else if (!strcmp(temp, "ms"))
		{
			self.ent.maxMS = atof(val);
			self.ent.hVel = self.ent.maxMS / 12.0f;
		}
		else if (!strcmp(temp, "weight"))
			self.ent.vVel = atof(val);		
		else if (!strcmp(temp, "base dmg"))
			self.baseDmg = self.currDmg = atoi(val);
		else if (!strcmp(temp, "hp"))
			self.maxHP = atoi(val);
		else if (!strcmp(temp, "stamina"))
			self.maxStamina = atoi(val);
		else if (!strcmp(temp, "parry"))
		{
			self.parryDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.parryCD = atof(val) * 1000.0f;
		}
		else if (!strcmp(temp, "evade"))
		{
			self.evadeDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.evadeCD = atof(val) * 1000.0f;
		}
		else if (!strcmp(temp, "animations"))
		{
			int num = atoi(val);
			fgets(temp, tempSize, file);
			val = temp;
			for (int i = 0; i < num; i++)
			{
				self.anims[i].dur = atof(StrSplitInTwo(val, ' ')) * 1000;
			}
		}
		else if (!strcmp(temp, "atks"))
		{
			self.numberOfAttacks = atoi(val);
			self.atks = (Attack*)malloc(sizeof(Attack) * self.numberOfAttacks);
			for (int i = 0; i < self.numberOfAttacks; i++)
			{
				fgets(temp, tempSize, file);
				val = temp;
				self.atks[i].dmg = atof(StrSplitInTwo(val, ' ')) / 100.0f;
				self.atks[i].dir = atof(StrSplitInTwo(val, ' '));
				self.atks[i].hitboxOffset.x = atoi(StrSplitInTwo(val, ' '));
				self.atks[i].hitboxOffset.y = atoi(StrSplitInTwo(val, ' '));
				self.atks[i].hitbox.w = atoi(StrSplitInTwo(val, ' '));
				self.atks[i].hitbox.h = atoi(StrSplitInTwo(val, ' '));
				self.atks[i].delay = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
				self.atks[i].dur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
				self.atks[i].postAtkDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
				self.atks[i].CD = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
				self.atks[i].stunDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
				self.atks[i].staminaCost = atoi(StrSplitInTwo(val, ' '));
				self.atks[i].dur += self.atks[i].delay + self.atks[i].postAtkDur;
				self.atks[i].sprite.dur = self.atks[i].dur;
			}
			PlayerResetAttacks(self, self.numberOfAttacks);
		}
		else if (!strcmp(temp, "plunge atk") || !strcmp(temp, "charge atk"))
		{
			int i = strstr(temp, "plunge") ? 1 : 0;
			fgets(temp, tempSize, file);
			val = temp;
			self.chargeAtk[i].atk.dmg = atof(StrSplitInTwo(val, ' ')) / 100.0f;
			self.chargeAtk[i].atk.dir = atof(StrSplitInTwo(val, ' '));
			self.chargeAtk[i].atk.hitboxOffset.x = atoi(StrSplitInTwo(val, ' '));
			self.chargeAtk[i].atk.hitboxOffset.y = atoi(StrSplitInTwo(val, ' '));
			self.chargeAtk[i].atk.hitbox.w = atoi(StrSplitInTwo(val, ' '));
			self.chargeAtk[i].atk.hitbox.h = atoi(StrSplitInTwo(val, ' '));
			if (i == 0)
				self.chargeAtk[i].atk.delay = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			else
				self.chargeAtk[i].atk.delay = 0;
			self.chargeAtk[i].atk.dur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].atk.postAtkDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].atk.CD = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].atk.stunDur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].atk.staminaCost = atoi(StrSplitInTwo(val, ' '));
			self.chargeAtk[i].preChrgTime = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].chrgSprite.dur = atof(StrSplitInTwo(val, ' ')) * 1000.0f;
			self.chargeAtk[i].atk.dur += self.chargeAtk[i].atk.delay + self.chargeAtk[i].atk.postAtkDur;
			self.chargeAtk[i].atk.sprite.dur = self.chargeAtk[i].atk.dur;
			PlayerResetChargeAttack(self, 0);
			PlayerResetChargeAttack(self, 1);
		}
	}
	fclose(file);
	PlayerLoadCharacterSprites(self, name);
}

void AnimationLoad(AnimatedSprite& anim, const char* path)
{
	anim.image = ImageLoad(path);
	anim.frameWdt = anim.image.rect.h;
	anim.frameCount = anim.image.rect.w / anim.frameWdt;
	anim.frameTime = ceilf(float(anim.dur) / anim.frameCount);
	anim.currFrame = { 0, 0, anim.frameWdt, anim.frameWdt };
	anim.timeElapsed = 0;
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
	for (int i = 0; i < 2; i++)
	{
		char buffer[14];
		strcpy_s(buffer, i == 0 ? "Charge.png" : "Plunge.png");
		strcat_s(path, buffer);
		AnimationLoad(self.chargeAtk[i].chrgSprite, path);
		StrReplace(path, buffer, "");
		strcpy_s(buffer, i == 0 ? "ChargeAtk.png" : "PlungeAtk.png");
		strcat_s(path, buffer);
		AnimationLoad(self.chargeAtk[i].atk.sprite, path);
		StrReplace(path, buffer, "");
	}
	strcat_s(path, "*.png");
	char files[][10] = { "Idle", "Run", "Jump", "Fall", "Take hit", "Death", "Parry"};
	for (int i = IDLE; i < 7; i++)
	{
		StrReplace(path, "*", files[i]);
		AnimationLoad(self.anims[i], path);
		StrReplace(path, files[i], "*");
	}
	self.currSprite = self.anims + IDLE;
}

void PlayerReboot(Player& self)
{
	self.dOrder = FOREGROUND;
	self.currHP = self.maxHP;
	self.currStamina = self.maxStamina;
	self.currStaminaCD = 0;
	self.currEvadeDur = self.evadeDur;
	self.currEvadeCD = self.evadeCD;
	for (int i = IDLE; i < 6; i++)
	{
		self.anims[i].timeElapsed = 0;
	}
	PlayerResetChargeAttack(self, 0);
	PlayerResetChargeAttack(self, 1);
	PlayerResetAttacks(self, self.numberOfAttacks);
	self.currAtk = self.atks;
	self.currAtkIndex = 0;
	strcpy_s(self.status, "idle");
	self.currParrCD = self.parryCD;
	self.currParrDur = self.parryDur;
	self.isBusy = false;
	self.isDismounting = false;
	self.canAttack = self.canEvade = self.canParry = true;
	self.ent.currMS = self.ent.verMS = 0;
	self.ent.isInAir = self.ent.isMoving = false;
	self.ent.dir = 1;
}

void PlayerNextAttack(Player& self)
{
	if (strstr(self.status, "postAttack"))
	{
		self.currAtkIndex = (self.currAtkIndex + 1) % self.numberOfAttacks;
		self.currAtk = &self.atks[self.currAtkIndex];
		return;
	}
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	self.currAtkIndex = 0;
	self.currAtk = self.atks;
}

void PlayerInput(Player& self)
{
	if (self.isDisabled)
		return;

	if (!self.isBusy || strstr(self.status, "plunge") && !strstr(self.status, "Atk"))
		self.isDismounting = kb.state[self.ctrls.dismount];

	if (!self.isBusy || strstr(self.status, "evade"))
	{
		if (!self.canMove && !KeyHold(self.ctrls.left) && !KeyHold(self.ctrls.right))
			self.canMove = true;
		if (self.canMove)
		{
			self.ent.isMoving = kb.state[self.ctrls.left] + kb.state[self.ctrls.right] == 1;
			if (self.ent.isMoving)
			{
				if (strstr(self.status, "postAttack"))
				{
					StrReplace(self.status, "postAttack", "");
					PlayerAttackPenalty(self, 0.14);
				}
				self.ent.dir = -kb.state[self.ctrls.left] + kb.state[self.ctrls.right];
				StrReplace(self.status, "idle", "");
				if (!strstr(self.status, "jump") && !strstr(self.status, "fall") && !strstr(self.status, "run"))
					strcat_s(self.status, "run");
			}
			else
				StrReplace(self.status, "run", "idle");
		}
		if (OnKeyPress(self.ctrls.jump) && !self.ent.isInAir)
		{
			self.ent.verMS = -self.ent.vVel * 17;
			self.ent.isInAir = true;
			StrReplace(self.status, "run", "jump");
			StrReplace(self.status, "idle", "jump");
			StrReplace(self.status, "postAttack", "jump");
			self.canMove = true;
		}
	}
	if (self.isBusy)
	{
		if (strstr(self.status, "attack") && !self.isDealingDmg &&
			(OnKeyPress(self.ctrls.right) || OnKeyPress(self.ctrls.left) || OnKeyPress(self.ctrls.jump)))
		{
			StrReplace(self.status, "attack", "idle");
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			self.isBusy = false;
			self.canMove = true;
			return PlayerInput(self);
		}
		if (strstr(self.status, "chrg"))
		{
			self.isHoldingAtk = KeyHold(self.ctrls.chargeAtk);
		}
		if (strstr(self.status, "parry"))
		{
			self.isHoldingParry = KeyHold(self.ctrls.parry);
		}
		return;
	}
	if (OnKeyPress(self.ctrls.chargeAtk) && self.currStamina > 0)
	{
		if (!self.ent.isInAir && self.canCharge)
		{
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			PlayerResetChargeAttack(self, 0);
			self.currAtk = &self.chargeAtk[0].atk;
			self.ent.currMS = 0;
			self.isDismounting = false;
			self.isBusy = self.isHoldingAtk = true;
			self.ent.isMoving = self.canCharge = self.canMove = false;
			strcpy_s(self.status, "chrg");
			return;
		}
		if (self.ent.isInAir && self.canPlunge)
		{
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			PlayerResetChargeAttack(self, 1);
			self.currAtk = &self.chargeAtk[1].atk;
			PlayerDecreaseStamina(self, self.currAtk->staminaCost);
			self.ent.currMS = 0;
			self.ent.verMS = -self.ent.vVel;
			self.isBusy = true;
			self.ent.isMoving = self.canCharge = self.canMove = false;
			strcpy_s(self.status, "plunge");
			return;
		}
	}
	if (OnKeyPress(self.ctrls.attack) && self.canAttack
		&& self.currStamina > 0)
	{
		if (!self.ent.isInAir)
			self.ent.currMS = 0;
		self.currAtk->sprite.timeElapsed = 0;
		PlayerNextAttack(self);
		PlayerDecreaseStamina(self, self.currAtk->staminaCost);
		self.isBusy = true;
		self.canAttack = self.ent.isMoving = self.canMove = false;
		strcpy_s(self.status, "attack");
		return;
	}
	if (kb.state[self.ctrls.parry] && self.canParry && self.currStamina > 0)
	{
		self.isBusy = self.isHoldingParry = true;
		self.canParry = self.canMove = self.ent.isMoving = false;
		self.ent.currMS = 0;
		strcpy_s(self.status, "parry");
		return;
	}
	if (OnKeyPress(self.ctrls.evade) && self.canEvade)
	{
		self.isBusy = true;
		self.canEvade = false;
		strcat_s(self.status, "_evade");
		return;
	}
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
	if (platform.isDismountable)
		self.dOrder = PLATFORMS;
	self.ent.isInAir = false;
	self.ent.verMS = 0;
	StrReplace(self.status, "jump", "idle");
	StrReplace(self.status, "fall", "idle");
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
	if (strstr(self.status, "run"))
		StrReplace(self.status, "run", "idle");
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
		self.atks[i].sprite.timeElapsed = 0;
	}
	self.canAttack = self.canDealDmg = true;
	self.isDealingDmg = false;
}

void PlayerResetChargeAttack(Player& self, int type)
{
	self.chargeAtk[type].atk.currCD = self.chargeAtk[type].atk.CD;
	self.chargeAtk[type].atk.currDur = self.chargeAtk[type].atk.dur;
	self.chargeAtk[type].atk.sprite.timeElapsed = self.chargeAtk[type].chargeTime = 0;
	self.chargeAtk[type].chrgSprite.timeElapsed = 0;
	self.canCharge = self.canDealDmg = true;
	self.isDealingDmg = false;
}

bool PlayerProcessAttack(Player& self, Attack& atk, Uint16 dt)
{
	atk.currDur -= dt;
	if (atk.currDur >= atk.postAtkDur)
	{
		if (atk.dur - atk.currDur > atk.delay)
		{
			self.isDealingDmg = true;
			atk.hitbox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w - atk.hitboxOffset.x : -atk.hitbox.w + atk.hitboxOffset.x);
			atk.hitbox.y = self.ent.pos.y - atk.hitboxOffset.y;
		}
		return false;
	}
	self.canDealDmg = true;
	self.isBusy = self.isDealingDmg = false;
	return true;
}

bool PlayerProcessChargeAttack(Player& self, Uint16 dt)
{
	if (self.isHoldingAtk && !strstr(self.status, "Atk") || self.chargeAtk[0].chargeTime < MIN_CHARGE_TIME)
	{
		self.chargeAtk[0].chargeTime += dt;
		return false;
	}
	if (!strstr(self.status, "chrgAtk") && !strstr(self.status, "chrgPostAtk"))
		StrReplace(self.status, "chrg", "chrgAtk");
	if (!strstr(self.status, "chrgPostAtk"))
	{
		if (PlayerProcessAttack(self, self.chargeAtk[0].atk, dt))
		{
			PlayerDecreaseStamina(self, self.currAtk->staminaCost);
			StrReplace(self.status, "chrgAtk", "chrgPostAtk");
			self.isBusy = true;
		}
		return false;
	}
	self.currAtk->currDur -= dt;
	if (self.currAtk->currDur <= 0)
	{
		self.canMove = true;
		self.isBusy = false;
		return true;
	}
	return false;
}

bool PlayerProcessPlungeAttack(Player& self, Uint16 dt)
{
	ChargeAtk& atk = self.chargeAtk[1];
	if (atk.chrgSprite.timeElapsed < atk.preChrgTime)
	{
		self.ent.verMS = -self.ent.vVel;
		return false;
	}
	if (atk.chargeTime == 0)
	{
		self.ent.verMS = 10;
	}
	atk.chargeTime += dt;
	if (self.ent.verMS > 0)
	{
		atk.chargeTime += dt;
		self.ent.verMS = self.ent.vVel * 20;
		return false;
	}
	if (!strstr(self.status, "plungeAtk") && !strstr(self.status, "plungePostAtk"))
		StrReplace(self.status, "plunge", "plungeAtk");
	if (!strstr(self.status, "plungePostAtk"))
	{
		if (PlayerProcessAttack(self, atk.atk, dt))
		{
			StrReplace(self.status, "plungeAtk", "plungePostAtk");
			self.isBusy = true;
		}
		return false;
	}
	self.currAtk->currDur -= dt;
	if (self.currAtk->currDur <= 0)
	{
		self.canMove = true;
		self.isBusy = false;
		return true;
	}
	return false;
}

void PlayerProcessParry(Player& self, Uint16 dt)
{
	if (self.isHoldingParry || self.parryDur - self.currParrDur <= MIN_PARRY_DUR)
	{
		self.currParrDur -= dt;
		if (self.currParrDur > 0)
			return;
	}
	self.isBusy = false;
	self.canMove = true;
	self.currParrDur = self.parryDur;
	PlayerDecreaseStamina(self, 0);
	StrReplace(self.status, "parry", "idle");
}

void PlayerProcessEvade(Player& self, Uint16 dt)
{
	self.currEvadeDur -= dt;
	if (self.currEvadeDur > 0)
		return;
	self.currEvadeDur = self.evadeDur;
	self.isBusy = false;
	StrReplace(self.status, "_evade", "");
}

void PlayerAttackPenalty(Player& self, float duration)
{
	duration *= 1000.0f;
	duration = max(self.currAtk->currCD, duration);
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	self.canAttack = self.isBusy = self.isDealingDmg = false;
	self.atks[0].currCD = duration;
	self.currAtkIndex = 0;
	self.currAtk = self.atks;
	StrReplace(self.status, "attack", "");
}

bool PlayerAttackCooldown(Player& self, Attack& atk, Uint16 dt)
{
	atk.currCD -= dt;
	if (atk.currCD > 0)
	{
		return false;
	}
	atk.currCD = 0;
	return true;
}

void PlayerCancelParry(Player& self)
{
	StrReplace(self.status, "parry", "idle");
	self.canMove = true;
	self.currParrDur = 0;
	self.isBusy = false;
}

void PlayerParryCooldown(Player& self, Uint16 dt)
{
	if (strstr(self.status, "parry"))
		return;
	self.currParrCD -= dt;
	if (self.currParrCD > 0)
		return;
	self.currParrCD = self.parryCD;
	self.canParry = true;
}

void PlayerEvadeCooldown(Player& self, Uint16 dt)
{
	if (strstr(self.status, "evade"))
		return;
	self.currEvadeCD -= dt;
	if (self.currEvadeCD > 0)
		return;
	self.currEvadeCD = self.evadeCD;
	self.canEvade = true;
}

void PlayerDisable(Player& self, float dur)
{
	dur *= 1000;
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	self.anims[HIT].timeElapsed = 0;
	strcpy_s(self.status, "hit");
	self.isDisabled = true;
	self.ent.isMoving = self.isBusy = false;
	self.disableDur = max(dur, self.disableDur);
}

void PlayerDisableElapse(Player& self, Uint16 dt)
{
	self.disableDur -= dt;
	if (self.disableDur <= 0)
	{
		self.disableDur = 0;
		self.isDisabled = false;
		StrReplace(self.status, "hit", "idle");
		self.canMove = true;
	}
}

void PlayerDecreaseStamina(Player& self, float amount)
{
	self.currStamina -= amount;
	if (self.currStamina < 0)
	{
		self.currStaminaCD = STAMINA_RECHARGE_CD * 2;
		self.currStamina = 0;
	}
	else
		self.currStaminaCD = STAMINA_RECHARGE_CD;
}

void PlayerStaminaRecharge(Player& self, Uint16 dt)
{
	self.currStaminaCD -= dt;
	if (self.currStaminaCD <= 0)
	{
		self.currStaminaCD = 0;
		self.currStamina = min(self.maxStamina, self.currStamina + STAMINA_RECHARGE_RATE);
	}
}

void PlayerPhysics(Player& self)
{
	EntityUpdate(self.ent);
	self.plungeRect.x = self.ent.rect.x;
	self.plungeRect.y = self.ent.rect.y + self.ent.rect.h;
}

void PlayerUpdate(Player& self, Uint16 dt)
{
	self.currDmg = self.baseDmg;

	if (self.currHP <= 0)
	{
		self.ent.isMoving = false;
		strcpy_s(self.status, "dead");
		return;
	}
	if (!self.isBusy)
		PlayerStaminaRecharge(self, dt);
	if (self.isDisabled)
		PlayerDisableElapse(self, dt);
	else if (self.isBusy)
	{
		if (strstr(self.status, "attack"))
		{
			if (PlayerProcessAttack(self, *self.currAtk, dt))
				StrReplace(self.status, "attack", "postAttack");
		}
		else if (strstr(self.status, "chrg"))
		{
			if (PlayerProcessChargeAttack(self, dt))
			{
				StrReplace(self.status, "chrgPostAtk", "idle");
			}
		}
		else if (strstr(self.status, "plunge"))
		{
			if (PlayerProcessPlungeAttack(self, dt))
			{
				StrReplace(self.status, "plungePostAtk", "idle");
			}
		}
		else if (strstr(self.status, "parry"))
			PlayerProcessParry(self, dt);
		else if (strstr(self.status, "evade"))
			PlayerProcessEvade(self, dt);
	}

	if (!strstr(self.status, "attack"))
	{
		if (strstr(self.status, "postAttack"))
		{
			self.currAtk->currDur -= dt;
			if (self.currAtk->currDur <= 0)
			{
				StrReplace(self.status, "postAttack", "idle");
				self.canMove = true;
			}
		}
		self.canAttack = PlayerAttackCooldown(self, self.atks[self.currAtkIndex], dt);
	}
	if (!strstr(self.status, "chrg"))
	{
		self.canCharge = PlayerAttackCooldown(self, self.chargeAtk[0].atk, dt);
	}
	PlayerParryCooldown(self, dt);
	PlayerEvadeCooldown(self, dt);
	if (!self.isBusy && self.ent.isInAir && !strstr(self.status, "fall") && !strstr(self.status, "hit"))
	{
		if (self.ent.verMS > 0 && !strstr(self.status, "jump"))
		{
			StrReplace(self.status, "run", "jump");
			StrReplace(self.status, "idle", "jump");
		}
		if (self.ent.verMS > 20)
		{
			StrReplace(self.status, "jump", "fall");
			return;
		}
	}
}

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font* font, int size, const SDL_Color& color, TTF_Font* outline)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.25, 0.09, 0.6, outline);
	VanishTextSetPos(txt, { EntityGetHorMid(self.ent) + RandInt(-self.ent.rect.w * 0.8, self.ent.rect.w * 0.8),
		EntityGetVerMid(self.ent) - 20 + RandInt(-40, 20) });
	return txt;
}

VanishText PlayerAttack(Player& self, Player& target, TTF_Font* font, TTF_Font* outline)
{
	self.canDealDmg = false;
	bool isCrit = RandInt(1, 100) <= self.currCritRate;
	if (!isCrit)
		self.currCritRate += 2.5;
	else
		self.currCritRate = self.baseCritRate;
	int dmg = int(self.currAtk->dmg * self.currDmg) * (isCrit ? 2 : 1);
	Sint8 dir;
	if (self.currAtk->dir == BOTH)
		dir = RectGetHorMid(self.ent.rect) < EntityGetHorMid(target.ent) ? 1 : -1;
	else
		dir = self.currAtk->dir * self.ent.dir;
	float chrgTime = strstr(self.status, "attack") ? 1 :
		strstr(self.status, "chrg") ? 1 + (self.chargeAtk[0].chargeTime - MIN_CHARGE_TIME) / 1000.0f :
		1 + (self.chargeAtk[1].chargeTime - self.chargeAtk[1].preChrgTime) / 1000.0f;
	dmg *= min(2, chrgTime);
	int takenDmg = PlayerTakeHit(target, dmg,
		self.currAtk->stunDur + self.chargeAtk[1].preChrgTime * bool(strstr(self.status, "plunge")),
		dir, self.baseDmg * 0.054);
	char buffer[10];
	sprintf_s(buffer, "%i", dmg);
	return PlayerSpawnText(target, buffer, font, RandInt(21, 25) + 10 * isCrit, { 230, 230, 230 }, outline);
}

int PlayerTakeHit(Player& self, int dmg, int stunDur, int dir, int pushPower)
{
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	strcpy_s(self.status, "hit");
	self.ent.dir = -dir;
	self.ent.currMS = dir * pushPower;
	self.anims[HIT].timeElapsed = 0;
	self.isDisabled = true;
	self.disableDur = max(stunDur, self.disableDur);
	self.ent.isMoving = self.isBusy = false;
	int takenDmg = dmg;
	self.currHP -= takenDmg;
	return takenDmg;
}

void PlayerDraw(const Player& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(
		{
			self.ent.rect.x - self.spriteOffset.x, self.ent.rect.y - self.spriteOffset.y,
			self.currSprite->frameWdt, self.currSprite->frameWdt
		}
	);
	//SDL_Rect hb = RectTransformForCurrWin(self.ent.rect);
	//SDL_RenderFillRect(ren, &hb);
	SDL_SetTextureAlphaMod(self.currSprite->image.texture, strstr(self.status, "evade") ? 100 : 255);
	SDL_RenderCopyEx(ren, self.currSprite->image.texture, &self.currSprite->currFrame, &drawRect,
		0, NULL, self.currSprite->flip);
	if (self.isDealingDmg)
	{
		SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
		drawRect = RectTransformForCurrWin(self.atks[self.currAtkIndex].hitbox);
		SDL_RenderFillRect(ren, &drawRect);
	}
	SDL_SetRenderDrawColor(ren, 200, 50, 50, 255);
	drawRect = RectTransformForCurrWin(self.hpBar);
	SDL_RenderFillRect(ren, &drawRect);
	SDL_SetRenderDrawColor(ren, 235, 223, 2, 255);
	drawRect = RectTransformForCurrWin(self.staminaBar);
	SDL_RenderFillRect(ren, &drawRect);

	SDL_SetRenderDrawColor(ren, 20, 20, 20, 255);
	drawRect = RectTransformForCurrWin({ RectGetHorMid(self.ent.rect) - 9, self.ent.rect.y - 32, 18, 18 });
	SDL_RenderFillRect(ren, &drawRect);
	SDL_SetRenderDrawColor(ren, self.color.r, self.color.g, self.color.b, 255);
	drawRect = RectTransformForCurrWin({ RectGetHorMid(self.ent.rect) - 7, self.ent.rect.y - 30, 14, 14 });
	SDL_RenderFillRect(ren, &drawRect);
}

void PlayerAnimate(Player& self, Uint16 dt)
{
	SDL_RendererFlip flip = self.ent.dir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	if (true)
	{
		AnimatedSprite* anim = self.anims + FALL;
		if (strstr(self.status, "ttack") || strstr(self.status, "Atk"))
		{
			anim = &self.currAtk->sprite;
		}
		else if (strstr(self.status, "idle"))
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
		else if (strstr(self.status, "parry"))
			anim = self.anims + PARRY;
		else if (strstr(self.status, "chrg") || strstr(self.status, "plunge"))
		{
			int i = strstr(self.status, "chrg") ? 0 : 1;
			anim = &self.chargeAtk[i].chrgSprite;
			if (anim->timeElapsed + dt >= anim->dur)
			{
				anim->timeElapsed = self.chargeAtk[i].preChrgTime - dt;
			}
		}
		else if (strstr(self.status, "dead"))
		{
			anim = self.anims + DEATH;
			if (anim->timeElapsed + dt >= anim->dur)
			{
				anim->timeElapsed = anim->dur - dt - 1;
			}
		}
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
		ImageDestroy(anim.image);
	for (int i = 0; i < self.numberOfAttacks; i++)
		ImageDestroy(self.atks[i].sprite.image);
	for (ChargeAtk& atk : self.chargeAtk)
	{
		ImageDestroy(atk.chrgSprite.image);
		ImageDestroy(atk.atk.sprite.image);
	}
	free(self.atks);
}