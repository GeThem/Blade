#include "App.h"

void AppChangeResolution(App& self, const SDL_Point& resolution)
{
	winW = resolution.x;
	winH = resolution.y;
	SDL_SetWindowSize(win, winW, winH);
	float x = fmin(winW / 16.0f, winH / 9.0f);
	scale = x / (realH / 9);
	crd0 = { (winW - int(16 * x)) / 2, (winH - int(9 * x)) / 2 };
}

void AppInit(App& self)
{
	self.MenuLeaveFuncs[0] = MainMenuLeave;
	self.MenuLeaveFuncs[1] = SettingsMenuLeave;
	self.MenuLeaveFuncs[2] = InGameMenuLeave;
	self.MenuLeaveFuncs[3] = ChoiceMenuLeave;
	srand(time(0));
	Init();
	SDL_GetDesktopDisplayMode(0, &self.display);
	float x = self.display.h * 0.75f / 9.0f;
	scale = x / (realH / 9);
	crd0 = { 0, 0 };
	self.wModeRes = { int(1920 * scale), int(1080 * scale) };
	DisplayInit(self.wModeRes.x, self.wModeRes.y, "Blade", SDL_WindowFlags(SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
	SettingsMenuInit(self.menu);
	if (self.menu.switchButtons[0].isActivated)
	{
		self.isFullscreen = true;
		SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
	}
	MenuClear(self.menu);
	MainMenuInit(self.menu);
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
	if (app.isFullscreen)
		SDL_ShowCursor(SDL_ENABLE);
}

void AppMenuDraw(App& self)
{
	self.menu.DrawFunc(self.menu);
}

void AppInGameMenuDraw(App& self)
{
	GameDraw(self.game);
	self.menu.DrawFunc(self.menu);
}

void MenuLoop(App& app)
{
	void(*DrawFunc)(App&) = app.menu.type == INGAMEMENU ? AppInGameMenuDraw : AppMenuDraw;
	do
	{
		AppFrameStartTime(app);

		DrawFunc(app);
		BlackStrips();
		SDL_RenderPresent(ren);

		AppHandleEvents(app);
		if (app.menu.type != MAPCHOICE && app.menu.type != CHARCHOICE)
		{
			app.loopFlag = app.menu.UpdateFunc(app.menu);
		}
		else
		{
			Sint8 val = app.menu.UpdateFunc(app.menu);
			if (val == MC_APPLY && !app.menu.activeChoiceButton)
				val = -1;
			app.loopFlag = val;
		}
		AppDelay(app);
	} while (app.loopFlag == -1);
	app.MenuLeaveFuncs[min(app.menu.type, 3)](app);
}

bool AppRun(App& self)
{
	if (self.loopFlag == GAME)
	{
		GameLoop(self);
		return true;
	}
	if (self.loopFlag == MENU)
	{
		MenuLoop(self);
		return true;
	}
	return false;
}

void MapChoiceInit(App& self)
{
	SDL_Color cbStates[2] = { {100, 100, 100, 255}, {200, 20, 20, 255} };
	ChoiceButton buttons[2] = {
		ChoiceButtonInit({ realW / 2 - 885, realH / 2 - 295, 870, 510 }, ImageLoad("data/maps/map1/preview.png"), cbStates, (int)MAP1),
		ChoiceButtonInit({ realW / 2 + 15, realH / 2 - 295, 870, 510 }, ImageLoad("data/maps/map2/preview.png"), cbStates, (int)MAP2)
	};
	ChoiceMenuInit(self.menu, "map select", 2, buttons, MAPCHOICE);
}

void CharChoiceInit(App& self, int charIndex)
{
	SDL_Color cbStates[2] = { {100, 100, 100, 255}, {20 + 150 * !charIndex, 20, 20 + 150 * charIndex, 255} };
	ChoiceButton buttons[2] = {
		ChoiceButtonInit({ realW / 3 - 255, realH / 2 - 295, 510, 510 }, ImageLoad("data/characters/Fantasy Warrior/preview.png"), cbStates, (int)CHAR1),
		ChoiceButtonInit({ realW * 2 / 3 - 255, realH / 2 - 295, 510, 510 }, ImageLoad("data/characters/Medieval Warrior/preview.png"), cbStates, (int)CHAR2)
	};
	char buffer[29];
	sprintf_s(buffer, "Player %i", charIndex + 1);
	ChoiceMenuInit(self.menu, buffer, 2, buttons, CHARCHOICE);
}

void ChoiceMenuLeave(App& self)
{
	if (self.loopFlag == MC_APPLY)
	{
		if (self.menu.type == CHARCHOICE && self.p1Char != NONE)
		{
			self.p2Char = self.menu.activeChoiceButton->index;
			GameInit(self.game, self.characters[self.p1Char], self.characters[self.p2Char], self.maps[self.map]);
			self.p1Char = self.p2Char = NONE;
			if (self.isFullscreen)
				SDL_ShowCursor(SDL_DISABLE);
			self.loopFlag = GAME;
			MenuClear(self.menu);
			InGameMenuInit(self.menu);
			self.restartGame = true;
			return;
		}
		if (self.menu.type == MAPCHOICE)
			self.map = self.menu.activeChoiceButton->index;
		else
			self.p1Char = self.menu.activeChoiceButton->index;
		self.loopFlag = MENU;
		MenuClear(self.menu);
		int charIndex = self.p1Char == NONE ? 0 : 1;
		CharChoiceInit(self, charIndex);
		return;
	}
	if (self.loopFlag == MC_BACK)
	{
		self.loopFlag = MENU;
		if (self.menu.type == MAPCHOICE)
		{
			MenuClear(self.menu);
			MainMenuInit(self.menu);
			return;
		}
		if (self.menu.type == CHARCHOICE)
		{
			if (self.p1Char == NONE)
			{
				MenuClear(self.menu);
				MapChoiceInit(self);
				self.menu.activeChoiceButton = self.menu.choiceButtons + self.map;
				self.menu.activeChoiceButton->isActivated = true;
				return;
			}
			MenuClear(self.menu);
			CharChoiceInit(self, 0);
			self.menu.activeChoiceButton = self.menu.choiceButtons + self.p1Char;
			self.menu.activeChoiceButton->isActivated = true;
			self.p1Char = NONE;
			return;
		}
	}
}

void MainMenuLeave(App& self)
{
	MenuClear(self.menu);
	if (self.loopFlag == MM_SETTINGS)
	{
		SettingsMenuInit(self.menu);
		self.loopFlag = MENU;
		return;
	}
	if (self.loopFlag == MM_PLAY)
	{
		MapChoiceInit(self);
		self.loopFlag = MENU;
		return;
	}
	self.loopFlag = QUIT;
}

void SettingsMenuLeave(App& app)
{
	if (app.loopFlag == SM_SAVE)
	{
		FILE* file, * tempFile;
		if (fopen_s(&tempFile, "data/temp____settings.txt", "w"))
			exit(0);
		const int tempS = 60;
		char temp[tempS];
		if (!fopen_s(&file, "data/settings.txt", "r"))
		{
			fgets(temp, tempS, file);
			char* val = temp;
			if (strstr(StrSplitInTwo(val, '='), "fullscreen"))
			{
				if (app.menu.switchButtons[0].isActivated != atoi(val))
				{
					if (app.menu.switchButtons[0].isActivated)
					{
						fprintf_s(tempFile, "fullscreen=1");
						AppChangeResolution(app, { app.display.w, app.display.h });
						SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);

						app.isFullscreen = true;
					}
					else
					{
						fprintf_s(tempFile, "fullscreen=0");
						SDL_SetWindowFullscreen(win, 0);
						AppChangeResolution(app, app.wModeRes);
						SDL_SetWindowPosition(win, (app.display.w - winW) / 2, (app.display.h - winH) / 2);
						app.isFullscreen = false;
					}
				}
				else
				{
					*--val = '=';
					fprintf_s(tempFile, temp);
				}
			}
			fclose(file);
			fclose(tempFile);
			remove("data/settings.txt");
			rename("data/temp____settings.txt", "data/settings.txt");
		}
	}
	MenuClear(app.menu);
	MainMenuInit(app.menu);
	app.loopFlag = MENU;
}

void InGameMenuLeave(App& self)
{
	if (self.loopFlag == IG_BACKTOMENU)
	{
		GameClose(self.game);
		MenuClear(self.menu);
		MainMenuInit(self.menu);
		self.loopFlag = MENU;
		self.restartGame = true;
		return;
	}
	if (self.isFullscreen)
		SDL_ShowCursor(SDL_DISABLE);
	self.restartGame = self.loopFlag == IG_RESTART;
	self.loopFlag = GAME;
}