#include "Game.h"
#include "Menus.h"
#include <stdio.h>

typedef enum State
{
	GAME,
	MENU,
	QUIT
} State;

typedef enum Menus
{
	MAINMENU = 0,
	SETTINGSMENU = 1,
	INGAMEMENU = 2
} Menus;

typedef bool (*Func)(Menu*&, Sint8&);

Sint8 GameLoop(Game& game, const bool& restartGame)
{
	if (restartGame)
		GameStart(game);
	Sint8 flag;
	do
	{
		GameFrameStartTime(game);
		GameHandleEvents(game);
		flag = GameUpdate(game);
		GameDraw(game);
		GameDelay(game);
	} while (flag != TOMENU);
	return MENU;
}

Sint8 MenuLoop(Menu& menu)
{
	MenuDrawBackground(menu);
	Sint8 flag;
	do
	{
		MenuFrameStartTime(menu);
		MenuHandleEvents(menu);
		flag = MenuUpdate(menu);
		MenuDraw(menu);
		MenuDelay(menu);
	} while (flag == -1);
	return flag;
}

bool MainMenuLeave(Menu*& menu, Sint8& flag)
{
	if (flag == MM_SETTINGS)
	{
		menu += SETTINGSMENU;
		flag = MENU;
		return true;
	}
	if (flag == MM_PLAY)
	{
		flag = GAME;
		menu += INGAMEMENU;
		return true;
	}
	flag = QUIT;
	return true;
}

bool SettingsMenuLeave(Menu*& menu, Sint8& flag)
{
	menu -= SETTINGSMENU;
	flag = MENU;
	return true;
}

bool InGameMenuLeave(Menu*& menu, Sint8& flag)
{
	if (flag == IG_BACKTOMENU)
	{
		flag = MENU;
		menu -= INGAMEMENU;
		return true;
	}
	bool returnVal = flag == IG_RESTART;
	flag = GAME;
	return returnVal;
}

int main(int argc, char** argv)
{
	Init();
	DisplayInit(1400, 800, "Blade");

	Game game = GameInit();
	Menu menus[] = { MainMenuInit(), SettingsMenuInit(), InGameMenuInit() };
	Func menuLeave[] = { MainMenuLeave, SettingsMenuLeave, InGameMenuLeave };

	Menu* currentMenu = menus + MAINMENU;
	Sint8 flag = MENU;
	bool restartGame = true;
	while (flag != QUIT)
	{
		if (flag == GAME)
		{
			flag = GameLoop(game, restartGame);
			continue;
		}
		if (flag == MENU)
		{
			flag = MenuLoop(*currentMenu);
			restartGame = menuLeave[currentMenu - menus](currentMenu, flag);
			continue;
		}
	}
	Quit();
	return 0;
}