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
	
	self.verMS += self.vVel;
	self.pos.y += self.verMS;
	self.isInAir = true;

	self.rect.x = self.pos.x;
	self.rect.y = self.pos.y;
}

void EntityMoveTo(Entity& self, const SDL_FPoint dest)
{
	self.rect.x = self.pos.x = dest.x;
	self.rect.y = self.pos.y = dest.y;
}

float EntityGetHorMid(const Entity& self)
{
	return self.pos.x + self.rect.w / 2;
}

float EntityGetVerMid(const Entity& self)
{
	return self.pos.y + self.rect.h / 2;
}