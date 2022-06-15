#include "SDL_default.h"
#include "Menus.h"

void MenuFrameStartTime(Menu& self)
{
	Uint32 currTime = SDL_GetTicks();
	Uint32 dt = currTime - self.lastTime;
	self.currTime += dt;
	self.lastTime = currTime;
}

void MenuDelay(Menu& self)
{
	Uint32 frameTime = SDL_GetTicks() - self.lastTime;
	if (FRAME_DELAY > frameTime)
		SDL_Delay(FRAME_DELAY - frameTime);
}

void MenuHandleEvents(Menu& self)
{
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

Menu MainMenuInit()
{
	TTF_Font* font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", 80);
	Menu self;
	SDL_Color states[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.Background = { 60, 60, 60, 255 };
	self.escReturn = MM_EXIT;
	self.textButtonsCount = 3;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[MM_PLAY] = TextButtonInit({ winW / 2 - 350, winH / 2 - 170, 700, 100 }, "PLAY", font, states);
	self.textButtons[MM_SETTINGS] = TextButtonInit({ winW / 2 - 350, winH / 2 - 50, 700, 100 }, "SETTINGS", font, states);
	self.textButtons[MM_EXIT] = TextButtonInit({ winW / 2 - 350, winH / 2 + 70, 700, 100 }, "EXIT", font, states);
	TTF_CloseFont(font);
	return self;
}

Menu InGameMenuInit()
{
	TTF_Font* font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", 80);
	Menu self;
	SDL_Color states[3]{ {100, 100, 100, 100}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.Background = { 60, 60, 60, 150 };
	self.escReturn = IG_RESUME;
	self.textButtonsCount = 3;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[IG_RESUME] = TextButtonInit({ winW / 2 - 350, winH / 2 - 170, 700, 100 }, "RESUME", font, states);
	self.textButtons[IG_RESTART] = TextButtonInit({ winW / 2 - 350, winH / 2 - 50, 700, 100 }, "RESTART", font, states);
	self.textButtons[IG_BACKTOMENU] = TextButtonInit({ winW / 2 - 350, winH / 2 + 70, 700, 100 }, "BACK TO MENU", font, states);
	TTF_CloseFont(font);
	return self;
}

Menu SettingsMenuInit()
{
	Menu self;
	self.escReturn == SM_BACK;
	self.textButtonsCount = 0;
	return self;
}

Sint8 MenuUpdate(Menu& self)
{
	MouseUpdate();
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	return -1;
}

void MenuDraw(const Menu& self)
{
	const SDL_Color& BG = self.Background;
	ScreenFill(BG.r, BG.g, BG.b, BG.a);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
}