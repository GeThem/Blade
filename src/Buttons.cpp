#include "Buttons.h"

TextButton TextButtonInit(const SDL_Rect& rect, const char* text, TTF_Font* font, const SDL_Color(&butStates)[3])
{
	TextButton self{ rect };
	RenderText(self.textImg, font, text, { 200, 200, 200, 255 });
	RectSetPos(self.textImg.rect, rect.x + (rect.w - self.textImg.rect.w) / 2, rect.y + (rect.h - self.textImg.rect.h) / 2);
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
	self.currState = self.butStates;
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

void TextButtonDestroy(TextButton& self)
{
	ImageDestroy(self.textImg);
	self.currState = NULL;
}

SwitchButton SwitchButtonInit(const SDL_Rect& rect, const char* text, TTF_Font* font, const SDL_Color(&butStates)[4])
{
	SwitchButton self{ rect };
	RenderText(self.textImg, font, text, { 200, 200, 200, 255 });
	RectSetPos(self.textImg.rect, rect.x + rect.w + self.textImg.rect.h / 3, rect.y + (rect.h - self.textImg.rect.h) / 2);
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

void SwitchButtonDestroy(SwitchButton& self)
{
	ImageDestroy(self.textImg);
	self.currState = NULL;
}

ChoiceButton ChoiceButtonInit(const SDL_Rect& rect, const Image& image, const SDL_Color(&butStates)[2], int index)
{
	ChoiceButton self{ rect };
	self.index = index;
	self.outlineRect = { rect.x - 5, rect.y - 5, rect.w + 10, rect.h + 10 };
	self.image = image;
	RectSetPos(self.image.rect, rect.x + (rect.w - image.rect.w) / 2, rect.y + (rect.h - image.rect.h) / 2);
	for (int i = 0; i < 2; i++)
		self.butStates[i] = butStates[i];
	return self;
}

bool ChoiceButtonUpdate(ChoiceButton& self)
{
	SDL_Rect rect = RectTransformForCurrWin(self.rect);
	return SDL_PointInRect(&mouse.pos, &rect) && OnClick(SDL_BUTTON_LMASK);
}

void ChoiceButtonDraw(const ChoiceButton& self)
{
	const SDL_Color* c;
	SDL_Rect drawRect;
	if (self.isActivated)
	{
		c = self.butStates + 1;
		SDL_SetRenderDrawColor(ren, c->r, c->g, c->b, c->a);
		drawRect = RectTransformForCurrWin(self.outlineRect);
		SDL_RenderFillRect(ren, &drawRect);
	}
	c = self.butStates;
	SDL_SetRenderDrawColor(ren, c->r, c->g, c->b, c->a);
	drawRect = RectTransformForCurrWin(self.rect);
	SDL_RenderFillRect(ren, &drawRect);
	drawRect = RectTransformForCurrWin(self.image.rect);
	SDL_RenderCopy(ren, self.image.texture, NULL, &drawRect);
}

void ChoiceButtonDestroy(ChoiceButton& self)
{
	ImageDestroy(self.image);
}