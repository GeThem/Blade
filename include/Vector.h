#pragma once

struct Vector
{
	double x, y;
};

double VectorMagnitude(Vector*);

//Normalizes the vector.
//Returns false if the vector is a null vector, true otherwise.
bool VectorNormalize(Vector&);