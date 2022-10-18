#include "Menus.h"

void MainMenuInit(Menu& self)
{
	self.type = MAINMENU;
	self.UpdateFunc = MenuUpdate;
	self.DrawFunc = MenuDraw;

	self.background = { 60, 60, 60, 255 };
	self.escReturn = MM_EXIT;

	TTF_Font* font = TTF_OpenFont("data/fonts/slkscr.ttf", 80);
	SDL_Color states[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 4;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[MM_PLAY] = TextButtonInit({ realW / 2 - 350, realH / 2 - 230, 700, 100 }, "PLAY", font, states);
	self.textButtons[MM_SETTINGS] = TextButtonInit({ realW / 2 - 350, realH / 2 - 110, 700, 100 }, "SETTINGS", font, states);
	self.textButtons[MM_ABOUT] = TextButtonInit({ realW / 2 - 350, realH / 2 + 10, 700, 100 }, "ABOUT", font, states);
	self.textButtons[MM_EXIT] = TextButtonInit({ realW / 2 - 350, realH / 2 + 130, 700, 100 }, "EXIT", font, states);
	TTF_CloseFont(font);
}

void InGameMenuInit(Menu& self)
{
	self.type = INGAMEMENU;
	self.UpdateFunc = MenuUpdate;
	self.DrawFunc = MenuDraw;

	self.background = { 60, 60, 60, 150 };
	self.escReturn = IG_RESUME;

	TTF_Font* font = TTF_OpenFont("data/fonts/slkscr.ttf", 80);
	SDL_Color states[3]{ {100, 100, 100, 100}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 3;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[IG_RESUME] = TextButtonInit({ realW / 2 - 350, realH / 2 - 170, 700, 100 }, "RESUME", font, states);
	self.textButtons[IG_RESTART] = TextButtonInit({ realW / 2 - 350, realH / 2 - 50, 700, 100 }, "RESTART", font, states);
	self.textButtons[IG_BACKTOMENU] = TextButtonInit({ realW / 2 - 350, realH / 2 + 70, 700, 100 }, "BACK TO MENU", font, states);
	TTF_CloseFont(font);
}

void SettingsMenuInit(Menu& self)
{
	self.type = SETTINGSMENU;
	self.UpdateFunc = SettingsMenuUpdate;
	self.DrawFunc = SettingsMenuDraw;

	self.escReturn == SM_BACK;
	self.background = { 60, 60, 60, 255 };

	TTF_Font* font = TTF_OpenFont("data/fonts/slkscr.ttf", 70);
	SDL_Color TBStates[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 2;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[SM_SAVE] = TextButtonInit({ realW / 2 - 600, realH / 2 + 350, 500, 100 }, "SAVE", font, TBStates);
	self.textButtons[SM_BACK] = TextButtonInit({ realW / 2 + 100, realH / 2 + 350, 500, 100 }, "BACK", font, TBStates);

	TTF_SetFontSize(font, 50);
	self.switchButtonsCount = 1;
	SDL_Color SBStates[4]{ {100, 100, 100, 255}, {120, 120, 120, 255}, {180, 180, 180, 255}, {200, 200, 200, 255} };
	self.switchButtons = (SwitchButton*)malloc(sizeof(SwitchButton) * self.switchButtonsCount);
	self.switchButtons[0] = SwitchButtonInit({ realW / 2 - 400, realH / 2 - 250, 45, 45 }, "Fullscreen", font, SBStates);
	TTF_CloseFont(font);

	FILE* file, * tempFile;
	if (fopen_s(&tempFile, "data/temp____settings.txt", "w"))
		exit(0);
	const int tempSize = 60;
	char temp[tempSize];
	self.switchButtons[0].isActivated = false;
	int value = 0;
	if (!fopen_s(&file, "data/settings.txt", "r"))
	{
		fgets(temp, tempSize, file);
		char* val = temp;
		if (!strcmp(StrSplitInTwo(val, '='), "fullscreen"))
		{
			if (val)
			{
				value = atoi(val);
				self.switchButtons[0].isActivated = value;
			}
		}
		fclose(file);
	}
	fprintf_s(tempFile, "fullscreen=%i", value);
	fclose(tempFile);
	remove("data/settings.txt");
	rename("data/temp____settings.txt", "data/settings.txt");
}

Sint8 SettingsMenuUpdate(Menu& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	for (int i = 0; i < self.switchButtonsCount; i++)
		SwitchButtonUpdate(self.switchButtons[i]);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
	{
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	}
	return -1;
}

void SettingsMenuDraw(const Menu& self)
{
	const SDL_Color& bg = self.background;
	ScreenFill(bg.r, bg.g, bg.b, bg.a);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
	for (Uint8 i = 0; i < self.switchButtonsCount; i++)
		SwitchButtonDraw(self.switchButtons[i]);
}

Sint8 MenuUpdate(Menu& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
	{
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	}
	return -1;
}

void MenuDraw(const Menu& self)
{
	const SDL_Color& bg = self.background;
	ScreenFill(bg.r, bg.g, bg.b, bg.a);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
}

void ChoiceMenuInit(Menu& self, const char* text, int choiceButtonsCount, ChoiceButton* buttons, MenuType type)
{
	self.type = type;
	self.UpdateFunc = ChoiceMenuUpdate;
	self.DrawFunc = ChoiceMenuDraw;

	self.background = { 60, 60, 60, 255 };
	self.escReturn = MC_BACK;

	TTF_Font* font = TTF_OpenFont("data/fonts/slkscr.ttf", 70);
	SDL_Color states[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 2;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[MC_APPLY] = TextButtonInit({ realW / 2 - 600, realH / 2 + 350, 500, 100 }, "CONFIRM", font, states);
	self.textButtons[MC_BACK] = TextButtonInit({ realW / 2 + 100, realH / 2 + 350, 500, 100 }, "BACK", font, states);
	
	TTF_SetFontSize(font, 90);
	self.textsCount = 1;
	self.texts = (Image*)malloc(sizeof(Image) * self.textsCount);
	self.texts[0].texture = NULL;
	RenderText(self.texts[0], font, text, { 200, 200, 200, 255 });
	RectSetPos(self.texts[0].rect, (realW - self.texts[0].rect.w) / 2, 50);
	TTF_CloseFont(font);

	self.choiceButtonsCount = choiceButtonsCount;
	self.choiceButtons = (ChoiceButton*)malloc(sizeof(ChoiceButton) * self.choiceButtonsCount);
	for (int i = 0; i < choiceButtonsCount; i++)
	{
		self.choiceButtons[i] = buttons[i];
	}
}

Sint8 ChoiceMenuUpdate(Menu& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
	{
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	}

	for (int i = 0; i < self.choiceButtonsCount; i++)
	{
		if (ChoiceButtonUpdate(self.choiceButtons[i]))
		{
			if (self.activeChoiceButton)
			{
				self.activeChoiceButton->isActivated = !self.activeChoiceButton->isActivated;
				if (self.activeChoiceButton == self.choiceButtons + i)
				{
					self.activeChoiceButton = NULL;
					return -1;
				}
			}
			self.activeChoiceButton = self.choiceButtons + i;
			self.activeChoiceButton->isActivated = true;
			return -1;
		}
	}
	return -1;
}

void ChoiceMenuDraw(const Menu& self)
{
	const SDL_Color& bg = self.background;
	ScreenFill(bg.r, bg.g, bg.b, bg.a);
	for (int i = 0; i < self.textsCount; i++)
	{
		SDL_Rect drawRect = RectTransformForCurrWin(self.texts[i].rect);
		SDL_RenderCopy(ren, self.texts[i].texture, NULL, &drawRect);
	}
	for (int i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
	for (int i = 0; i < self.choiceButtonsCount; i++)
		ChoiceButtonDraw(self.choiceButtons[i]);
}

void AboutMenuInit(Menu& self)
{
	self.type = ABOUTMENU;
	self.UpdateFunc = AboutMenuUpdate;
	self.DrawFunc = AboutMenuDraw;

	self.background = { 60, 60, 60, 255 };
	self.escReturn = AM_BACK;

	TTF_Font* font = TTF_OpenFont("data/fonts/slkscr.ttf", 70);
	SDL_Color states[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 1;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[AM_BACK] = TextButtonInit({ realW / 2 - 250, realH / 2 + 350, 500, 100 }, "BACK", font, states);
	TTF_SetFontSize(font, 80);
	self.textsCount = 1;
	self.texts = (Image*)malloc(sizeof(Image) * self.textsCount);
	char text[] = "Hi, nothing here";
	SDL_Surface* surf = TTF_RenderUTF8_Solid_Wrapped(font, text, { 200, 200, 200, 255 }, realW - 400);
	self.texts[0].rect = { (realW - surf->w) / 2, (realH - surf->h) / 2 - 190, surf->w, surf->h };
	self.texts[0].texture = SDL_CreateTextureFromSurface(ren, surf);
	SDL_FreeSurface(surf);
	TTF_CloseFont(font);
}

Sint8 AboutMenuUpdate(Menu& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
	{
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	}
	return -1;
}

void AboutMenuDraw(const Menu& self)
{
	const SDL_Color& bg = self.background;
	ScreenFill(bg.r, bg.g, bg.b, bg.a);
	for (int i = 0; i < self.textsCount; i++)
	{
		SDL_Rect drawRect = RectTransformForCurrWin(self.texts[i].rect);
		SDL_RenderCopy(ren, self.texts[i].texture, NULL, &drawRect);
	}
	for (int i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
}

void MenuClear(Menu& self)
{
	if (self.textButtons) 
	{
		for (int i = 0; i < self.textButtonsCount; i++)
			TextButtonDestroy(self.textButtons[i]);
		free(self.textButtons);
		self.textButtons = NULL;
		self.textButtonsCount = 0;
	}
	if (self.switchButtons)
	{
		for (int i = 0; i < self.switchButtonsCount; i++)
			SwitchButtonDestroy(self.switchButtons[i]);
		free(self.switchButtons);
		self.switchButtons = NULL;
		self.switchButtonsCount = 0;
	}
	if (self.choiceButtons)
	{
		for (int i = 0; i < self.choiceButtonsCount; i++)
			ChoiceButtonDestroy(self.choiceButtons[i]);
		free(self.choiceButtons);
		self.choiceButtons = NULL;
		self.choiceButtonsCount = 0;
	}
	if (self.texts)
	{
		for (int i = 0; i < self.textsCount; i++)
			ImageDestroy(self.texts[i]);
		free(self.texts);
		self.texts = NULL;
		self.textsCount = 0;
	}
	self.activeChoiceButton = NULL;
	self.UpdateFunc = NULL;
	self.DrawFunc = NULL;
	self.type = MenuType(NONE);
}