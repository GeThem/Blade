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
	SDL_DisplayMode display;
	Uint32 lastTime = 0, currTime = 0, lastRenderedTime = 0, dt = 0;
	Game game;
	Menu menus[3];
	bool (*menuLeave[3])(Menu*&, Sint8&){ MainMenuLeave, SettingsMenuLeave, InGameMenuLeave };
	Menu* currMenu = menus;
	Sint8 loopFlag = MENU;
	bool restartGame = true;
} App;

void AppInit(App& self)
{
	Init();
	SDL_GetDesktopDisplayMode(0, &self.display);
	DisplayInit(1600, 900, "Blade");
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	double x = fmin(winW / 16.0, winH / 9.0);
	scale = x / (realH / 9);
	crd0 = { (winW - int(16 * x)) / 2, (winH - int(9 * x)) / 2 };
	self.game = GameInit();
	self.menus[MAINMENU] = MainMenuInit();
	self.menus[SETTINGSMENU] = SettingsMenuInit();
	self.menus[INGAMEMENU] = InGameMenuInit();
}

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
	if (app.restartGame)
	{
		GameRestart(app.game);
		AppRestartTime(app);
	}
	do
	{
		AppFrameStartTime(app);
		AppHandleEvents(app);
		app.loopFlag = GameUpdate(app.game, app.dt);
		GameDraw(app.game);
		SDL_RenderPresent(ren);
		AppDelay(app);
	} while (app.loopFlag != TOMENU);
	app.loopFlag = MENU;
}

void AppMenuDraw(const App& self)
{
	MenuDraw(*self.currMenu);
}

void AppInGameMenuDraw(const App& self)
{
	GameDraw(self.game);
	MenuDraw(*self.currMenu);
}

void MenuLoop(App& app)
{
	Sint8(*UpdateFunc)(Menu&) = CurrMenuType(app) == SETTINGSMENU ? SettingsMenuUpdate : MenuUpdate;
	void(*DrawFunc)(const App&) = CurrMenuType(app) == INGAMEMENU ? AppInGameMenuDraw : AppMenuDraw;
	do
	{
		AppFrameStartTime(app);
		AppHandleEvents(app);
		app.loopFlag = UpdateFunc(*app.currMenu);
		DrawFunc(app);
		SDL_RenderPresent(ren);
		AppDelay(app);
	} while (app.loopFlag == -1);
	app.restartGame = app.menuLeave[CurrMenuType(app)](app.currMenu, app.loopFlag);
}

void AppRun(App& self)
{
	if (self.loopFlag == GAME)
		return GameLoop(self);
	if (self.loopFlag == MENU)
		return MenuLoop(self);
	Quit();
}

int main(int argc, char** argv)
{
	App app;
	AppInit(app);
	while (true)
	{
		AppRun(app);
	}
	return 0;
}