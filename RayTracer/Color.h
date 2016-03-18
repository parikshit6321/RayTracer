#pragma once

class Color
{
public:

// Functions

	Color() { _r = 0.0; _g = 0.0; _b = 0.0; }
	Color(double r, double g, double b) : _r(r), _g(g), _b(b) { }
	
	double brightness()
	{
		double intensity = (_r + _g + _b) / 3;
		return intensity;
	}

	Color MultiplyScalar(double scalar)
	{
		return Color(_r * scalar, _g * scalar, _b * scalar);
	}

	Color Add(Color color)
	{
		return Color(_r + color._r, _g + color._g, _b + color._b);
	}

	Color MultiplyColor(Color color)
	{
		return Color(_r * color._r, _g * color._g, _b * color._b);
	}

	Color Average(Color color)
	{
		return Color((_r + color._r) / 2, (_g + color._g) / 2, (_b + color._b) / 2);
	}

	Color Clip()
	{
		double allLight = _r + _g + _b;
		double excessLight = allLight - 3;

		if (excessLight > 0)
		{
			_r = _r + excessLight * (_r / allLight);
			_g = _g + excessLight * (_g / allLight);
			_b = _b + excessLight * (_b / allLight);
		}
		if (_r > 1)
			_r = 1;
		if (_g > 1)
			_g = 1;
		if (_b > 1)
			_b = 1;
		
		if (_r < 0)
			_r = 0;
		if (_g < 0)
			_g = 0;
		if (_b < 0)
			_b = 0;

		return Color(_r, _g, _b);
	}

	~Color()
	{}

// Variables

	double _r, _g, _b;
};