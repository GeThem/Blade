#pragma once

#include <string.h>

#include "utils.h"
#include "SDL_default.h"

typedef struct VanishText
{
	SDL_FPoint pos;
	Image txtImg;
	float appearRate, vanishRate, sizeDecreaseRate, alpha, ratio;
	int existTime, finalSize, currentSize;
	bool isMoving;
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
	const char* text, TTF_Font* font, int size, const SDL_Color&, float appearTime,
	float vanishTime, float existTime, TTF_Font* outline = NULL, bool isMoving = false
);

void VanishTextUpdate(VanishText&, const Uint16& dt);
//Set pos for the center of the text
void VanishTextSetPos(VanishText&, const SDL_FPoint& pos);
void VanishTextDestroy(VanishText&);
void VanishTextDraw(const VanishText&);