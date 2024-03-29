#include "Player.h"

Uint32 PlayerGetStatus(const Player& self)
{
	return self.status & ~ALLFLAGS;
}

void PlayerToggleStatus(Player& self, Status status)
{
	self.status ^= status;
}

void PlayerSetStatus(Player& self, Status status)
{
	Uint32 flags = self.status & ALLFLAGS;
	self.status = status | flags;
}

Uint32 GetAnim(Uint32 status)
{
	Uint32 index = 0;
	status &= ~ALLFLAGS;
	for (; status; status >>= 1)
		index++;
	return index;
}

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
		{
			self.maxStamina = atoi(StrSplitInTwo(val, ' '));
			self.stmRecharge = self.currStmRecharge = atof(val);
		}
		else if (!strcmp(temp, "crit"))
		{
			self.baseCritRate = self.currCritRate = atof(StrSplitInTwo(val, ' '));
			self.baseCritDmg = atof(val) / 100.0f;
		}
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

void PlayerReboot(Player& self, Sint8 dir)
{
	self.dOrder = FOREGROUND;
	self.currHP = self.maxHP;
	self.currStamina = self.maxStamina;
	self.currStaminaCD = self.currEvadeCD = self.currParrCD = self.disableDur = 0;
	self.currEvadeDur = self.evadeDur;
	self.currParrDur = self.parryDur;
	for (int i = 0; i < 7; i++)
	{
		self.anims[i].timeElapsed = 0;
	}
	PlayerResetChargeAttack(self, 0);
	PlayerResetChargeAttack(self, 1);
	PlayerResetAttacks(self, self.numberOfAttacks);
	self.currAtk = self.atks;
	self.status = IDLE;
	self.ent.currMS = self.ent.verMS = self.activeBonuses = self.currAtkIndex = 0;
	self.ent.isInAir = self.ent.isMoving = false;
	self.ent.dir = dir;
}

