#include <stdio.h>

#include "SDL_default.h"

SDL_Window* win;
SDL_Renderer* ren;
int winW, winH;
Keyboard kb;
Mouse mouse;

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

	if (TTF_Init())
	{
		printf_s("Coudn't init TTF! Error: %s", SDL_GetError());
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
	TTF_Quit();
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

void ScreenFill(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(ren, r, g, b, a);
	SDL_Rect rect = { 0, 0, winW, winH };
	SDL_RenderFillRect(ren, &rect);
}

int RectGetVerMid(const SDL_Rect& self)
{
	return self.y + self.h / 2;
}

int RectGetHorMid(const SDL_Rect& self)
{
	return self.x + self.w / 2;
}

void KeyboardUpdate()
{
	for (Uint16 i = 4; i < 287; i++)
		kb.stateLast[i] = kb.state[i];
}

bool OnKeyPress(const SDL_Scancode& key)
{
	return kb.state[key] && !kb.stateLast[key];
}

void MouseUpdate()
{
	mouse.buttonsLast = mouse.buttons;
	mouse.buttons = SDL_GetMouseState(&mouse.pos.x, &mouse.pos.y);
}

bool OnButtonRelease(const Uint8 buttonMask)
{
	return !(mouse.buttons & buttonMask) && mouse.buttonsLast & buttonMask;
}

bool OnClick(const Uint8 buttonMask)
{
	return mouse.buttons & buttonMask && !(mouse.buttonsLast & buttonMask);
}

Image LoadImage(const char* filename)
{
	Image image;
	image.texture = LoadTexture(filename, &image.rect);
	return image;
}

SDL_Texture* LoadTexture(const char* filename, SDL_Rect* rect)
{
	SDL_Surface* surface = IMG_Load(filename);
	if (surface == NULL)
	{
		printf("Coudln't load image %s! Error: %s", filename, SDL_GetError());
		Quit(1);
	}
	if (rect != NULL)
	{
		*rect = { 0, 0, surface->w, surface->h };
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void RenderText(Image& textImg, TTF_Font* font, const char* string, const SDL_Color& color)
{
	if (textImg.texture != NULL)
		SDL_DestroyTexture(textImg.texture);
	SDL_Surface* surface = TTF_RenderText_Blended(font, string, color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	textImg.texture = texture;
	textImg.rect.w = surface->w;
	textImg.rect.h = surface->h;
	SDL_FreeSurface(surface);
}