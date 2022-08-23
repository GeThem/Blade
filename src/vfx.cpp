#include "vfx.h"

VanishText VanishTextGenerate(
	const char* text, TTF_Font* font, const SDL_Color& color, float vVel, float vSpd,
	float hSpd, float appearTime, float vanishTime, float existTime
)
{
	VanishText self;
	self.vVel = vVel;
	self.hSpd = hSpd;
	self.vSpd = vSpd;
	self.appearTick = ceilf(255.0f / (appearTime * FPS));
	self.vanishTick = ceilf(255.0f / (vanishTime * FPS));
	self.existTime = existTime * 1000;
	self.alpha = color.a;

	//if (self.txtImg.texture != NULL)
	//	SDL_DestroyTexture(self.txtImg.texture);
	//SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	//TTF_SetFontOutline(font, 1);
	//SDL_Surface* surface_o = TTF_RenderText_Blended(font, text, { 0, 0, 0, 1 });
	//self.txtImg.rect.w = surface_o->w;
	//self.txtImg.rect.h = surface_o->h;
	//SDL_BlitSurface(surface_o, NULL, surface, &self.txtImg.rect);
	//SDL_Texture* texture = SDL_CreateTextureFromSurface(ren, surface);
	//self.txtImg.texture = texture;
	//SDL_SetTextureBlendMode(self.txtImg.texture, SDL_BLENDMODE_BLEND);
	//SDL_FreeSurface(surface_o);
	//SDL_FreeSurface(surface);
	//TTF_SetFontOutline(font, 0);

	RenderText(self.txtImg, font, text, color);
	return self;
}

void VanishTextUpdate(VanishText& self, const Uint16& dt)
{
	self.pos.x += self.hSpd;
	self.pos.y += self.vSpd;
	self.txtImg.rect.x = self.pos.x;
	self.txtImg.rect.y = self.pos.y;

	self.vSpd += self.vVel;
	if (self.existTime > 0 && self.alpha < 255)
	{
		self.alpha += self.appearTick;
		if (self.alpha > 255)
			self.alpha = 255;
	}
	else if (self.existTime <= 0)
	{
		self.alpha -= self.vanishTick;
		if (self.alpha < 0)
			self.alpha = 0;
	}
	else
	{
		self.existTime -= dt;
		return;
	}
	self.alpha;
	SDL_SetTextureAlphaMod(self.txtImg.texture, self.alpha);
}

void VanishTextSetPos(VanishText& self, const SDL_Point& pos)
{
	self.pos.x = self.txtImg.rect.x = pos.x;
	self.pos.y = self.txtImg.rect.y = pos.y;
}

void VanishTextDestroy(VanishText& self)
{
	ImageDestroy(self.txtImg);
}

void VanishTextDraw(const VanishText& self)
{
	SDL_Rect drawRect = RectTransformForCurrWin(self.txtImg.rect);
	SDL_RenderCopy(ren, self.txtImg.texture, NULL, &drawRect);
}