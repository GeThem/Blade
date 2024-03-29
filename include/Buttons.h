#pragma once

#include "SDL_default.h"

typedef enum ButtonStates
{
	INACTIVE = 0,
	ACTIVE = 1,
	PRESSED = 2,
	PRESSED_ACTIVE = 3
} ButtonStates;

typedef struct TextButton
{
	SDL_Rect rect;
	Image textImg;
	SDL_Color butStates[3], * currState;
} TextButton;

TextButton TextButtonInit(const SDL_Rect&, const char* text, TTF_Font*, const SDL_Color(&butStates)[3]);
bool TextButtonUpdate(TextButton&);
void TextButtonDraw(const TextButton&);
void TextButtonDestroy(TextButton&);

typedef struct SwitchButton
{
	SDL_Rect rect;
	Image textImg;
	SDL_Color butStates[4], * currState;
	bool isActivated = false;
} SwitchButton;

SwitchButton SwitchButtonInit(const SDL_Rect&, const char* text, TTF_Font*, const SDL_Color (&butStates)[4]);
bool SwitchButtonUpdate(SwitchButton&);
void SwitchButtonDraw(const SwitchButton&);
void SwitchButtonDestroy(SwitchButton&);

typedef struct ChoiceButton
{
	SDL_Rect rect, outlineRect;
	int index;
	Image image;
	SDL_Color butStates[2];
	bool isActivated = false;
} ChoiceButton;

ChoiceButton ChoiceButtonInit(const SDL_Rect&, const Image&, const SDL_Color(&butStates)[2], int index);
bool ChoiceButtonUpdate(ChoiceButton&);
void ChoiceButtonDraw(const ChoiceButton&);
void ChoiceButtonDestroy(ChoiceButton&);