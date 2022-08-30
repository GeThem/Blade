#include "Buttons.h"

TextButton TextButtonInit(const SDL_Rect& rect, const char* text, TTF_Font* font, const SDL_Color(&butStates)[3])
{
	TextButton self{ rect };
	RenderText(self.textImg, font, text, { 200, 200, 200, 255 });
	self.textImg.rect.x = self.rect.x + (self.rect.w - self.textImg.rect.w) / 2;
	self.textImg.rect.y = self.rect.y + (self.rect.h - self.textImg.rect.h) / 2;
	for (int i = 0; i < 3; i++)
		self.butStates[i] = butStates[i];
	self.currState = self.butStates;
	return self;
}

bool TextButtonUpdate(TextButton& self)
{
	SDL_Rect rect = RectTransformForCurrWin(self.rect);
	if (!SDL_PointInRect(&mouse.pos, &rect))
	{
		self.currState = self.butStates;
		return false;
	}
	if (!OnButtonRelease(SDL_BUTTON_LMASK))
	{
		self.currState = self.butStates + (mouse.buttons & SDL_BUTTON_LMASK ? PRESSED : ACTIVE);
		return false;
	}
	return true;
}

void TextButtonDraw(const TextButton& self)
{
	SDL_Color* c = self.currState;
	SDL_SetRenderDrawColor(ren, c->r, c->g, c->b, c->a);
	SDL_Rect drawRect = RectTransformForCurrWin(self.rect);
	SDL_RenderFillRect(ren, &drawRect);
	SDL_SetTextureAlphaMod(self.textImg.texture, c->a);
	drawRect = RectTransformForCurrWin(self.textImg.rect);
	SDL_RenderCopy(ren, self.textImg.texture, NULL, &drawRect);
}

SwitchButton SwitchButtonInit(const SDL_Rect& rect, const char* text, TTF_Font* font, const SDL_Color(&butStates)[4])
{
	SwitchButton self{ rect };
	RenderText(self.textImg, font, text, { 200, 200, 200, 255 });
	self.textImg.rect.x = self.rect.x + self.rect.w + self.textImg.rect.h / 3;
	self.textImg.rect.y = self.rect.y + (self.rect.h - self.textImg.rect.h) / 2;
	for (int i = 0; i < 4; i++)
		self.butStates[i] = butStates[i];
	self.currState = self.butStates;
	return self;
}

bool SwitchButtonUpdate(SwitchButton& self)
{
	SDL_Rect rect = RectTransformForCurrWin(self.rect);
	if (!SDL_PointInRect(&mouse.pos, &rect))
	{
		self.currState = self.butStates + (self.isActivated ? PRESSED : INACTIVE);
		return self.isActivated;
	}
	if (!OnClick(SDL_BUTTON_LMASK))
	{
		self.currState = self.butStates + (self.isActivated ? PRESSED_ACTIVE : ACTIVE);
		return self.isActivated;
	}
	self.isActivated = !self.isActivated;
	self.currState = self.butStates + PRESSED_ACTIVE;
	return self.isActivated;
}

void SwitchButtonDraw(const SwitchButton& self)
{
	SDL_Color* c = self.currState;
	SDL_SetRenderDrawColor(ren, c->r, c->g, c->b, c->a);
	SDL_Rect drawRect = RectTransformForCurrWin(self.rect);
	SDL_RenderFillRect(ren, &drawRect);
	drawRect = RectTransformForCurrWin(self.textImg.rect);
	SDL_RenderCopy(ren, self.textImg.texture, NULL, &drawRect);
}