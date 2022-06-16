#include "Game.h"
#include "Menus.h"
#include <stdio.h>

typedef enum Loop
{
	GAME,
	MENU,
	QUIT
} Loop;

typedef enum MenuType
{
	MAINMENU = 0,
	SETTINGSMENU = 1,
	INGAMEMENU = 2
} MenuType;

bool MainMenuLeave(Menu*& menu, Sint8& loopFlag)
{
	if (loopFlag == MM_SETTINGS)
	{
		menu += SETTINGSMENU;
		loopFlag = MENU;
		return true;
	}
	if (loopFlag == MM_PLAY)
	{
		loopFlag = GAME;
		menu += INGAMEMENU;
		return true;
	}
	loopFlag = QUIT;
	return true;
}

bool SettingsMenuLeave(Menu*& menu, Sint8& loopFlag)
{
	menu -= SETTINGSMENU;
	loopFlag = MENU;
	return true;
}

bool InGameMenuLeave(Menu*& menu, Sint8& loopFlag)
{
	if (loopFlag == IG_BACKTOMENU)
	{
		loopFlag = MENU;
		menu -= INGAMEMENU;
		return true;
	}
	bool returnVal = loopFlag == IG_RESTART;
	loopFlag = GAME;
	return returnVal;
}

typedef struct App
{
	SDL_Event ev;
	Uint32 lastTime = 0, currTime = 0, lastRenderedTime = 0, dt = 0;
	Game game = GameInit();
	Menu menus[3]{ MainMenuInit(), SettingsMenuInit(), InGameMenuInit() };
	bool (*menuLeave[3])(Menu*&, Sint8&){ MainMenuLeave, SettingsMenuLeave, InGameMenuLeave };
	Menu* currMenu = menus;
	Sint8 loopFlag = MENU;
	bool restartGame = true;
} App;

MenuType CurrMenuType(App& self)
{
	return (MenuType)(self.currMenu - self.menus);
}

void AppRestartTime(App& self)
{
	self.currTime = self.lastRenderedTime = 0;
	self.lastTime = SDL_GetTicks();
}

void AppFrameStartTime(App& self)
{
	Uint32 currTime = SDL_GetTicks();
	self.dt = currTime - self.lastTime;
	self.currTime += self.dt;
	self.lastTime = currTime;
}

void AppDelay(App& self)
{
	Uint32 frameTime = SDL_GetTicks() - self.lastTime;
	if (FRAME_DELAY > frameTime)
		SDL_Delay(FRAME_DELAY - frameTime);
}

void AppHandleEvents(App& self)
{
	MouseUpdate();
	KeyboardUpdate();
	while (SDL_PollEvent(&self.ev))
	{
		switch (self.ev.type)
		{
		case SDL_QUIT:
			Quit();
		}
	}
}

void GameLoop(App& app)
{
	Game& game = app.game;
	if (app.restartGame)
	{
		GameRestart(game);
		AppRestartTime(app);
	}
	do
	{
		AppFrameStartTime(app);
		AppHandleEvents(app);
		app.loopFlag = GameUpdate(game, app.dt);
		GameDraw(game);
		SDL_RenderPresent(ren);
		AppDelay(app);
	} while (app.loopFlag != TOMENU);
	app.loopFlag = MENU;
}

void MenuLoop(App& app)
{
	bool isInGameMenu = CurrMenuType(app) == INGAMEMENU;
	Sint8(*UpdateFunc)(Menu&) = CurrMenuType(app) == SETTINGSMENU ? SettingsMenuUpdate : MenuUpdate;
	Menu& menu = *app.currMenu;
	do
	{
		AppFrameStartTime(app);
		AppHandleEvents(app);
		app.loopFlag = UpdateFunc(menu);
		if (isInGameMenu)
			GameDraw(app.game);
		MenuDraw(menu);
		SDL_RenderPresent(ren);
		AppDelay(app);
	} while (app.loopFlag == -1);
	app.restartGame = app.menuLeave[CurrMenuType(app)](app.currMenu, app.loopFlag);
}

void AppRun(App& self)
{
	if (self.loopFlag == GAME)
	{
		GameLoop(self);
		return;
	}
	if (self.loopFlag == MENU)
	{
		MenuLoop(self);
		return;
	}
	Quit();
}

int main(int argc, char** argv)
{
	Init();
	DisplayInit(1400, 800, "Blade");
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	App app;
	while (true)
	{
		AppRun(app);
	}
	return 0;
}