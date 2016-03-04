#pragma once

#include "Color.h"
#include "Vect.h"

class Light
{
public:

	Light() : _position(0, 10, 0), _color(1, 1, 1)
	{
	}

	Light(Vect position, Color color) : _position(position), _color(color)
	{
	}

	Vect GetPosition()					{ return _position; }
	Color GetColor()					{ return _color; }

	void SetPosition(Vect position)		{ _position = position; }
	void SetColor(Color color)			{ _color = color; }

	~Light()
	{
	}

private:

	Vect	_position;
	Color	_color;
};