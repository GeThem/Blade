#pragma once

#include "SDL_default.h"
#include <string.h>

typedef struct VanishText
{
	SDL_FPoint pos;
	TTF_Font* font;
	SDL_Color color;
	char text[30];
	float vSpd, hSpd, vVel;
	float appearTick, vanishTick;
	int existTime;
	float alpha;
	Image txtImg;
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
	const char* text, TTF_Font* font, const SDL_Color&, float vVel, float vSpd,
	float hSpd, float appearTime, float vanishTime, float existTime
);

void VanishTextUpdate(VanishText&, const Uint16& dt);
void VanishTextSetPos(VanishText&, const SDL_Point& pos);
void VanishTextDestroy(VanishText&);
void VanishTextDraw(const VanishText&);