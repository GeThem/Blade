#include "Map.h"

void MapLoad(Map& self, const char* name)
{
	const Uint8 tempSize = 100;
	char temp[tempSize] = "data/maps/";
	strcat_s(temp, name);
	strcat_s(temp, "/bg.png");
	self.bg = ImageLoad(temp);
	self.bg.rect.x = self.bg.rect.y = 0;
	StrReplace(temp, "bg", "fg");
	self.fg = ImageLoad(temp);
	self.fg.rect.x = self.fg.rect.y = 0;
	StrReplace(temp, "fg", "platforms");
	self.platformsLayer = ImageLoad(temp);
	self.platformsLayer.rect.x = self.platformsLayer.rect.y = 0;
	StrReplace(temp, "platforms.png", "objects.txt");
	FILE* file;
	if (fopen_s(&file, temp, "r"))
		exit(0);

	fgets(temp, tempSize, file);
	self.platformsCount = atoi(temp);
	self.platforms = (Platform*)malloc(self.platformsCount * sizeof(Platform));
	char* val;
	for (int i = 0; i < self.platformsCount; i++)
	{
		fgets(temp, tempSize, file);
		val = temp;
		int x, y, w, h;
		bool hColl, vColl, isDism;
		x = atoi(StrSplitInTwo(val, ' '));
		y = atoi(StrSplitInTwo(val, ' '));
		w = atoi(StrSplitInTwo(val, ' '));
		h = atoi(StrSplitInTwo(val, ' '));
		hColl = atoi(StrSplitInTwo(val, ' '));
		vColl = atoi(StrSplitInTwo(val, ' '));
		isDism = atoi(StrSplitInTwo(val, ' '));
		PlatformInit(self.platforms[i], { x, y, w, h }, hColl, vColl, isDism);
	}
	while (!feof(file))
	{
		int currBonusesCount = 0;
		bool canProceed = false;
		fgets(temp, tempSize, file);
		val = temp;
		StrSplitInTwo(val, '=');
		if (!strcmp(temp, "bonuses"))
		{
			self.bonusesCount = atoi(val);
			self.bonuses = (Bonus*)malloc(sizeof(Bonus) * self.bonusesCount);
			canProceed = true;
		}
		if (!strcmp(temp, "dd"))
		{
			int loops = atoi(val) + currBonusesCount;
			for (; currBonusesCount < loops; currBonusesCount++)
			{
				fgets(temp, tempSize, file);
				val = temp;
				DoubleDamageInit(self.bonuses[currBonusesCount], { atoi(StrSplitInTwo(val, ' ')), atoi(val) });
			}
		}
	}
}


void MapDrawBG(Map& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(self.bg.rect);
	SDL_RenderCopy(ren, self.bg.texture, NULL, &drawRect);
}

void MapDrawPlatformsLayer(Map& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(self.platformsLayer.rect);
	SDL_RenderCopy(ren, self.platformsLayer.texture, NULL, &drawRect);
}

void MapDrawFG(Map& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(self.fg.rect);
	SDL_RenderCopy(ren, self.fg.texture, NULL, &drawRect);
}

void MapDeinit(Map& self)
{
	ImageDestroy(self.bg);
	ImageDestroy(self.fg);
	ImageDestroy(self.platformsLayer);
	free(self.platforms);
	self.platformsCount = 0;
}