#include "Game.h"

VanishText GameSpawnText(const SDL_FPoint& pos, const char* text, TTF_Font* font, int size, const SDL_Color& color, TTF_Font* outline)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.25, 0.09, 0.6, outline);
	VanishTextSetPos(txt, pos);
	return txt;
}

void GameInit(Game& self, const char* p1, const char* p2, const char* map)
{
	for (int i = 0; i < 2; i++)
	{
		DoubleDamageInit(self.ddbonuses[i], { 100 + (realW - 220) * i, realH - 150 }, 10, 5);
	}

	self.playersInteractionsFont = TTF_OpenFont("data/fonts/PressStart2P-Regular.ttf", 50);
	self.playersInteractionsFontOutline = TTF_OpenFont("data/fonts/PressStart2P-Regular.ttf", 50);
	TTF_SetFontOutline(self.playersInteractionsFontOutline, FONT_OUTLINE_SIZE);
	GameLoadControls(self);
	for (Uint8 i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		self.drawPriority[i] = &player;
		PlayerLoadCharacter(player, i == 0 ? p1 : p2);
		PlayerReboot(player);
		player.hpBar = { realW * i, 0, 0, 50 };
		player.staminaBar = { realW * i, 60, 0, 20 };
		player.color = { (Uint8)(150 * !i), 0, (Uint8)(150 * i) };
	}
	MapLoad(self.map, map);
}

void GameLoadControls(Game& self)
{
	FILE* file;
	if (fopen_s(&file, "data/config.txt", "r"))
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
		else if (!strcmp(temp, "throw"))
			self.players[playerN].ctrls.thrw = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "parry"))
			self.players[playerN].ctrls.parry = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "evade"))
			self.players[playerN].ctrls.evade = SDL_GetScancodeFromName(val);
	}
	fclose(file);
}

void GameRestart(Game& self)
{
	for (Uint8 i = 0; i < 2; i++)
	{
		PlayerReboot(self.players[i]);
		EntityMoveTo(self.players[i].ent, {(1.0f + 2.0f * i) * realW / 4.0f, realH / 5.0f });
	}
	ListClear(self.texts);
}

void GameHandleArenaCollisions(Game& self)
{
	for (Player& player : self.players)
	{
		player.canPlunge = true;
		player.dOrder = FOREGROUND;
		for (int i = 0; i < self.map.platformsCount; i++)
		{
			Platform& platform = self.map.platforms[i];
			for (Projectile& projectile : player.projectiles)
			{
				ProjectilePlatformHorCollision(projectile, platform);
				ProjectilePlatformVerCollision(projectile, platform);
			}
			if (SDL_HasIntersection(&player.plungeRect, &platform.rect))
				player.canPlunge = false;
			PlayerPlatformVerCollision(player, platform);
			PlayerPlatformHorCollision(player, platform);
		
		}
	}
}