void PlayerNextAttack(Player& self)
{
	if (PlayerGetStatus(self) == POSTATTACK)
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
	Uint32 status = PlayerGetStatus(self);
	if (status & (DISABLED | DEAD))
		return;
	if (!(self.status & ISBUSY) || status == PLUNGE || self.status & EVADE)
	{
		if (!(self.status & CANDISMOUNT) && !KeyHold(self.ctrls.dismount))
			self.status |= CANDISMOUNT;
		if (self.status & CANDISMOUNT)
		{
			self.status &= ~ISDISMOUNTING;
			self.status |= kb.state[self.ctrls.dismount] ? ISDISMOUNTING : 0;
		}
	}

	if (!(self.status & ISBUSY) || self.status & EVADE)
	{
		if (!(self.status & CANMOVE) && !KeyHold(self.ctrls.left) && !KeyHold(self.ctrls.right))
			self.status |= CANMOVE;
		if (self.status & CANMOVE)
		{
			self.ent.isMoving = kb.state[self.ctrls.left] + kb.state[self.ctrls.right] == 1;
			if (self.ent.isMoving)
			{
				self.ent.dir = -kb.state[self.ctrls.left] + kb.state[self.ctrls.right];
				if (status == POSTATTACK)
				{
					PlayerSetStatus(self, RUN);
					PlayerAttackPenalty(self, 0.14);
				}
				else if (status == IDLE)
					PlayerSetStatus(self, RUN);
			}
			else
				self.status &= ~RUN;
		}
		if (OnKeyPress(self.ctrls.jump) && !self.ent.isInAir)
		{
			self.ent.verMS = -self.ent.vVel * 17;
			self.ent.isInAir = true;
			PlayerSetStatus(self, JUMP);
			self.status |= CANMOVE;
		}
	}
	if (self.status & ISBUSY)
	{
		if (status == ATTACK && !(self.status & ISDEALINGDMG) &&
			(OnKeyPress(self.ctrls.right) || OnKeyPress(self.ctrls.left) || OnKeyPress(self.ctrls.jump)))
		{
			PlayerSetStatus(self, IDLE);
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			self.status &= ~ISBUSY;
			self.status |= CANMOVE;
			return PlayerInput(self);
		}
		if (status == CHRG)
		{
			self.status &= ~ISHOLDINGATK;
			self.status |= KeyHold(self.ctrls.chargeAtk) ? ISHOLDINGATK : 0;
			return;
		}
		if (status == PARRY)
		{
			self.status &= ~ISHOLDINGPARRY;
			self.status |= KeyHold(self.ctrls.parry) ? ISHOLDINGPARRY : 0;
			return;
		}
		return;
	}
	self.status |= ISBUSY;
	if (OnKeyPress(self.ctrls.chargeAtk) && self.currStamina > 0)
	{
		if (!self.ent.isInAir && self.status & CANCHARGE)
		{
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			PlayerResetChargeAttack(self, 0);
			self.currAtk = &self.chargeAtk[0].atk;
			self.ent.currMS = 0;
			self.ent.isMoving = false;
			PlayerSetStatus(self, CHRG);
			self.status |= ISHOLDINGATK;
			self.status &= ~ISDISMOUNTING & ~CANCHARGE & ~CANMOVE;
			return;
		}
		if (self.ent.isInAir && self.status & CANPLUNGE)
		{
			PlayerResetAttacks(self, self.currAtkIndex + 1);
			PlayerResetChargeAttack(self, 1);
			self.currAtk = &self.chargeAtk[1].atk;
			self.ent.currMS = 0;
			PlayerDecreaseStamina(self, self.currAtk->staminaCost);
			self.ent.verMS = -self.ent.vVel;
			self.ent.isMoving = false;
			PlayerSetStatus(self, PLUNGE);
			self.status &= ~CANCHARGE & ~CANMOVE;
			return;
		}
	}
	if (OnKeyPress(self.ctrls.attack) && self.status & CANATTACK && self.currStamina > 0)
	{
		if (!self.ent.isInAir)
			self.ent.currMS = 0;
		self.currAtk->sprite.timeElapsed = 0;
		PlayerNextAttack(self);
		PlayerDecreaseStamina(self, self.currAtk->staminaCost);
		self.ent.isMoving = false;
		PlayerSetStatus(self, ATTACK);
		self.status &= ~CANATTACK & ~CANMOVE;
		return;
	}
	if (kb.state[self.ctrls.parry] && self.status & CANPARRY && self.currStamina > 0)
	{
		self.ent.isMoving = false;
		self.currParrDur = self.parryDur;
		self.ent.currMS = 0;
		PlayerSetStatus(self, PARRY);
		self.status |= ISHOLDINGPARRY;
		self.status &= ~CANPARRY & ~CANMOVE;
		return;
	}
	if (OnKeyPress(self.ctrls.evade) && self.status & CANEVADE)
	{
		self.status &= ~CANEVADE;
		self.status |= EVADE;
		self.currEvadeDur = self.evadeDur;
		return;
	}
	self.status &= ~ISBUSY;
}

