#pragma once

#include "Vect.h"

class Ray
{
public:

// Functions

	Ray() : _origin(0.0, 0.0, 0.0), _direction(1.0, 0.0, 0.0)
	{
	}

	Ray(Vect origin, Vect direction) : _origin(origin), _direction(direction)
	{
	}

	Vect GetOrigin()						{ return _origin; }
	Vect GetDirection()						{ return _direction; }

	void SetOrigin(const Vect & origin)		{ _origin = origin; }
	void SetDirection(const Vect & direction)	{ _direction = direction; }

	~Ray()
	{
	}

// Variables

	Vect _origin, _direction;
};