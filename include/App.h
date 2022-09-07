#pragma once

#include "SDL_default.h"
#include "Game.h"
#include "Menus.h"

typedef enum Loop
{
	GAME,
	MENU,
	QUIT
} Loop;

typedef struct App
{
	SDL_Event ev;
	SDL_DisplayMode display;
	SDL_Point wModeRes;
	Uint32 lastTime = 0, currTime = 0, lastRenderedTime = 0, dt = 0;
	Game game;
	Menu menu;
	Sint8 loopFlag = MENU;
	bool restartGame = true, isFullscreen = false;
	char characters[2][20]{ "Fantasy Warrior", "Medieval Warrior" };
	char maps[2][10]{ "map1", "map2" };
	int map = NONE, p1Char = NONE, p2Char = NONE;
	void (*MenuLeaveFuncs[4])(App&);
} App;

void AppChangeResolution(App&, const SDL_Point& resolution);
void AppInit(App&);

void AppRestartTime(App&);
void AppFrameStartTime(App&);
void AppDelay(App&);

void AppHandleEvents(App&);

void AppMenuDraw(App&);
void AppInGameMenuDraw(App&);

void GameLoop(App&);
void MenuLoop(App&);

bool AppRun(App&);

void ChoiceMenuLeave(App&);
void SettingsMenuLeave(App&);
void InGameMenuLeave(App&);
void MainMenuLeave(App&);