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

void ProjectilePlatformVerCollision(Projectile&, Platform& platform);
void ProjectilePlatformHorCollision(Projectile&, Platform& platform);

void ProjectilePickUp(Projectile&);

void ProjectileUpdate(Projectile&);

void ProjectileDraw(const Projectile&);