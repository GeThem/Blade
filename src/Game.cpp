#include "Game.h"

VanishText GameSpawnText(const SDL_FPoint& pos, const char* text, TTF_Font* font,
	int size, const SDL_Color& color, TTF_Font* outline)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.25, 0.09, 0.6, outline, RandInt(0, 1));
	VanishTextSetPos(txt, pos);
	return txt;
}

void GameRenderTime(Game& self)
{
	char buffer[3];
	sprintf_s(buffer, "%02.0f", ceil(self.currTime / 1000.0f));
	TTF_SetFontSize(self.font, 60);
	TTF_SetFontSize(self.fontOutline, 60);
	SDL_Color color;
	if (self.currTime > 10000)
		color = { 230, 230, 230, 255 };
	else
		color = { 230, 50, 50, 255 };
	RenderText(self.timer, self.font, buffer, color, self.fontOutline);
}

void GameInit(Game& self, const char* p1, const char* p2, const char* map)
{
	BonusesInit();

	self.font = TTF_OpenFont("data/fonts/PressStart2P-Regular.ttf", 50);
	self.fontOutline = TTF_OpenFont("data/fonts/PressStart2P-Regular.ttf", 50);
	TTF_SetFontOutline(self.fontOutline, FONT_OUTLINE_SIZE);

	self.currRCountdown = 0;
	self.currTime = self.roundTime;
	self.lastRenderedTime = self.roundTime / 1000;
	GameRenderTime(self);
	RectSetPos(self.timer.rect, (realW - self.timer.rect.w) / 2, 10);

	GameLoadControls(self);
	for (Uint8 i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		self.drawPriority[i] = &player;
		PlayerLoadCharacter(player, i == 0 ? p1 : p2);
		PlayerReboot(player, i == 1 ? -1 : 1);
		player.hpBar = { realW * i, 0, 0, 50 };
		player.staminaBar = { realW * i, 60, 0, 20 };
		player.color = { (Uint8)(20 + 130 * !i), 20, (Uint8)(20 + 130 * i) };
	}
	MapLoad(self.map, map);
}

void GameLoadControls(Game& self)
{
	FILE* file;
	if (fopen_s(&file, "data/controls.txt", "r"))
		exit(0);
	char temp[40];
	Uint8 playerN = SDL_MAX_UINT8;
	while (!feof(file))
	{
		fgets(temp, 40, file);
		if (*temp == '\n')
			continue;
		char* val = temp + strlen(temp) - 1;
		if (*val == '\n')
			*val = '\0';
		val = strchr(temp, '=');
		*val++ = '\0';
		
		if (!strcmp(temp, "player"))
			playerN = atoi(val) - 1;
		else if (!strcmp(temp, "left"))
			self.players[playerN].ctrls.left = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "right"))
			self.players[playerN].ctrls.right = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "jump"))
			self.players[playerN].ctrls.jump = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "dismount"))
			self.players[playerN].ctrls.dismount = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "attack"))
			self.players[playerN].ctrls.attack = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "charge attack"))
			self.players[playerN].ctrls.chargeAtk = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "parry"))
			self.players[playerN].ctrls.parry = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "evade"))
			self.players[playerN].ctrls.evade = SDL_GetScancodeFromName(val);
	}
	fclose(file);
}

void GameRestart(Game& self, bool clearScores)
{
	self.currTime = self.roundTime;
	self.lastRenderedTime = self.roundTime / 10;
	for (Uint8 i = 0; i < 2; i++)
	{
		PlayerReboot(self.players[i], i == 1 ? -1 : 1);
		EntityMoveTo(self.players[i].ent, {(1.0f + 2.0f * i) * realW / 4.0f, realH / 5.0f });
	}
	for (int i = 0; i < self.map.bonusesCount; i++)
	{
		self.map.bonuses[i].player = NULL;
		self.map.bonuses[i].isAvailable = true;
	}
	ListClear(self.vanishTexts);

	if (clearScores)
		self.playersWins[0] = self.playersWins[1] = 0;
}

void GameHandleArenaCollisions(Game& self)
{
	for (Player& player : self.players)
	{
		player.status |= CANPLUNGE;
		player.dOrder = FOREGROUND;
		for (int i = 0; i < self.map.platformsCount; i++)
		{
			Platform& platform = self.map.platforms[i];
			if (SDL_HasIntersection(&player.plungeRect, &platform.rect))
				player.status &= ~CANPLUNGE;
			PlayerPlatformVerCollision(player, platform);
			PlayerPlatformHorCollision(player, platform);
		}
	}
}

int GameRoundEndCheck(Game& self)
{
	if (self.restartingRound)
		return -1;
	if (self.currTime == 0)
	{
		self.currRCountdown = self.roundRestartCountdown;
		if (self.players[0].currHP < self.players[1].currHP)
			return 2;
		if (self.players[0].currHP > self.players[1].currHP)
			return 1;
		return 0;
	}
	if (self.players[0].currHP <= 0 && self.players[1].currHP <= 0)
		return 0;
	for (int i = 0; i < 2; i++)
	{
		if (self.players[i].currHP <= 0)
			return !i + 1;
	}
	return -1;
}

