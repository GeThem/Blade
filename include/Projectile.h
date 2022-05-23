#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Entity.h"

struct Projectile
{
	Entity ent;
	bool isPickable = false, wasThrown = false;
	float damage;
	float pickCD, currPickCD;
};

void ProjectilePlatformVerCollision(Projectile&, const Platform&);
void ProjectilePlatformHorCollision(Projectile&, const Platform&);

void ProjectilePickUp(Projectile&);

void ProjectileUpdate(Projectile&);

void ProjectileDraw(const Projectile&);