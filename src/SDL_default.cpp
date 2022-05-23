#include <stdio.h>

#include "SDL_default.h"

SDL_Window* win;
SDL_Renderer* ren;
int winW, winH;
const Uint8* kbState = SDL_GetKeyboardState(NULL);

void Init(Uint32 flags)
{
	if (SDL_Init(flags)) {
		printf_s("Coudn't init SDL! Error: %s", SDL_GetError());
		system("pause");
		Quit(1);
	}

	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
	{
		printf_s("Coudn't init IMG! Error: %s", SDL_GetError());
		system("pause");
		Quit(1);
	}
}

void Quit(int error)
{
	if (ren != NULL)
		SDL_DestroyRenderer(ren);
	if (win != NULL)
		SDL_DestroyWindow(win);
	IMG_Quit();
	SDL_Quit();
	exit(error);
}

void DisplayInit(int win_w, int win_h, const char* name, SDL_WindowFlags flags) {
	win = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, flags);
	if (win == NULL) {
		printf_s("Couldn't create window! Error: %s", SDL_GetError());
		system("pause");
		Quit(1);
	}
	winW = win_w;
	winH = win_h;

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL) {
		printf_s("Couldn't create renderer! Error: %s", SDL_GetError());
		system("pause");
		Quit(1);
	}
}

void DisplayQuit() {
	if (ren != NULL)
		SDL_DestroyRenderer(ren);
	if (win != NULL)
		SDL_DestroyWindow(win);
}

void ScreenFill(int r, int g, int b, int a)
{
	SDL_SetRenderDrawColor(ren, r, g, b, a);
	SDL_RenderClear(ren);
}

int RectGetVerMid(const SDL_Rect& self)
{
	return self.y + self.h / 2;
}

int RectGetHorMid(const SDL_Rect& self)
{
	return self.x + self.w / 2;
}

bool OnPress(const SDL_Scancode& key)
{
	static Uint8 kbLast[286]{ 0 };
	if (!kbState[key])
	{
		kbLast[key] = 0;
		return false;
	}
	if (!kbLast[key])
	{
		kbLast[key] = 1;
		return true;
	}
	return false;
}

SDL_Texture* LoadImage(const char* file, SDL_Rect& rect)
{
	SDL_Surface* surface = IMG_Load(file);
	if (surface == NULL)
	{
		printf("Coudln't load image %s! Error: %s", file, SDL_GetError());
		Quit(1);
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	rect = { 0, 0, surface->w, surface->h };
	SDL_FreeSurface(surface);
	return texture;
}
