#include "Projectile.h"

void ProjectilePlatformVerCollision(Projectile& self, const Platform& platform)
{
	if (!self.ent.isMoving || !platform.vCollCheck || !SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	self.isPickable = self.ent.isMoving = false;
	SDL_FPoint moveTo{ self.ent.pos.x, platform.rect.y };
	if (self.ent.verMS > 0 && int(EntityGetVerMid(self.ent) - self.ent.verMS) < platform.rect.y)
		moveTo.y -= self.ent.rect.h * 0.8f;
	else
		moveTo.y += platform.rect.h - self.ent.rect.h * 0.2f;
	EntityMoveTo(self.ent, moveTo);
}

void ProjectilePlatformHorCollision(Projectile& self, const Platform& platform)
{
	if (!self.ent.isMoving || !platform.hCollCheck || !SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	self.isPickable = self.ent.isMoving = false;
	SDL_FPoint moveTo{ platform.rect.x, self.ent.pos.y };
	if (self.ent.currMS > 0)
	{
		if (int(self.ent.pos.x + self.ent.rect.w - self.ent.currMS) > platform.rect.x)
			return;
		moveTo.x -= self.ent.rect.w * 0.8f;
	}
	else
	{
		if (int(self.ent.pos.x - self.ent.currMS) < platform.rect.x + platform.rect.w)
			return;
		moveTo.x += platform.rect.w - self.ent.rect.w * 0.2f;
	}
	EntityMoveTo(self.ent, moveTo);
}

void ProjectilePickUp(Projectile& self)
{
	self.isPickable = self.ent.isMoving = self.wasThrown = false;
}

void ProjectileUpdate(Projectile& self, Uint16 dt)
{
	if (!self.wasThrown || self.isPickable)
		return;
	if (self.ent.isMoving)
	{
		EntityUpdate(self.ent);
		return;
	}
	self.currPickCD -= dt;
	if (self.currPickCD > 0)
		return;
	self.currPickCD = self.pickCD;
	self.isPickable = true;
}

void ProjectileDraw(const Projectile& self)
{
	if (!self.wasThrown)
		return;
	SDL_Rect drawRect = self.ent.rect;
	drawRect.x = ceilf(drawRect.x * scale);
	drawRect.y = ceilf(drawRect.y * scale);
	drawRect.h = ceilf(drawRect.h * scale);
	drawRect.w = ceilf(drawRect.w * scale);
	SDL_SetRenderDrawColor(ren, 100, 100, 0, 255);
	SDL_RenderFillRect(ren, &drawRect);
}