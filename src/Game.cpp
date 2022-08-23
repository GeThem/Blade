#include "Game.h"

Game GameInit()
{
	Game self;
	GameLoadControls(self);
	for (Uint8 i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		PlayerLoadCharacter(player, "Char1.txt");
		player.color = { (Uint8)(150 * !i), 0, (Uint8)(150 * i) };
	}
	self.arena[0].rect = { 0, realH - 50, realW, 200 };
	self.arena[1] = { {0, realH - 300, realW / 5, 20 }, false, true, true };
	self.arena[2] = { {-50, 0, 60, realH}, true, false };
	self.arena[3] = { {realW - 10, 0, 60, realH}, true, false };
	self.arena[4] = { {int(realW * 0.5), realH - 320, 40, 600}, true, true };
	self.arena[5] = { {0, realH - 500, realW / 5, 20 }, false, true, true };
	return self;
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
		
			if (player.isDealingDmg && player.atks[0].dur != player.atks[0].currDur && SDL_HasIntersection(&platform.rect, &player.attackBox))
				PlayerAttackPenalty(player, 2);
		}
	}
}

Sint8 GameUpdate(Game& self, const Uint16& dt)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return TOMENU;

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
	if (self.players[0].isDealingDmg && self.players[0].canDealDmg
		&& SDL_HasIntersection(&self.players[0].attackBox, &self.players[1].ent.rect))
	{
		self.players[0].canDealDmg = false;
		
		int atk = self.players[0].isAttacking ? self.players[0].atks[self.players[0].currAtk].dmg :
			self.players[0].chargeAtk.dmg * min(1 + 0.002f * max(0, self.players[0].chargeAtk.chargeTime - 100), 2);

		char buffer[30];
		sprintf_s(buffer, "%i", PlayerTakeHit(self.players[1], atk));
		VanishText txt = PlayerSpawnText(self.players[1], buffer, self.playersInteractionsFont,
			RandInt(37, 43), { 200, 200, 200 });
		ListAppend(self.texts, txt);
	}

	for (int i = 0; i < 2; i++)
	{
		Player& player = self.players[i];
		player.hpRect = { realW * i, 0, (i ? -1 : 1) * int(realW * 0.4f / player.maxHP * player.currHP), 50 };
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
	
	for (const Player& player : self.players)
		PlayerDraw(player);

	for (VLElem* curr = self.texts.curr; curr; curr = ListNext(self.texts))
	{
		VanishTextDraw(curr->val);
	}
}

void GameQuit()
{
	Quit();
}