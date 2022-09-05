#include "Bonuses.h"

void DoubleDamageInit(Bonus& self, const SDL_Point& pos, float dur, float cd)
{
	self.type = DOUBLE_DAMAGE;
	self.ApplyFunc = DoubleDamageApply;
	self.UpdateFunc = DoubleDamageUpdate;
	self.DrawFunc = DoubleDamageDraw;

	self.img = ImageLoad("data/bonuses/double damage.png");
	RectSetPos(self.img.rect, pos.x, pos.y);
	self.currDur = self.maxDur = dur * 1000.0f;
	self.maxCD = cd * 1000.0f;
	self.currCD = 0;
	self.player = NULL;
	self.isAvailable = true;
}

void DoubleDamageApply(Bonus& self, Player& player)
{
	self.isAvailable = false;
	self.currDur = self.maxDur;
	self.currCD = self.maxCD;
	self.player = &player;
	player.currDmg *= 2;
}

void DoubleDamageUpdate(Bonus& self, Uint16 dt)
{
	if (!self.player)
	{
		self.currCD -= dt;
		if (self.currCD <= 0)
			self.currCD = 0;
		return;
	}
	self.player->currDmg *= 2;
	self.currDur -= dt;
	if (self.currDur <= 0)
	{
		self.player = NULL;
		self.isAvailable = true;
	}
}

void DoubleDamageDraw(Bonus& self)
{
	if (self.isAvailable)
	{
		SDL_Rect drawRect = RectTransformForCurrWin(self.img.rect);
		SDL_RenderCopy(ren, self.img.texture, NULL, &drawRect);
	}
}

void BonusDeinit(Bonus& self)
{
	ImageDestroy(self.img);
}