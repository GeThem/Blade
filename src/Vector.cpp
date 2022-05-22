#include <math.h>

#include "Vector.h"

double VectorMagnitude(Vector* self)
{
	return sqrt(self->x * self->x + self->y * self->y);
}

bool VectorNormalize(Vector& self)
{
	double length = VectorMagnitude(&self);
	if (!length)
		return false;
	self.x /= length;
	self.y /= length;
	return true;
}