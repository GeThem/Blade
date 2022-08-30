#pragma once

#include <random>
#include <time.h>

#include "Buttons.h"

typedef enum MButtons
{
	MM_PLAY = 0,
	MM_SETTINGS = 1,
	MM_EXIT = 2,
	IG_RESUME = 0,
	IG_RESTART = 1,
	IG_BACKTOMENU = 2,
	SM_SAVE = 0,
	SM_BACK = 1,
} MButtons;

typedef struct Menu
{
	TextButton* textButtons = NULL;
	SwitchButton* switchButtons = NULL;
	Uint8 textButtonsCount = 0, switchButtonsCount = 0, escReturn;
	SDL_Color Background;
	Sint8(*UpdateFunc)(Menu&) = NULL;
} Menu;

Sint8 MenuUpdate(Menu&);
void MenuDraw(const Menu&);

Menu MainMenuInit();

Menu SettingsMenuInit();
Sint8 SettingsMenuUpdate(Menu&);

Menu InGameMenuInit();