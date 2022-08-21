#include "Platform.h"

void PlatformDraw(const Platform& self)
{
	SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
	SDL_Rect drawRect = RectTransformForCurrWin(self.rect);
	SDL_RenderFillRect(ren, &drawRect);
}