void GameRoundEndInvoke(Game& self, int i)
{
	self.currRCountdown = self.roundRestartCountdown;
	self.restartingRound = true;
	char msg[14];
	if (i == 0)
		strcpy_s(msg, "DRAW");
	else
	{
		self.playersWins[i - 1] += 1;
		sprintf_s(msg, "PLAYER %i WINS", i);
	}
	VanishText txt = VanishTextGenerate(msg, self.font, 90, { 230, 230, 230, 255 },
		0, 0, self.currRCountdown, self.fontOutline);
	VanishTextSetPos(txt, { realW / 2, realH / 2 });
	ListAppend(self.vanishTexts, txt);
}

void GameUpdateVisuals(Game& self, const Uint16& dt)
{
	for (int i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		PlayerAnimate(player, dt);
		player.hpBar.w = (i ? -1 : 1) * int(realW * 0.4f / player.maxHP * player.currHP);
		player.staminaBar.w = (i ? -1 : 1) * int(realW * 0.3f / player.maxStamina * player.currStamina);
	}

	for (VLElem* curr = self.vanishTexts.curr; curr; )
	{
		VanishTextUpdate(curr->val, dt);
		if (curr->val.alpha == 0 && curr->val.existTime <= 0)
		{
			VanishTextDestroy(curr->val);
			ListRemoveCurrent(self.vanishTexts);
			curr = self.vanishTexts.curr;
			continue;
		}
		curr = ListNext(self.vanishTexts);
	}

}

void GameUpdateTime(Game& self, const Uint16& dt)
{
	if (!self.restartingRound)
		self.currTime = fmaxf(0, self.currTime - dt);
	if (ceilf(self.currTime / 1000.0f) != self.lastRenderedTime)
	{
		self.lastRenderedTime = ceilf(self.currTime / 1000.0f);
		GameRenderTime(self);
	}

	if (self.currRCountdown)
	{
		self.currRCountdown -= dt;
		if (self.currRCountdown <= 0)
		{
			self.currRCountdown = 0;
			GameRestart(self, 0);
			self.restartingRound = false;
		}
	}
}

void PlayersClashCheckInvoke(Game& self)
{
	if (!(self.players[0].status & ISDEALINGDMG && self.players[1].status & ISDEALINGDMG))
		return;
	Player& p1 = self.players[0], & p2 = self.players[1];
	SDL_Rect inter;
	if (SDL_IntersectRect(&p1.currAtk->hitbox, &p2.currAtk->hitbox, &inter)
		&& p1.currAtk->dir * p1.ent.dir * p2.currAtk->dir * p2.ent.dir <= 0)
	{
		VanishText txt = GameSpawnText({ (float)RectGetHorMid(inter) + RandInt(-5, 5), (float)RectGetVerMid(inter) + RandInt(-5, 5) },
			"Clash", self.font, RandInt(27, 34), { 250, 94, 32 }, self.fontOutline);
		ListAppend(self.vanishTexts, txt);
		PlayerResetAttacks(p1, p1.currAtkIndex + 1);
		PlayerResetAttacks(p2, p2.currAtkIndex + 1);
		PlayerDisable(p1, 0.3);
		PlayerDisable(p2, 0.3);
	}
}

void PlayersAttacksHandle(Game& self)
{
	Player* atkPl = NULL, * recPl = NULL;
	for (int i = 0; i < 2; i++)
	{
		if (!(self.players[i].status & ISDEALINGDMG))
			continue;
		atkPl = &self.players[i];
		recPl = &self.players[!i];
		self.drawPriority[1] = atkPl;
		self.drawPriority[0] = recPl;
		if (!(recPl->currHP > 0 && atkPl->status & CANDEALDMG && SDL_HasIntersection(&atkPl->currAtk->hitbox, &recPl->ent.rect)))
			continue;
		if (recPl->status & EVADE)
		{
			atkPl->status &= ~CANDEALDMG;
			VanishText txt = PlayerSpawnText(*recPl, "Evade", self.font,
				RandInt(27, 34), { 20, 247, 115 }, self.fontOutline);
			ListAppend(self.vanishTexts, txt);
			return;
		}
		if (PlayerGetStatus(*recPl) != PARRY)
		{
			ListAppend(self.vanishTexts, PlayerAttack(*atkPl, *recPl, self.font, self.fontOutline));
			return;
		}
		int atkPlMid = EntityGetHorMid(atkPl->ent), recPlMid = EntityGetHorMid(recPl->ent);
		int successParryDir = atkPlMid < recPlMid ? -1 : atkPlMid == recPlMid ? 0 : 1;
		if (successParryDir * recPl->ent.dir < 0)
			continue;
		VanishText txt;
		atkPl->status &= ~CANDEALDMG;
		if (recPl->parryDur - recPl->currParrDur <= MIN_PARRY_DUR)
		{
			txt = PlayerSpawnText(*recPl, "Parry", self.font,
				RandInt(27, 34), { 20, 247, 115 }, self.fontOutline);
			PlayerDecreaseStamina(*recPl, 0);
			PlayerCancelParry(*recPl);
			if (PlayerGetStatus(*atkPl) == ATTACK)
			{
				PlayerAttackPenalty(*atkPl, 0.6);
				PlayerDisable(*atkPl, 0.3);
			}
		}
		else
		{
			txt = PlayerSpawnText(*recPl, "Block", self.font,
				RandInt(27, 34), { 229, 240, 24 }, self.fontOutline);
			recPl->ent.currMS = 5 * atkPl->ent.dir;
			PlayerDecreaseStamina(*recPl, atkPl->currAtk->dmg * atkPl->currDmg * 0.09);
			if (recPl->currStamina == 0)
			{
				PlayerDisable(*recPl, 0.5);
			}
		}
		ListAppend(self.vanishTexts, txt);
		return;
	}
}

