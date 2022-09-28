#include "vfx.h"

VanishText VanishTextGenerate(
	const char* text, TTF_Font* font, int size, const SDL_Color& color,
	float appearTime, float vanishTime, float existTime, TTF_Font* outline,
	bool isMoving
)
{
	VanishText self;
	self.isMoving = isMoving;
	self.alpha = appearTime != 0 ? 1 : 255;
	if (font == NULL)
	{
		font = TTF_OpenFont("data/fonts/PressStart2P-Regular.ttf", size);
		RenderText(self.txtImg, font, text, { color.r, color.g, color.b, 255 });
		TTF_CloseFont(font);
	}
	else
	{
		TTF_SetFontSize(font, size);
		if (outline)
			TTF_SetFontSize(outline, size);
		RenderText(self.txtImg, font, text, {color.r, color.g, color.b, 255}, outline);
	}
	self.finalSize = self.txtImg.rect.h;
	self.currentSize = self.finalSize * 5;

	self.appearRate = 51.5f;
	self.vanishRate = 255.0f / (vanishTime * FPS);
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
	if (self.existTime > 0 && self.currentSize != self.finalSize)
	{
		self.alpha = fminf(self.alpha + self.appearRate, 255);
		self.currentSize = fmaxf(self.currentSize - self.sizeDecreaseRate, self.finalSize);
		self.txtImg.rect.h = self.currentSize;
		self.txtImg.rect.w = self.currentSize * self.ratio;
		self.txtImg.rect.x = self.pos.x - self.txtImg.rect.w / 2;
		self.txtImg.rect.y = self.pos.y - self.txtImg.rect.h / 2;
	}
	else
	{
		if (self.isMoving)
			VanishTextSetPos(self, { self.pos.x, self.pos.y - 0.4f });
		if (self.existTime <= 0)
		{
			self.alpha = fmaxf(self.alpha - self.vanishRate, 0);
		}
		else
		{
			self.existTime -= dt;
			return;
		}
	}
	SDL_SetTextureAlphaMod(self.txtImg.texture, self.alpha);
}

void VanishTextSetPos(VanishText& self, const SDL_FPoint& pos)
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