void PlayerPlatformVerCollision(Player& self, Platform& platform)
{
	if (!platform.vCollCheck)
		return;
	if (self.ent.verMS < 0.0f || !SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	if (int(self.ent.pos.y + self.ent.rect.h - self.ent.verMS) > platform.rect.y)
		return;
	if (self.status & ISDISMOUNTING && platform.isDismountable)
	{
		self.status &= ~ISDISMOUNTING & ~CANDISMOUNT;
		return;
	}
	if (platform.isDismountable)
		self.dOrder = PLATFORMS;
	self.ent.isInAir = false;
	self.ent.verMS = 0;
	self.status &= ~JUMP & ~FALL;
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
	self.status &= ~RUN;
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
	self.status |= CANATTACK | CANDEALDMG;
	self.status &= ~ISDEALINGDMG;
}

void PlayerResetChargeAttack(Player& self, int type)
{
	self.chargeAtk[type].atk.currCD = self.chargeAtk[type].atk.CD;
	self.chargeAtk[type].atk.currDur = self.chargeAtk[type].atk.dur;
	self.chargeAtk[type].atk.sprite.timeElapsed = self.chargeAtk[type].chargeTime = 0;
	self.chargeAtk[type].chrgSprite.timeElapsed = 0;
	self.status |= CANCHARGE | CANDEALDMG;
	self.status &= ~ISDEALINGDMG;
}

bool PlayerProcessAttack(Player& self, Attack& atk, Uint16 dt)
{
	atk.currDur -= dt;
	if (atk.currDur >= atk.postAtkDur)
	{
		if (atk.dur - atk.currDur > atk.delay)
		{
			self.status |= ISDEALINGDMG;
			atk.hitbox.x = self.ent.rect.x + (self.ent.dir == 1 ? self.ent.rect.w - atk.hitboxOffset.x : -atk.hitbox.w + atk.hitboxOffset.x);
			atk.hitbox.y = self.ent.pos.y - atk.hitboxOffset.y;
		}
		return false;
	}
	self.status |= CANDEALDMG;
	self.status &= ~ISBUSY & ~ISDEALINGDMG;
	return true;
}

bool PlayerProcessChargeAttack(Player& self, Uint16 dt)
{
	if (self.status & ISHOLDINGATK && PlayerGetStatus(self) == CHRG || self.chargeAtk[0].chargeTime < MIN_CHARGE_TIME)
	{
		self.chargeAtk[0].chargeTime += dt;
		return false;
	}
	if (PlayerGetStatus(self) == CHRG)
		PlayerSetStatus(self, CHRGATK);
	if (PlayerGetStatus(self) != CHRGPOSTATK)
	{
		if (PlayerProcessAttack(self, self.chargeAtk[0].atk, dt))
		{
			PlayerDecreaseStamina(self, self.currAtk->staminaCost);
			PlayerSetStatus(self, CHRGPOSTATK);
			self.status |= ISBUSY;
		}
		return false;
	}
	self.currAtk->currDur -= dt;
	if (self.currAtk->currDur <= 0)
	{
		self.status |= CANMOVE;
		self.status &= ~ISBUSY;
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
	if (PlayerGetStatus(self) == PLUNGE)
		PlayerSetStatus(self, PLUNGEATK);
	if (PlayerGetStatus(self) != PLUNGEPOSTATK)
	{
		if (PlayerProcessAttack(self, atk.atk, dt))
		{
			PlayerSetStatus(self, PLUNGEPOSTATK);
			self.status |= ISBUSY;
		}
		return false;
	}
	self.currAtk->currDur -= dt;
	if (self.currAtk->currDur <= 0)
	{
		self.status |= CANMOVE;
		self.status &= ~ISBUSY;
		return true;
	}
	return false;
}

void PlayerProcessParry(Player& self, Uint16 dt)
{
	self.currParrCD = self.parryCD;
	if (self.status & ISHOLDINGPARRY || self.parryDur - self.currParrDur <= MIN_PARRY_DUR)
	{
		self.currParrDur -= dt;
		if (self.currParrDur > 0)
			return;
	}
	self.status &= ~ISBUSY;
	self.status |= CANMOVE;
	PlayerDecreaseStamina(self, 0);
	PlayerSetStatus(self, IDLE);
}

void PlayerProcessEvade(Player& self, Uint16 dt)
{
	self.currEvadeCD = self.evadeCD;
	self.currEvadeDur -= dt;
	if (self.currEvadeDur > 0)
		return;
	self.status &= ~ISBUSY;
	PlayerToggleStatus(self, EVADE);
}

void PlayerAttackPenalty(Player& self, float duration)
{
	duration *= 1000.0f;
	duration = max(self.currAtk->currCD, duration);
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	self.atks[0].currCD = duration;
	self.chargeAtk[0].atk.currCD = duration;
	self.currAtkIndex = 0;
	self.currAtk = self.atks;
	self.status &= ~ATTACK & ~CHRGATK & ~PLUNGEATK & ~ISBUSY & ~CANATTACK & ~ISDEALINGDMG;
}

bool PlayerAttackCooldown(Player& self, Attack& atk, Uint16 dt)
{
	atk.currCD -= dt;
	if (atk.currCD > 0)
		return false;
	atk.currCD = 0;
	return true;
}

void PlayerCancelParry(Player& self)
{
	self.status &= ~PARRY & ~ISBUSY;
	self.status |= CANMOVE;
}

void PlayerParryCooldown(Player& self, Uint16 dt)
{
	if (PlayerGetStatus(self) == PARRY)
		return;
	self.currParrCD -= dt;
	if (self.currParrCD > 0)
		return;
	self.currParrCD = 0;
	self.status |= CANPARRY;
}

void PlayerEvadeCooldown(Player& self, Uint16 dt)
{
	if (PlayerGetStatus(self) == EVADE)
		return;
	self.currEvadeCD -= dt;
	if (self.currEvadeCD > 0)
		return;
	self.currEvadeCD = 0;
	self.status |= CANEVADE;
}

void PlayerDisable(Player& self, float dur)
{
	dur *= 1000;
	self.anims[GetAnim(DISABLED)].timeElapsed = 0;
	PlayerSetStatus(self, DISABLED);
	self.status &= ~CANMOVE & ~ISBUSY;
	self.ent.isMoving = false;
	self.disableDur = max(dur, self.disableDur);
}

void PlayerDisableElapse(Player& self, Uint16 dt)
{
	self.disableDur -= dt;
	if (self.disableDur > 0)
		return;
	self.disableDur = 0;
	self.status &= ~DISABLED;
	self.status |= CANMOVE;
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
		self.currStamina = fminf(self.maxStamina, self.currStamina + self.currStmRecharge);
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
	self.currCritRate = self.baseCritRate;

	if (self.currHP <= 0)
	{
		self.ent.isMoving = false;
		PlayerSetStatus(self, DEAD);
		return;
	}
	Uint32 status = PlayerGetStatus(self);
	if (!(self.status & ISBUSY))
		PlayerStaminaRecharge(self, dt);
	if (status == DISABLED)
		PlayerDisableElapse(self, dt);
	else if (self.status & ISBUSY)
	{
		if (status == ATTACK)
		{
			if (PlayerProcessAttack(self, *self.currAtk, dt))
				PlayerSetStatus(self, POSTATTACK);
		}
		else if (status >= CHRG && status <= CHRGPOSTATK)
		{
			if (PlayerProcessChargeAttack(self, dt))
				PlayerSetStatus(self, IDLE);
		}
		else if (status >= PLUNGE && status <= PLUNGEPOSTATK)
		{
			if (PlayerProcessPlungeAttack(self, dt))
				PlayerSetStatus(self, IDLE);
		}
		else if (status == PARRY)
			PlayerProcessParry(self, dt);
		else if (self.status & EVADE)
			PlayerProcessEvade(self, dt);
	}
	if (status != ATTACK)
	{
		if (status == POSTATTACK)
		{
			self.currAtk->currDur -= dt;
			if (self.currAtk->currDur <= 0)
			{
				PlayerToggleStatus(self, POSTATTACK);
				self.status |= CANMOVE;
			}
		}
		self.status &= ~CANATTACK;
		self.status |= PlayerAttackCooldown(self, self.atks[self.currAtkIndex], dt) ? CANATTACK : 0;
	}
	if (status < CHRG || status > CHRGPOSTATK)
	{
		self.status &= ~CANCHARGE;
		self.status |= PlayerAttackCooldown(self, self.chargeAtk[0].atk, dt) ? CANCHARGE : 0;
	}
	PlayerParryCooldown(self, dt);
	PlayerEvadeCooldown(self, dt);
	status = PlayerGetStatus(self);
	if (!(self.status & ISBUSY) && self.ent.isInAir && !(status & (FALL | DISABLED)))
	{
		PlayerSetStatus(self, self.ent.verMS > 20 ? FALL : JUMP);
		self.status |= CANMOVE;
	}
}

VanishText PlayerSpawnText(Player& self, const char* text, TTF_Font* font, int size, const SDL_Color& color, TTF_Font* outline)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.25, 0.09, 0.6, outline, RandInt(0, 1));
	VanishTextSetPos(txt, { EntityGetHorMid(self.ent) + RandInt(-self.ent.rect.w * 0.8, self.ent.rect.w * 0.8),
		EntityGetVerMid(self.ent) - 20 + RandInt(-40, 20) });
	return txt;
}

