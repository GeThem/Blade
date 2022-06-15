#pragma once

#include "Buttons.h"

typedef enum MButtons
{
	MM_PLAY = 0,
	MM_SETTINGS = 1,
	MM_EXIT = 2,
	IG_RESUME = 0,
	IG_RESTART = 1,
	IG_BACKTOMENU = 2,
	SM_BACK = 0
} MButtons;

typedef struct Menu
{
	SDL_Event ev;
	Uint32 lastTime = 0, currTime = 0;
	TextButton* textButtons;
	Uint8 textButtonsCount, escReturn;
	Uint8 alpha = 255;
} Menu;

void MenuFrameStartTime(Menu&);
void MenuHandleEvents(Menu&);
Sint8 MenuUpdate(Menu&);
void MenuDrawBackground(const Menu&);
void MenuDraw(const Menu&);
void MenuDelay(Menu&);

Menu MainMenuInit();

Menu SettingsMenuInit();

Menu InGameMenuInit();