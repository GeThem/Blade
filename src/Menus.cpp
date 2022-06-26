#include "SDL_default.h"
#include "Menus.h"

Menu MainMenuInit()
{
	Menu self;
	self.Background = { 60, 60, 60, 255 };
	self.escReturn = MM_EXIT;
	
	TTF_Font* font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", scale * 80);
	SDL_Color states[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 3;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[MM_PLAY] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int( scale * (realH / 2 - 170)), int(scale * 700), int (scale* 100) }, "PLAY", font, states);
	self.textButtons[MM_SETTINGS] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int( scale * (realH / 2 - 50)), int(scale * 700), int (scale* 100) }, "SETTINGS", font, states);
	self.textButtons[MM_EXIT] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int( scale * (realH / 2 + 70)), int(scale * 700), int (scale* 100) }, "EXIT", font, states);
	TTF_CloseFont(font);
	
	return self;
}

Menu InGameMenuInit()
{
	Menu self;
	self.Background = { 60, 60, 60, 150 };
	self.escReturn = IG_RESUME;
	
	TTF_Font* font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", scale * 80);
	SDL_Color states[3]{ {100, 100, 100, 100}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 3;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[IG_RESUME] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int(scale * (realH / 2 - 170)), int(scale * 700), int(scale * 100) }, "RESUME", font, states);
	self.textButtons[IG_RESTART] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int(scale * (realH / 2 - 50)), int(scale * 700), int(scale * 100) }, "RESTART", font, states);
	self.textButtons[IG_BACKTOMENU] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 350)), crd0.y + int(scale * (realH / 2 + 70)), int(scale * 700), int(scale * 100) }, "BACK TO MENU", font, states);
	TTF_CloseFont(font);
	
	return self;
}

Menu SettingsMenuInit()
{
	Menu self;
	self.escReturn == SM_BACK;
	self.Background = { 60, 60, 60, 255 };

	TTF_Font* font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", scale * 70);
	SDL_Color TBStates[3]{ {100, 100, 100, 255}, {196, 22, 22, 255}, {22, 196, 22, 255} };
	self.textButtonsCount = 2;
	self.textButtons = (TextButton*)malloc(sizeof(TextButton) * self.textButtonsCount);
	self.textButtons[SM_SAVE] = TextButtonInit({ crd0.x + int(scale * (realW / 2 - 600)), crd0.y + int(scale * (realH / 2 + 250)), int(scale * 500), int(scale * 100)}, "SAVE", font, TBStates);
	self.textButtons[SM_BACK] = TextButtonInit({ crd0.x + int(scale * (realW / 2 + 100)), crd0.y + int(scale * (realH / 2 + 250)), int(scale * 500), int(scale * 100)}, "BACK", font, TBStates);
	TTF_CloseFont(font);
	
	font = TTF_OpenFont("data/fonts/JetBrainsMono-Bold.ttf", scale * 50);
	self.switchButtonsCount = 1;
	SDL_Color SBStates[4]{ {100, 100, 100, 255}, {120, 120, 120, 255}, {180, 180, 180, 255}, {200, 200, 200, 255} };
	self.switchButtons = (SwitchButton*)malloc(sizeof(SwitchButton) * self.switchButtonsCount);
	self.switchButtons[0] = SwitchButtonInit({ crd0.x + int(scale * (realW / 2 - 400)), crd0.y + int(scale * (realH / 2 - 250)), int(scale * 45), int(scale * 45)}, "Fullscreen", font, SBStates);
	TTF_CloseFont(font);
	
	return self;
}

Sint8 SettingsMenuUpdate(Menu& self)
{
	if (OnKeyPress(SDL_SCANCODE_ESCAPE))
		return self.escReturn;
	SwitchButtonUpdate(self.switchButtons[0]);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
	{
		if (TextButtonUpdate(self.textButtons[i]))
			return i;
	}
	return -1;
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
	const SDL_Color& BG = self.Background;
	ScreenFill(BG.r, BG.g, BG.b, BG.a);
	for (Uint8 i = 0; i < self.switchButtonsCount; i++)
		SwitchButtonDraw(self.switchButtons[i]);
	for (Uint8 i = 0; i < self.textButtonsCount; i++)
		TextButtonDraw(self.textButtons[i]);
}