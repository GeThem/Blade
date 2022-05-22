#pragma once

#define FRAME_DELAY 16.667f
#define FPS 60

#include <SDL.h>
#include <SDL_image.h>

extern SDL_Window* win;
extern SDL_Renderer* ren;
extern int win_w, win_h;

void init(Uint32 flags = SDL_INIT_VIDEO);

void quit(int error = 1);

void display_init(int win_w, int win_h, const char* name = "A window", Uint32 flags = SDL_WINDOW_SHOWN);

void display_quit();

void screen_fill(int r, int g, int b, int a = 255);

int RectGetVerMid(const SDL_Rect&);
int RectGetHorMid(const SDL_Rect&);

SDL_Texture* load_img(const char* file, SDL_Rect& rect);