#include <stdio.h>

#include "SDL_default.h"

SDL_Window* win;
SDL_Renderer* ren;
int win_w, win_h;

void init(Uint32 flags)
{
	if (SDL_Init(flags)) {
		printf_s("Coudn't init SDL! Error: %s", SDL_GetError());
		system("pause");
		quit(1);
	}

	if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
	{
		printf_s("Coudn't init IMG! Error: %s", SDL_GetError());
		system("pause");
		quit(1);
	}
}

void quit(int error)
{
	if (ren != NULL)
		SDL_DestroyRenderer(ren);
	if (win != NULL)
		SDL_DestroyWindow(win);
	IMG_Quit();
	SDL_Quit();
	exit(error);
}

void display_init(int win_w1, int win_h1, const char* name, Uint32 flags) {
	win = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w1, win_h1, flags);
	if (win == NULL) {
		printf_s("Couldn't create window! Error: %s", SDL_GetError());
		system("pause");
		quit(1);
	}
	win_w = win_w1;
	win_h = win_h1;

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == NULL) {
		printf_s("Couldn't create renderer! Error: %s", SDL_GetError());
		system("pause");
		quit(1);
	}
}

void display_quit() {
	if (ren != NULL)
		SDL_DestroyRenderer(ren);
	if (win != NULL)
		SDL_DestroyWindow(win);
}

void screen_fill(int r, int g, int b, int a)
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

SDL_Texture* load_img(const char* file, SDL_Rect& rect)
{
	SDL_Surface* surface = IMG_Load(file);
	if (surface == NULL)
	{
		printf("Coudln't load image %s! Error: %s", file, SDL_GetError());
		quit(1);
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	rect = { 0, 0, surface->w, surface->h };
	SDL_FreeSurface(surface);
	return texture;
}
