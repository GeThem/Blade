#include "Platform.h"

void PlatformDraw(const Platform& self)
{
	SDL_Rect drawRect = self.rect;
	drawRect.x = ceilf(drawRect.x * scale);
	drawRect.y = ceilf(drawRect.y * scale);
	drawRect.w = ceilf(drawRect.w * scale);
	drawRect.h = ceilf(drawRect.h * scale);
	SDL_SetRenderDrawColor(ren, 100, 100, 100, 255);
	SDL_RenderFillRect(ren, &drawRect);
}