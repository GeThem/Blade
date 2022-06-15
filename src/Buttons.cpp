#include "SDL_default.h"
#include "Buttons.h"

TextButton TextButtonInit(SDL_Rect rect, const char* text, TTF_Font* font, const SDL_Color* butStates)
{
	TextButton self{ rect };
	RenderText(self.textImg, font, text, { 200, 200, 200, 255 });
	self.textImg.rect.x = self.rect.x + (self.rect.w - self.textImg.rect.w) / 2;
	self.textImg.rect.y = self.rect.y + (self.rect.h - self.textImg.rect.h) / 2;
	for (int i = 0; i < 3; i++)
		self.butStates[i] = butStates[i];
	self.currState = self.butStates;
	SDL_SetTextureBlendMode(self.textImg.texture, SDL_BLENDMODE_BLEND);
	return self;
}

bool TextButtonUpdate(TextButton& self)
{
	if (!SDL_PointInRect(&mouse.pos, &self.rect))
	{
		self.currState = self.butStates;
		return false;
	}
	if (!OnButtonRelease(SDL_BUTTON_LMASK))
	{
		self.currState = &self.butStates[mouse.buttons & SDL_BUTTON_LMASK ? PRESSED : ACTIVE];
		return false;
	}
	return true;
}

void TextButtonDraw(const TextButton& self)
{
	SDL_Color* c = self.currState;
	SDL_SetRenderDrawColor(ren, c->r, c->g, c->b, c->a);
	SDL_RenderFillRect(ren, &self.rect);
	SDL_SetTextureAlphaMod(self.textImg.texture, c->a);
	SDL_RenderCopy(ren, self.textImg.texture, NULL, &self.textImg.rect);
}

bool SwitchButtonUpdate(SwitchButton&);
void SwitchButtonDraw(const SwitchButton&);