#include "Platform.h"

void PlatformDraw(const Platform& self)
{
	SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
	SDL_RenderFillRect(ren, &self.rect);
}