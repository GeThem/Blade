#pragma once

#include "SDL_default.h"
#include "Platform.h"
#include "Entity.h"

typedef struct Projectile
{
	Entity ent;
	bool isPickable = false, wasThrown = false;
	float atk;
	float pickCD, currPickCD;
} Projectile;

void ProjectilePlatformVerCollision(Projectile&, const Platform&);
void ProjectilePlatformHorCollision(Projectile&, const Platform&);

void ProjectilePickUp(Projectile&);

void ProjectileUpdate(Projectile&, Uint16 dt);

void ProjectileDraw(const Projectile&);