VanishText PlayerAttack(Player& self, Player& target, TTF_Font* font, TTF_Font* outline)
{
	self.status &= ~CANDEALDMG;
	bool isCrit = RandFloat(1, 100) <= self.currCritRate;
	self.currCritRate = isCrit ? self.currCritRate + 2.5 : self.baseCritRate;
	Sint8 dir;
	if (self.currAtk->dir == BOTH)
		dir = RectGetHorMid(self.ent.rect) < EntityGetHorMid(target.ent) ? 1 : -1;
	else
		dir = self.currAtk->dir * self.ent.dir;
	Uint32 status = PlayerGetStatus(self);
	float chrgTime = 1, stunDur = self.currAtk->stunDur;
	if (status == CHRGATK)
	{
		chrgTime += min(2, (self.chargeAtk[0].chargeTime - MIN_CHARGE_TIME) / 1000.0f);
		stunDur += chrgTime / 2.0f;
	}
	else if (status == PLUNGEATK)
	{
		chrgTime += (self.chargeAtk[1].chargeTime - self.chargeAtk[1].preChrgTime) / 1000.0f;
		stunDur += chrgTime * 3;
	}
	int dmg = int(self.currAtk->dmg * self.currDmg) * (1 + self.baseCritDmg * isCrit) * chrgTime;
	char buffer[10];
	sprintf_s(buffer, "%i", PlayerTakeHit(target, dmg, stunDur, dir, self.baseDmg * 0.054));
	return PlayerSpawnText(target, buffer, font, RandInt(21, 25) + 10 * isCrit, { 230, 230, 230 }, outline);
}

