#include "Bonuses.h"

void DoubleDamageInit(DoubleDamage& self, const SDL_Point& pos, float dur, float cd)
{
	self.img = ImageLoad("data/bonuses/double damage.png");
	self.img.rect.x = pos.x;
	self.img.rect.y = pos.y;
	self.currDur = self.maxDur = dur * 1000.0f;
	self.maxCD = cd * 1000.0f;
	self.currCD = 0;
	self.player = NULL;
	self.isAvailable = true;
}

void DoubleDamageApply(DoubleDamage& self, Player& player)
{
	self.isAvailable = false;
	self.currDur = self.maxDur;
	self.currCD = self.maxCD;
	self.player = &player;
	for (int i = 0; i < self.player->numberOfAttacks; i++)
		self.player->atks[i].dmg *= 2;
	for (int i = 0; i < 2; i++)
		self.player->chargeAtk[i].atk.dmg *= 2;
}

void DoubleDamageUpdate(DoubleDamage& self, Uint16 dt)
{
	if (!self.player)
	{
		self.currCD -= dt;
		if (self.currCD <= 0)
			self.currCD = 0;
		return;
	}
	self.currDur -= dt;
	if (self.currDur <= 0)
	{
		for (int i = 0; i < self.player->numberOfAttacks; i++)
			self.player->atks[i].dmg /= 2;
		for (int i = 0; i < 2; i++)
			self.player->chargeAtk[i].atk.dmg /= 2;
		self.player = NULL;
		self.isAvailable = true;
	}
}

void DoubleDamageDraw(DoubleDamage& self)
{
	if (self.isAvailable)
	{
		SDL_Rect drawRect = RectTransformForCurrWin(self.img.rect);
		SDL_RenderCopy(ren, self.img.texture, NULL, &drawRect);
	}
}