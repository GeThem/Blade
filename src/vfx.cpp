#include "vfx.h"

VanishText VanishTextGenerate(
	const char* text, TTF_Font* font, int size, const SDL_Color& color,
	float appearTime, float vanishTime, float existTime
)
{
	VanishText self;
	self.alpha = 1;
	TTF_SetFontSize(font, size);
	RenderText(self.txtImg, font, text, {color.r, color.g, color.b, 255});
	self.finalSize = self.txtImg.rect.h;
	self.currentSize = self.finalSize * 5;

	self.appearRate = ceilf(255.0f / (appearTime * FPS));
	self.vanishRate = ceilf(255.0f / (vanishTime * FPS));
	self.sizeDecreaseRate = ceilf((self.currentSize - self.finalSize) / float(appearTime * FPS));
	self.existTime = existTime * 1000;

	SDL_SetTextureAlphaMod(self.txtImg.texture, self.alpha);
	self.ratio = self.txtImg.rect.w / (float)self.txtImg.rect.h;
	self.txtImg.rect.h = self.currentSize;
	self.txtImg.rect.w = self.currentSize * self.ratio;
	return self;
}

void VanishTextUpdate(VanishText& self, const Uint16& dt)
{
	if (self.existTime > 0 && self.alpha < 255)
	{
		self.alpha += self.appearRate;
		if (self.alpha > 255)
			self.alpha = 255;
		self.currentSize -= self.sizeDecreaseRate;
		if (self.currentSize < self.finalSize)
			self.currentSize = self.finalSize;
		self.txtImg.rect.h = self.currentSize;
		self.txtImg.rect.w = self.currentSize * self.ratio;
		self.txtImg.rect.x = self.pos.x - self.txtImg.rect.w / 2;
		self.txtImg.rect.y = self.pos.y - self.txtImg.rect.h / 2;

	}
	else if (self.existTime <= 0)
	{
		self.alpha -= self.vanishRate;
		if (self.alpha < 0)
			self.alpha = 0;
	}
	else
	{
		self.existTime -= dt;
		return;
	}
	SDL_SetTextureAlphaMod(self.txtImg.texture, self.alpha);
}

void VanishTextSetPos(VanishText& self, const SDL_Point& pos)
{
	self.pos.x = pos.x;
	self.pos.y = pos.y;
	self.txtImg.rect.x = pos.x - self.txtImg.rect.w / 2;
	self.txtImg.rect.y = pos.y - self.txtImg.rect.h / 2;
}

void VanishTextDestroy(VanishText& self)
{
	ImageDestroy(self.txtImg);
}

void VanishTextDraw(const VanishText& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(self.txtImg.rect);
	SDL_RenderCopy(ren, self.txtImg.texture, NULL, &drawRect);
	Uint8 alpha;
	SDL_GetTextureAlphaMod(self.txtImg.texture, &alpha);
}