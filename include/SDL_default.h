#pragma once

#define FRAME_DELAY 16.667f
#define FPS 60

#include <SDL.h>
#include <SDL_image.h>

extern SDL_Window* win;
extern SDL_Renderer* ren;
extern int winW, winH;
extern const Uint8* kbState;

void Init(Uint32 flags = SDL_INIT_VIDEO);

void Quit(int error = 1);

void DisplayInit(int win_w, int win_h, const char* name = "A window", SDL_WindowFlags flags = SDL_WINDOW_SHOWN);

void DisplayQuit();

void ScreenFill(int r, int g, int b, int a = 255);

bool OnPress(const SDL_Scancode&);

int RectGetVerMid(const SDL_Rect&);
int RectGetHorMid(const SDL_Rect&);

SDL_Texture* LoadImage(const char* file, SDL_Rect& rect);