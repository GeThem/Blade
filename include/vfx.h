#pragma once

#include "SDL_default.h"
#include <string.h>

typedef struct VanishText
{
	SDL_FPoint pos;
	float appearRate, vanishRate, sizeDecreaseRate;
	int existTime;
	int finalSize, currentSize;
	int alpha;
	Image txtImg;

	float ratio;
} VanishText;

typedef struct VLElem
{
	VLElem* next = NULL, * prev = NULL;
	VanishText val;
};

typedef struct VList
{
	VLElem* head = NULL, *curr = NULL, *tail = NULL;
} VList;

VanishText VanishTextGenerate(
	const char* text, TTF_Font* font, int size, const SDL_Color&,
	float appearTime, float vanishTime, float existTime
);

void VanishTextUpdate(VanishText&, const Uint16& dt);
//Set pos for the center of the text
void VanishTextSetPos(VanishText&, const SDL_FPoint& pos);
void VanishTextDestroy(VanishText&);
void VanishTextDraw(const VanishText&);