int PlayerTakeHit(Player& self, int dmg, int stunDur, int dir, int pushPower)
{
	PlayerResetAttacks(self, self.currAtkIndex + 1);
	PlayerSetStatus(self, DISABLED);
	self.status &= ~ISBUSY;
	self.ent.dir = -dir;
	self.ent.currMS = dir * pushPower;
	self.anims[GetAnim(DISABLED)].timeElapsed = 0;
	self.disableDur = max(stunDur, self.disableDur);
	self.ent.isMoving = false;
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
	SDL_SetTextureAlphaMod(self.currSprite->image.texture, self.status & EVADE ? 100 : 255);
	SDL_RenderCopyEx(ren, self.currSprite->image.texture, &self.currSprite->currFrame, &drawRect,
		0, NULL, self.currSprite->flip);
	//if (self.status & ISDEALINGDMG)
	//{
	//	SDL_SetRenderDrawColor(ren, 0, 150, 0, 255);
	//	drawRect = RectTransformForCurrWin(self.currAtk->hitbox);
	//	SDL_RenderFillRect(ren, &drawRect);
	//}
	drawRect = self.marker.rect;
	RectSetPos(drawRect, RectGetHorMid(self.ent.rect) - 9, self.ent.rect.y - 32);
	drawRect = RectTransformForCurrWin(drawRect);
	SDL_RenderCopy(ren, self.marker.texture, NULL, &drawRect);
}

void PlayerAnimate(Player& self, Uint16 dt)
{
	SDL_RendererFlip flip = self.ent.dir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
	if (true)
	{
		Uint32 status = PlayerGetStatus(self);
		AnimatedSprite* anim;
		if (status == CHRG || status == PLUNGE)
		{
			int i = status == CHRG ? 0 : 1;
			anim = &self.chargeAtk[i].chrgSprite;
			if (anim->timeElapsed + dt >= anim->dur)
				anim->timeElapsed = self.chargeAtk[i].preChrgTime - dt;
		}
		else if (status >= ATTACK && status <= PLUNGEPOSTATK)
		{
			anim = &self.currAtk->sprite;
		}
		else
		{
			anim = self.anims + GetAnim(status);
			if (status == RUN)
				dt *= abs(self.ent.currMS / self.ent.maxMS);
			else if (status == DEAD && anim->timeElapsed + dt >= anim->dur)
				anim->timeElapsed = anim->dur - dt - 1;
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