Sint8 GameUpdate(Game& self, const Uint16& dt)
{
	//std::cout << self.players[0].currEvadeCD << "  " << self.players[0].currParrCD << "  " << self.players[0].currAtk->currCD <<
	//	"  " << self.players[0].currStaminaCD << "  " << self.players[0].currAtk->currDur << "  " << self.players[0].currParrDur << '\n';
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return TOMENU;
	
	int winner = GameRoundEndCheck(self);
	if (winner != -1)
		GameRoundEndInvoke(self, winner);

	GameUpdateVisuals(self, dt);
	
	for (Player& player : self.players)
	{
		PlayerInput(player);
		PlayerPhysics(player);
	}
	GameHandleArenaCollisions(self);
	for (Player& player : self.players)
	{
		PlayerUpdate(player, dt);
	}

	// BONUSES
	for (int i = 0; i < 2; i++)
	{
		self.map.bonuses[i].UpdateFunc(self.map.bonuses[i], dt);
		if (self.map.bonuses[i].isAvailable)
		{
			for (Player* player : self.drawPriority)
				if (PlayerGetStatus(*player) != DEAD && SDL_HasIntersection(&player->ent.rect, &self.map.bonuses[i].img.rect)
					&& !(player->activeBonuses & self.map.bonuses[i].type))
				{
					self.map.bonuses[i].ApplyFunc(self.map.bonuses[i], *player);
					break;
				}
		}
	}

	// PLAYERS INTERACTIONS
	PlayersClashCheckInvoke(self);
	PlayersAttacksHandle(self);

	GameUpdateTime(self, dt);

	return 0;
}

void GameDraw(Game& self)
{
	MapDrawBG(self.map);
	
	for (int i = 0; i < self.map.bonusesCount; i++)
		self.map.bonuses[i].DrawFunc(self.map.bonuses[i]);

	for (const Player* player : self.drawPriority)
		if (player->dOrder == PLATFORMS)
			PlayerDraw(*player);

	MapDrawPlatformsLayer(self.map);

	for (const Player* player : self.drawPriority)
		if (player->dOrder == FOREGROUND)
			PlayerDraw(*player);

	
	MapDrawFG(self.map);

	for (VLElem* curr = self.vanishTexts.curr; curr; curr = ListNext(self.vanishTexts))
	{
		VanishTextDraw(curr->val);
	}
	
	SDL_Rect drawRect;
	int i = 0;
	for (Player& player : self.players)
	{
		SDL_SetRenderDrawColor(ren, 200, 50, 50, 255);
		drawRect = RectTransformForCurrWin(player.hpBar);
		SDL_RenderFillRect(ren, &drawRect);
		SDL_SetRenderDrawColor(ren, 235, 223, 2, 255);
		drawRect = RectTransformForCurrWin(player.staminaBar);
		SDL_RenderFillRect(ren, &drawRect);
		for (Uint8 bonus = 1; bonus <= pow(2, BONUS_TYPES); bonus <<= 1)
		{
			if (player.activeBonuses & bonus)
			{
				drawRect = self.map.bonuses[0].img.rect;
				RectSetPos(drawRect, 10 + (realW - 20 - self.map.bonuses[0].img.rect.w) * i, 90);
				drawRect = RectTransformForCurrWin(drawRect);
				SDL_RenderCopy(ren, bonusesImages[GetBonusImageIndex(bonus)].texture, NULL, &drawRect);
			}
		}
		i++;
	}

	drawRect = RectTransformForCurrWin(self.timer.rect);
	SDL_RenderCopy(ren, self.timer.texture, NULL, &drawRect);
}

void GameClose(Game& self)
{
	for (Player& player : self.players)
	{
		PlayerClear(player);
	}
	ListClear(self.vanishTexts);
	MapDeinit(self.map);
	TTF_CloseFont(self.font);
	TTF_CloseFont(self.fontOutline);
	
	BonusesDeinit();
}