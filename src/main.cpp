#include "Game.h"
#include "Menus.h"

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
	char characters[2][20] { "Fantasy Warrior", "" };
} App;

void AppChangeResolution(App& self, const SDL_Point& resolution)
{
	winW = resolution.x;
	winH = resolution.y;
	SDL_SetWindowSize(win, winW, winH);
	//SDL_SetWindowPosition(win, (self.display.w - winW) / 2, (self.display.h - winH) / 2);
	double x = fmin(winW / 16.0, winH / 9.0);
	scale = x / (realH / 9);
	crd0 = { (winW - int(16 * x)) / 2, (winH - int(9 * x)) / 2 };
}

void AppInit(App& self)
{
	srand(time(0));
	Init();
	SDL_GetDesktopDisplayMode(0, &self.display);
	DisplayInit(1920, 1080, "Blade", SDL_WindowFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	double x = fmin(winW / 16.0, winH / 9.0);
	scale = x / (realH / 9);
	crd0 = { (winW - int(16 * x)) / 2, (winH - int(9 * x)) / 2 };
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
		case SDL_WINDOWEVENT:
			switch (self.ev.window.event)
		case SDL_WINDOWEVENT_RESIZED:
			{
				int w, h;
				SDL_GetRendererOutputSize(ren, &w, &h);
				AppChangeResolution(self, { w, h });
				break;
			}
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

		GameDraw(app.game);
		BlackStrips();
		SDL_RenderPresent(ren);

		AppHandleEvents(app);
		app.loopFlag = GameUpdate(app.game, app.dt);

		AppDelay(app);
	} while (app.loopFlag != TOMENU);
	app.loopFlag = MENU;
}

void AppMenuDraw(App& self)
{
	MenuDraw(*self.currMenu);
}

void AppInGameMenuDraw(App& self)
{
	GameDraw(self.game);
	MenuDraw(*self.currMenu);
}

void MenuLoop(App& app)
{
	bool isSettingsMenu = CurrMenuType(app) == SETTINGSMENU;
	void(*DrawFunc)(App&) = CurrMenuType(app) == INGAMEMENU ? AppInGameMenuDraw : AppMenuDraw;
	do
	{
		AppFrameStartTime(app);
		AppHandleEvents(app);
		app.loopFlag = app.currMenu->UpdateFunc(*app.currMenu);
		DrawFunc(app);
		BlackStrips();
		SDL_RenderPresent(ren);
		AppDelay(app);
	} while (app.loopFlag == -1);
	if (isSettingsMenu && app.loopFlag == SM_SAVE)
	{
		if (app.currMenu->switchButtons[0].isActivated)
			SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
		{
			SDL_SetWindowFullscreen(win, 0);
			AppChangeResolution(app, { 1920, 1080 });
		}
	}
	if (CurrMenuType(app) == MAINMENU && app.loopFlag == GAME)
	{
		GameInit(app.game, app.characters[0], app.characters[0], "");
	}
	if (CurrMenuType(app) == INGAMEMENU && app.loopFlag == IG_BACKTOMENU)
	{
		GameClose(app.game);
	}
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