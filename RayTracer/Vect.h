#pragma once

#include <math.h>

class Vect
{
public:

// Functions

	Vect() : _x(0.0), _y(0.0), _z(0.0), _magnitude(0.0)
	{
	}
	Vect(double x, double y, double z) : _x(x), _y(y), _z(z)
	{
		SetMagnitude();
	}

	double GetX()			{ return _x; }
	double GetY()			{ return _y; }
	double GetZ()			{ return _z; }

	double GetMagnitude()	{ return _magnitude; }

	void SetX(double x)		{ _x = x; SetMagnitude(); }
	void SetY(double y)		{ _y = y; SetMagnitude(); }
	void SetZ(double z)		{ _z = z; SetMagnitude(); }

	void SetVector(double x, double y, double z)
	{
		_x = x;
		_y = y;
		_z = z;

		SetMagnitude();
	}

	Vect Normalize()
	{
		return Vect(_x / _magnitude, _y / _magnitude, _z / _magnitude);
	}

	Vect Negative()
	{
		return Vect(-1.0 * _x, -1.0 * _y, -1.0 * _z);
	}

	double Dot(Vect other)
	{
		return ((_x * other._x) + (_y * other._y) + (_z * other._z));
	}

	Vect Cross(Vect other)
	{
		return Vect(_y * other._z - _z * other._y, _z * other._x - _x * other._z, _x * other._y - _y * other._x);
	}

	Vect Add(Vect other)
	{
		return Vect(_x + other._x, _y + other._y, _z + other._z);
	}

	Vect ScalarMult(double scalar)
	{
		return Vect(_x * scalar, _y * scalar, _z * scalar);
	}

	~Vect()
	{
	}

// Variables
	
	void SetMagnitude()
	{
		_magnitude = sqrt((_x * _x) + (_y * _y) + (_z * _z));
	}

	double _x, _y, _z, _magnitude;
};