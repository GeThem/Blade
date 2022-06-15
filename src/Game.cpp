#include <atlfile.h>

#include "Game.h"

Game GameInit()
{
	Game self;
	GameLoadControls(self);
	for (Uint8 i = 0; i < 2; i++)
	{
		PlayerLoadCharacter(self.players[i], "Char1.txt");
		self.players[i].color = { (Uint8)(150 * !i), 0, (Uint8)(150 * i) };
	}
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
		else if (!strcmp(temp, "throw"))
			self.players[playerN].ctrls.thrw = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "parry"))
			self.players[playerN].ctrls.parry = SDL_GetScancodeFromName(val);
		else if (!strcmp(temp, "evade"))
			self.players[playerN].ctrls.evade = SDL_GetScancodeFromName(val);
	}
	fclose(file);
}

void GameStart(Game& self)
{
	self.currTime = self.lastRenderedTime = 0;
	self.lastTime = SDL_GetTicks();
	Uint8 i = 0;
	for (Player& player : self.players)
	{
		PlayerReboot(player);
		EntityMoveTo(player.ent, { (1.0f + 2.0f * i) * winW / 4.0f, winH / 5.0f });
		for (Projectile& projectile : player.projectiles)
		{
			projectile.ent.pos = { 0, 0 };
			projectile.ent.verMS = 0;
			projectile.ent.rect = { 0, 0, 50, 50 };
			projectile.ent.vVel = 0.5;
			projectile.pickCD = projectile.currPickCD = 1000;
		}
		i++;
	}
	self.arena[0].rect = { 0, winH - 50, winW, 50 };
	self.arena[1] = { { 0, winH - 300, winW / 5, 20 }, false, true, true };
	self.arena[2] = { {-50, 0, 60, winH}, true, false };
	self.arena[3] = { {winW - 10, 0, 60, winH}, true, false };
	self.arena[4] = { {(int)(winW * 0.5), (int)(winH * 0.6), 40, 600}, true, true };
}

void GameFrameStartTime(Game& self)
{
	Uint32 currTime = SDL_GetTicks();
	self.dt = currTime - self.lastTime;
	self.currTime += self.dt;
	self.lastTime = currTime;
}

void GameDelay(Game& self)
{
	Uint32 frameTime = SDL_GetTicks() - self.lastTime;
	if (FRAME_DELAY > frameTime)
		SDL_Delay(FRAME_DELAY - frameTime);
}

void GameHandleEvents(Game& self)
{
	KeyboardUpdate();
	while (SDL_PollEvent(&self.ev))
	{
		switch (self.ev.type)
		{
		case SDL_QUIT:
			GameQuit();
		}
	}
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
		
			if (player.isAttacking && SDL_HasIntersection(&platform.rect, &player.attackBox))
				PlayerAttackPenalty(player, 2);
		}
	}
}

Sint8 GameUpdate(Game& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return TOMENU;
	static float term = -0.1;
	int i = 0;
	for (Player& player : self.players)
	{
		PlayerUpdate(player, self.dt);
		player.currHP += term;
		if (player.currHP < 0 && term < 0 || player.currHP > player.maxHP && term > 0)
			term = -term;
		player.hpRect = {0 + winW * i, 0, (i ? -1 : 1) * (int)((winW * 0.4f) / player.maxHP * player.currHP), 50};
		i++;
	}
	
	GameHandleArenaCollisions(self);
	return 0;
}

void GameDraw(Game& self)
{
	ScreenFill(0, 0, 0);
	for (const Player& player : self.players)
		for (const Projectile& projectile : player.projectiles)
			ProjectileDraw(projectile);
	
	for (const Platform& platform : self.arena)
		PlatformDraw(platform);
	
	for (const Player& player : self.players)
		PlayerDraw(player);

	SDL_RenderPresent(ren);
}

void GameQuit()
{
	Quit();
}