#pragma once

#include <random>
#include <time.h>

#include "SDL_default.h"
#include "utils.h"
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
	MC_APPLY = 0,
	MC_BACK = 1,
	MAP1 = 0,
	MAP2 = 1,
	CHAR1 = 0,
	CHAR2 = 1
} MButtons;

typedef enum MenuType
{
	MAINMENU = 0,
	SETTINGSMENU = 1,
	INGAMEMENU = 2,
	MAPCHOICE = 3,
	CHARCHOICE = 4,
} MenuType;

typedef struct Menu
{
	Image* texts = NULL;
	TextButton* textButtons = NULL;
	SwitchButton* switchButtons = NULL;
	ChoiceButton* choiceButtons = NULL;
	ChoiceButton* activeChoiceButton = NULL;
	Uint8 textButtonsCount = 0, switchButtonsCount = 0, choiceButtonsCount = 0, textsCount = 0;
	Uint8 escReturn;
	SDL_Color background;
	Sint8(*UpdateFunc)(Menu&) = NULL;
	void(*DrawFunc)(const Menu&) = NULL;
	MenuType type;
} Menu;

Sint8 MenuUpdate(Menu&);
void MenuDraw(const Menu&);

void MainMenuInit(Menu&);

void SettingsMenuInit(Menu&);
Sint8 SettingsMenuUpdate(Menu&);
void SettingsMenuDraw(const Menu&);

void InGameMenuInit(Menu&);

void ChoiceMenuInit(Menu&, const char* text, int choiceButtonsCount, ChoiceButton*, MenuType type);
Sint8 ChoiceMenuUpdate(Menu&);
void ChoiceMenuDraw(const Menu&);

void MenuClear(Menu&);