#include "Projectile.h"

void ProjectilePlatformVerCollision(Projectile& self, Platform& platform)
{
	if (!self.ent.isMoving || !platform.vCollCheck)
		return;
	if (!SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	self.isPickable = self.ent.isMoving = false;
	SDL_FPoint moveTo{ self.ent.pos.x, platform.rect.y };
	if (EntityGetVerMid(self.ent) <= RectGetVerMid(platform.rect))
		moveTo.y -= self.ent.rect.h * 0.8f;
	else
		moveTo.y += platform.rect.h - self.ent.rect.h * 0.2f;
	EntityMoveTo(self.ent, moveTo);
}

void ProjectilePlatformHorCollision(Projectile& self, Platform& platform)
{
	if (!self.ent.isMoving || !platform.hCollCheck)
		return;
	if (!SDL_HasIntersection(&self.ent.rect, &platform.rect))
		return;
	self.isPickable = self.ent.isMoving = false;
	SDL_FPoint moveTo{ platform.rect.x, self.ent.pos.y };
	if (EntityGetHorMid(self.ent) <= RectGetHorMid(platform.rect))
		moveTo.x -= self.ent.rect.w * 0.8f;
	else
		moveTo.x += platform.rect.w - self.ent.rect.w * 0.2f;
	EntityMoveTo(self.ent, moveTo);
}

void ProjectilePickUp(Projectile& self)
{
	self.isPickable = self.ent.isMoving = self.wasThrown = false;
}

void ProjectileUpdate(Projectile& self)
{
	if (!self.wasThrown || self.isPickable)
		return;
	if (self.ent.isMoving)
	{
		EntityUpdate(self.ent);
		return;
	}
	if (self.currPickCD > 0)
		self.currPickCD -= FRAME_DELAY;
	else
	{
		self.currPickCD = self.pickCD;
		self.isPickable = true;
	}
}

void ProjectileDraw(const Projectile& self)
{
	if (!self.wasThrown)
		return;
	SDL_SetRenderDrawColor(ren, 100, 100, 0, 255);
	SDL_RenderFillRect(ren, &self.ent.rect);
}