#include "Entity.h"

void EntityUpdate(Entity& self)
{
	if (self.isMoving)
	{
		if (self.isInAir)
			self.currMS += self.hVel * self.dir * 0.9f;
		else
			self.currMS += self.hVel * self.dir;
		if (fabsf(self.currMS) > self.maxMS)
			self.currMS = self.maxMS * self.dir;
	}
	else if (!self.isInAir)
	{
		self.currMS -= self.hVel * self.dir;
		if (self.currMS * self.dir < 0)
			self.currMS = 0;
	}
	self.pos.x += self.currMS;
	
	self.vVel += self.weight;
	self.pos.y += self.vVel;
	self.isInAir = true;

	self.rect.x = self.pos.x;
	self.rect.y = self.pos.y;
}

void EntityMoveTo(Entity& self, const SDL_Point dest)
{
	self.pos.x = self.rect.x = dest.x;
	self.pos.y = self.rect.y = dest.y;
}

int EntityGetHorMid(const Entity& self)
{
	return RectGetHorMid(self.rect);
}

int EntityGetVerMid(const Entity& self)
{
	return RectGetVerMid(self.rect);
}