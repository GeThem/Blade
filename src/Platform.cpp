#include "Platform.h"

void PlatformInit(Platform& self, const SDL_Rect& rect, bool hColl, bool vColl, bool isDismountable)
{
	self.rect = rect;
	self.hCollCheck = hColl;
	self.vCollCheck = vColl;
	self.isDismountable = isDismountable;
}

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
	for (int i = 0; i < self.platformsCount; i++)
	{
		fgets(temp, tempSize, file);
		char* strPointer = temp;
		int x, y, w, h;
		bool hColl, vColl, isDism;
		x = atoi(StrSplitInTwo(strPointer, ' '));
		y = atoi(StrSplitInTwo(strPointer, ' '));
		w = atoi(StrSplitInTwo(strPointer, ' '));
		h = atoi(StrSplitInTwo(strPointer, ' '));
		hColl = atoi(StrSplitInTwo(strPointer, ' '));
		vColl = atoi(StrSplitInTwo(strPointer, ' '));
		isDism = atoi(StrSplitInTwo(strPointer, ' '));
		PlatformInit(self.platforms[i], { x, y, w, h }, hColl, vColl, isDism);
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