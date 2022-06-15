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

void GameLoop(Game& game, const bool& restartGame, Sint8& flag)
{
	if (restartGame)
		GameStart(game);
	do
	{
		GameFrameStartTime(game);
		GameHandleEvents(game);
		flag = GameUpdate(game);
		GameDraw(game);
		SDL_RenderPresent(ren);
		GameDelay(game);
	} while (flag != TOMENU);
	flag = MENU;
}

void MenuLoop(Menu& menu, const Game* game, Sint8& flag)
{
	do
	{
		MenuFrameStartTime(menu);
		MenuHandleEvents(menu);
		flag = MenuUpdate(menu);
		if (game)
			GameDraw(*game);
		MenuDraw(menu);
		SDL_RenderPresent(ren);
		MenuDelay(menu);
	} while (flag == -1);
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
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	Game game = GameInit();
	Menu menus[] = { MainMenuInit(), SettingsMenuInit(), InGameMenuInit() };
	bool (*menusLeave[])(Menu*&, Sint8&) = { MainMenuLeave, SettingsMenuLeave, InGameMenuLeave };

	Menu* currentMenu = menus;
	Sint8 flag = MENU;
	bool restartGame = true;
	while (flag != QUIT)
	{
		if (flag == GAME)
		{
			GameLoop(game, restartGame, flag);
			continue;
		}
		if (flag == MENU)
		{
			MenuLoop(*currentMenu, currentMenu - menus == INGAMEMENU ? &game : NULL, flag);
			restartGame = menusLeave[currentMenu - menus](currentMenu, flag);
			continue;
		}
	}
	Quit();
	return 0;
}