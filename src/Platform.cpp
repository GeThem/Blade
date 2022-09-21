#include "Platform.h"

void PlatformInit(Platform& self, const SDL_Rect& rect, bool hColl, bool vColl, bool isDismountable)
{
	self.rect = rect;
	self.hCollCheck = hColl;
	self.vCollCheck = vColl;
	self.isDismountable = isDismountable;
}