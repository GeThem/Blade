#include "Game.h"

VanishText GameSpawnText(const SDL_FPoint& pos, const char* text, TTF_Font* font, int size, const SDL_Color& color)
{
	VanishText txt = VanishTextGenerate(text, font, size, color, 0.25, 0.09, 0.6);
	VanishTextSetPos(txt, pos);
	return txt;
}

void GameInit(Game& self)
{
	self.playersInteractionsFont = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", 30);
	GameLoadControls(self);
	for (Uint8 i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		self.drawPriority[i] = &player;
		PlayerLoadCharacter(player, "Fantasy Warrior");
		PlayerReboot(player);
		player.color = { (Uint8)(150 * !i), 0, (Uint8)(150 * i) };
	}
	self.arena[0].rect = { 0, realH - 50, realW, 200 };
	self.arena[1] = { {0, realH - 250, realW / 5, 20 }, false, true, true };
	self.arena[2] = { {-50, 0, 60, realH}, true, false };
	self.arena[3] = { {realW - 10, 0, 60, realH}, true, false };
	self.arena[4] = { {int(realW * 0.5), realH - 200, 40, 600}, true, true };
	self.arena[5] = { {0, realH - 400, realW / 5, 20 }, false, true, true };
	self.arena[6] = { {650, realH - 520, realW / 6, 20 }, false, true, true };
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
		EntityMoveTo(self.players[i].ent, { crd0.x + (1.0f + 2.0f * i) * realW / 4.0f, crd0.y + realH / 5.0f });
	}
	ListClear(self.texts);
}

void GameHandleArenaCollisions(Game& self)
{
	for (Platform& platform : self.arena)
	{
		for (Player& player : self.players)
		{
			for (Projectile& projectile : player.projectiles)
			{
				ProjectilePlatformHorCollision(projectile, platform);
				ProjectilePlatformVerCollision(projectile, platform);
			}
		
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
		Player& p1 = self.players[0];
		//if (i == 0)
		//std::cout << p1.atks[p1.currAtk].betweenHitsTime << "  " << (int)p1.currAtk << "  " << p1.status << "  " << p1.currSprite << '\n';
		Player& player = self.players[i];
		//if (i == 0)
		//	std::cout << player.ent.currMS << '\n';
		//if (i == 0)
		//	std::cout << p1.currSprite << '\n';
		PlayerAnimate(player, dt);
		player.hpRect = { realW * i, 0, (i ? -1 : 1) * int(realW * 0.4f / player.maxHP * player.currHP), 50 };
		//if (i == 0)
		//std::cout << p1.currSprite->image.texture << '\n';
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
		EntityUpdate(player.ent);
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
		if (SDL_IntersectRect(&p1.atks[p1.currAtk].hitbox, &p2.atks[p2.currAtk].hitbox, &inter)
			&& p1.atks[p1.currAtk].dir * p1.ent.dir * p2.atks[p2.currAtk].dir * p2.ent.dir <= 0)
		{
			VanishText txt = GameSpawnText({ (float)RectGetHorMid(inter) + RandInt(-5, 5), (float)RectGetVerMid(inter) + RandInt(-5, 5) },
				"Clash", self.playersInteractionsFont, RandInt(37, 44), {250, 94, 32});
			ListAppend(self.texts, txt);
			PlayerResetAttacks(p1, p1.currAtk + 1);
			PlayerResetAttacks(p2, p2.currAtk + 1);
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
		if (atkPl)
		{
			if (atkPl->canDealDmg && SDL_HasIntersection(&atkPl->atks[atkPl->currAtk].hitbox, &recPl->ent.rect))
			{
				if (strstr(recPl->status, "_parry"))
				{
					int atkPlDir;
					if (atkPl->atks[atkPl->currAtk].dir == 0)
						atkPlDir = EntityGetHorMid(atkPl->ent) < EntityGetHorMid(recPl->ent) ? 1 : -1;
					else
						atkPlDir = atkPl->atks[atkPl->currAtk].dir * atkPl->ent.dir;
					if (atkPlDir != recPl->ent.dir)
					{
						VanishText txt;
						if (recPl->parryDur - recPl->currParrDur <= 170)
						{
							txt = PlayerSpawnText(*recPl, "Parry", self.playersInteractionsFont, RandInt(37, 44), {20, 247, 115});
							PlayerResetAttacks(*atkPl, atkPl->currAtk + 1);
							PlayerDisable(*atkPl, 0.3);
							PlayerCancelParry(*recPl);
						}
						else
						{
							txt = PlayerSpawnText(*recPl, "Block", self.playersInteractionsFont, RandInt(37, 44), {229, 240, 24});
							atkPl->canDealDmg = false;
						}
						ListAppend(self.texts, txt);
						break;
					}
				}
				atkPl->canDealDmg = false;

				Attack& atk = strstr(atkPl->status, "_attack") ? atkPl->atks[atkPl->currAtk] : atkPl->atks[0];
				//: p1.chargeAtk.dmg * min(1 + 0.002f * max(0, p1.chargeAtk.chargeTime - 100), 2);
				char buffer[6];
				int takenDmg = PlayerTakeHit(*recPl, atk, EntityGetHorMid(atkPl->ent) < EntityGetHorMid(recPl->ent) ? 1 : -1);
				sprintf_s(buffer, "%i", takenDmg);
				VanishText txt = PlayerSpawnText(*recPl, buffer, self.playersInteractionsFont, RandInt(17, 24) + takenDmg, { 230, 230, 230 });
				ListAppend(self.texts, txt);
			}
		}
	}

	return 0;
}

void GameDraw(Game& self)
{
	ScreenFill(3, 186, 252);
	for (const Player& player : self.players)
		for (const Projectile& projectile : player.projectiles)
			ProjectileDraw(projectile);
	
	for (const Platform& platform : self.arena)
		PlatformDraw(platform);
	
	for (const Player* player : self.drawPriority)
		PlayerDraw(*player);

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
	TTF_CloseFont(self.playersInteractionsFont);
}