Sint8 GameUpdate(Game& self, const Uint16& dt)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return TOMENU;

	for (int i = 0; i < 2; i++)
	{
		DoubleDamageUpdate(self.ddbonuses[i], dt);
	}
	for (int i = 0; i < 2; i++)
	{
		if (self.ddbonuses[i].isAvailable)
		{
			for (Player* player : self.drawPriority)
				if (SDL_HasIntersection(&player->ent.rect, &self.ddbonuses[i].img.rect))
				{
					DoubleDamageApply(self.ddbonuses[i], *player);
					break;
				}
		}
	}


	for (int i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		PlayerAnimate(player, dt);
		player.hpBar.w = (i ? -1 : 1) * int(realW * 0.4f / player.maxHP * player.currHP);
		player.staminaBar.w = (i ? -1 : 1) * int(realW * 0.3f / player.maxStamina * player.currStamina);
	}

	for (VLElem* curr = self.texts.curr; curr; )
	{
		VanishTextUpdate(curr->val, dt);
		if (curr->val.alpha == 0 && curr->val.existTime <= 0)
		{
			VanishTextDestroy(curr->val);
			ListRemoveCurrent(self.texts);
			curr = self.texts.curr;
			continue;
		}
		curr = ListNext(self.texts);
	}

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
	// ATTACK
	if (self.players[0].isDealingDmg && self.players[1].isDealingDmg)
	{
		Player& p1 = self.players[0], & p2 = self.players[1];
		SDL_Rect inter;
		if (SDL_IntersectRect(&p1.currAtk->hitbox, &p2.currAtk->hitbox, &inter)
			&& p1.currAtk->dir * p1.ent.dir * p2.currAtk->dir * p2.ent.dir <= 0)
		{
			VanishText txt = GameSpawnText({ (float)RectGetHorMid(inter) + RandInt(-5, 5), (float)RectGetVerMid(inter) + RandInt(-5, 5) },
				"Clash", self.playersInteractionsFont, RandInt(37, 44), {250, 94, 32});
			ListAppend(self.texts, txt);
			PlayerResetAttacks(p1, p1.currAtkIndex + 1);
			PlayerResetAttacks(p2, p2.currAtkIndex + 1);
			PlayerDisable(p1, 0.3);
			PlayerDisable(p2, 0.3);
		}
	}
	Player* atkPl = NULL, * recPl = NULL;
	for (int i = 0; i < 2; i++)
	{
		if (self.players[i].isDealingDmg)
		{
			atkPl = &self.players[i];
			recPl = &self.players[!i];
			self.drawPriority[1] = atkPl;
			self.drawPriority[0] = recPl;
		}
		if (atkPl && recPl->currHP > 0)
		{
			if (atkPl->canDealDmg && SDL_HasIntersection(&atkPl->currAtk->hitbox, &recPl->ent.rect))
			{
				if (strstr(recPl->status, "parry"))
				{
					int atkPlMid = EntityGetHorMid(atkPl->ent), recPlMid = EntityGetHorMid(recPl->ent);
					int successParryDir = atkPlMid < recPlMid ? -1 : atkPlMid == recPlMid ? 0 : 1;
					if (successParryDir * recPl->ent.dir >= 0)
					{
						VanishText txt;
						atkPl->canDealDmg = false;
						if (recPl->parryDur - recPl->currParrDur <= MIN_PARRY_DUR)
						{
							txt = PlayerSpawnText(*recPl, "Parry", self.playersInteractionsFont,
								RandInt(37, 44), {20, 247, 115}, self.playersInteractionsFontOutline);
							PlayerDecreaseStamina(*recPl, 0);
							PlayerCancelParry(*recPl);
							if (strstr(atkPl->status, "attack"))
							{
								PlayerResetAttacks(*atkPl, atkPl->currAtkIndex + 1);
								PlayerDisable(*atkPl, 0.3);
							}
						}
						else
						{
							txt = PlayerSpawnText(*recPl, "Block", self.playersInteractionsFont,
								RandInt(37, 44), {229, 240, 24}, self.playersInteractionsFontOutline);
							recPl->ent.currMS = 5 * atkPl->ent.dir;
							PlayerDecreaseStamina(*recPl, atkPl->currAtk->dmg * 0.09);
							if (recPl->currStamina == 0)
							{
								PlayerDisable(*recPl, 0.5);
							}
						}
						ListAppend(self.texts, txt);
						break;
					}
				}
				else if (strstr(recPl->status, "evade"))
				{
					atkPl->canDealDmg = false;
					VanishText txt = PlayerSpawnText(*recPl, "Evade", self.playersInteractionsFont,
						RandInt(37, 44), { 20, 247, 115 }, self.playersInteractionsFontOutline);
					ListAppend(self.texts, txt);
					break;
				}
				//: p1.chargeAtk.dmg * min(1 + 0.002f * max(0, p1.chargeAtk.chargeTime - 100), 2);
				ListAppend(self.texts, PlayerAttack(*atkPl, *recPl, self.playersInteractionsFont, self.playersInteractionsFontOutline));
			}
		}
	}

	return 0;
}

void GameDraw(Game& self)
{
	MapDrawBG(self.map);

	for (const Player& player : self.players)
		for (const Projectile& projectile : player.projectiles)
			ProjectileDraw(projectile);
	
	for (DoubleDamage& bonus : self.ddbonuses)
		DoubleDamageDraw(bonus);

	for (const Player* player : self.drawPriority)
		if (player->dOrder == PLATFORMS)
			PlayerDraw(*player);

	MapDrawPlatformsLayer(self.map);

	for (const Player* player : self.drawPriority)
		if (player->dOrder == FOREGROUND)
			PlayerDraw(*player);
	
	MapDrawFG(self.map);

	for (VLElem* curr = self.texts.curr; curr; curr = ListNext(self.texts))
	{
		VanishTextDraw(curr->val);
	}
}

void GameClose(Game& self)
{
	for (Player& player : self.players)
	{
		PlayerClear(player);
	}
	ListClear(self.texts);
	MapDeinit(self.map);
	TTF_CloseFont(self.playersInteractionsFont);
	TTF_CloseFont(self.playersInteractionsFontOutline);
}