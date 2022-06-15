#pragma once

typedef enum ButtonStates
{
	INACTIVE = 0,
	ACTIVE = 1,
	PRESSED = 2
} ButtonStates;

typedef struct TextButton
{
	SDL_Rect rect;
	Image textImg;
	SDL_Color butStates[3], * currState;
	bool wasPressed = false;
} TextButton;

TextButton TextButtonInit(SDL_Rect rect, const char* text, TTF_Font* font, const SDL_Color* butStates);
bool TextButtonUpdate(TextButton&);
void TextButtonDraw(const TextButton&);

typedef struct SwitchButton
{
	SDL_Rect rect;
	bool activated = true;
};

bool SwitchButtonUpdate(SwitchButton&);
void SwitchButtonDraw(const SwitchButton&);