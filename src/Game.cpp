#include "Game.h"

Game GameInit(int win_w, int win_h)
{
	init();
	display_init(win_w, win_h, "Blade");

	Game self;
	PlayerLoadCharacter(self.player, "Char1.txt");
	EntityMoveTo(self.player.ent, { win_w / 4, win_h / 5 });
	self.player.ent.dir = 1;
	self.arena[0].rect = { 0, win_h - 50, win_w, 50 };
	self.arena[1].rect = { 0, win_h - 300, win_w / 5, 20 };
	self.arena[2] = { {-50, 0, 60, win_h}, true, false };
	self.arena[3] = { {win_w - 10, 0, 60, win_h}, true, false };
	self.arena[4] = { {(int)(win_w * 0.5), (int)(win_h * 0.6), 40, 600}, true, true};
	
	for (Projectile& projectile : self.player.projectiles)
	{
		projectile.ent.pos = { 0, 0 };
		projectile.ent.rect = { 0, 0, 50, 50 };
		projectile.ent.maxMS = projectile.ent.currMS = 40;
		projectile.ent.weight = 0.5;
		projectile.pickCD = projectile.currPickCD = 1000;
	}
	//self.lastTime = SDL_GetTicks();
	return self;
}

void GameFrameStartTime(Game& self)
{
	self.frameStart = SDL_GetTicks();
	//self.dt = (self.frameStart - self.lastTime) / 1000.0;
	//self.lastTime = self.frameStart;
}

void GameDelay(Game& self)
{
	self.frameTime = SDL_GetTicks() - self.frameStart;
	if (FRAME_DELAY > self.frameTime)
		SDL_Delay(FRAME_DELAY - self.frameTime);
}

void GameHandleEvents(Game& self)
{
	while (SDL_PollEvent(&self.ev))
	{
		switch (self.ev.type)
		{
		case SDL_KEYDOWN:
			if (self.ev.key.keysym.scancode != SDL_SCANCODE_ESCAPE)
				break;
		case SDL_QUIT:
			GameQuit();
		}
	}
}

void GameHandleArenaCollisions(Game& self)
{
	for (Platform& platform : self.arena)
	{
		for (Projectile& projectile : self.player.projectiles)
		{
			ProjectilePlatformHorCollision(projectile, platform);
			ProjectilePlatformVerCollision(projectile, platform);
		}
		
		PlayerPlatformVerCollision(self.player, platform);
		PlayerPlatformHorCollision(self.player, platform);
		
		if (self.player.isAttacking && SDL_HasIntersection(&platform.rect, &self.player.attackBox))
			PlayerAttackPenalty(self.player, 2);
	}
}

void GameUpdate(Game& self)
{
	GameHandleEvents(self);
	
	PlayerUpdate(self.player);
	
	GameHandleArenaCollisions(self);
}

void GameDraw(Game& self)
{
	screen_fill(0, 0, 0);
	for (const Projectile& projectile : self.player.projectiles)
		ProjectileDraw(projectile);
	for (const Platform& platform : self.arena)
		PlatformDraw(platform);
	PlayerDraw(self.player);

	SDL_RenderPresent(ren);
}

void GameQuit()
{
	quit();
}