#include "Bonuses.h"

Image bonusesImages[1];

int GetBonusImageIndex(Uint32 type)
{
	int index = -1;
	for (; type; type >>= 1)
		index++;
	return index;
}

void BonusesInit()
{
	bonusesImages[0] = ImageLoad("data/bonuses/double damage.png");
	SDL_SetTextureScaleMode(bonusesImages[0].texture, SDL_ScaleModeBest);
}

void DoubleDamageInit(Bonus& self, const SDL_Point& pos)
{
	self.type = DOUBLE_DAMAGE;
	self.ApplyFunc = DoubleDamageApply;
	self.UpdateFunc = DoubleDamageUpdate;
	self.DrawFunc = DoubleDamageDraw;

	self.img = bonusesImages[GetBonusImageIndex(self.type)];
	RectSetPos(self.img.rect, pos.x, pos.y);
	self.currDur = self.maxDur = 10000;
	self.maxCD = 5000;
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
	self.player->activeBonuses |= DOUBLE_DAMAGE;
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
		self.player->activeBonuses &= ~DOUBLE_DAMAGE;
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

void BonusesDeinit()
{
	for (Image& img : bonusesImages)
		ImageDestroy(img);
}