#pragma once

#include <string.h>

#include "utils.h"
#include "SDL_default.h"

typedef struct VanishText
{
	SDL_FPoint pos;
	float appearRate, vanishRate, sizeDecreaseRate;
	int existTime;
	int finalSize, currentSize;
	float alpha;
	Image txtImg;
	bool isMoving = RandInt(0, 1);

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
	float appearTime, float vanishTime, float existTime, TTF_Font* outline=NULL
);

void VanishTextUpdate(VanishText&, const Uint16& dt);
//Set pos for the center of the text
void VanishTextSetPos(VanishText&, const SDL_FPoint& pos);
void VanishTextDestroy(VanishText&);
void VanishTextDraw(const VanishText&);