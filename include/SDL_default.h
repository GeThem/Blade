#pragma once

#define FPS 60
#define FRAME_DELAY (1000 / FPS)
#define realW 1920
#define realH 1080
#define FONT_OUTLINE_SIZE 1
#define NONE 100

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>

typedef struct Mouse
{
	SDL_Point pos;
	Uint8 buttons = SDL_GetMouseState(&pos.x, &pos.y), buttonsLast = 0;
} Mouse;

typedef struct Keyboard
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	Uint8 stateLast[287]{ 0 };
} Keyboard;

extern SDL_Window* win;
extern SDL_Renderer* ren;
extern int winW, winH;
extern Keyboard kb;
extern Mouse mouse;
extern float scale;
extern SDL_Point crd0;

typedef struct Image
{
	SDL_Texture* texture = NULL;
	SDL_Rect rect{ 0, 0 };
} Image;

void Init(Uint32 flags = SDL_INIT_VIDEO);

void Quit(int error = 1);

void DisplayInit(int win_w, int win_h, const char* name = "A window", SDL_WindowFlags flags = SDL_WINDOW_SHOWN);

void DisplayQuit();

void ScreenFill(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
void BlackStrips();

void KeyboardUpdate();
bool OnKeyPress(const SDL_Scancode&);
bool KeyHold(const SDL_Scancode& key);

void MouseUpdate();
bool OnButtonRelease(const Uint8 buttonMask);
bool OnClick(const Uint8 buttonMask);

int RectGetVerMid(const SDL_Rect&);
int RectGetHorMid(const SDL_Rect&);

Image ImageLoad(const char* filename);

SDL_Texture* LoadTexture(const char* filename, SDL_Rect*);

void RenderText(Image& textImg, TTF_Font* font, const char* string, const SDL_Color&, TTF_Font* outline=NULL);

void ImageDestroy(Image&);

SDL_Rect RectTransformForCurrWin(SDL_Rect);
void RectSetPos(SDL_Rect&, int x, int y);