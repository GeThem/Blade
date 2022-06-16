#pragma once

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

TextButton TextButtonInit(const SDL_Rect&, const char* text, TTF_Font*, const SDL_Color* butStates);
bool TextButtonUpdate(TextButton&);
void TextButtonDraw(const TextButton&);

typedef struct SwitchButton
{
	SDL_Rect rect;
	Image textImg;
	SDL_Color butStates[4], * currState;
	bool isActivated = false;
} SwitchButton;

SwitchButton SwitchButtonInit(const SDL_Rect&, const char* text, TTF_Font*, const SDL_Color* butStates);
bool SwitchButtonUpdate(SwitchButton&);
void SwitchButtonDraw(const SwitchButton&);