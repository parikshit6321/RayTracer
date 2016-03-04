#pragma once

#include "Color.h"

class Material
{
public:

	Material() : _diffuseColor(0, 0, 0), _specularIntensity(0.0f),
				 _reflectivity(0.0f), _refractivity(0.0f)
	{
	}

	Material(Color diffuseColor, float specularIntensity, float reflectivity, float refractivity)
		: _diffuseColor(diffuseColor), _specularIntensity(specularIntensity),
		  _reflectivity(reflectivity), _refractivity(refractivity)
	{
	}

	Color GetColor()									{ return _diffuseColor; }
	float GetSpecularIntensity()						{ return _specularIntensity; }
	float GetReflectivity()								{ return _reflectivity; }
	float GetRefractivity()								{ return _refractivity; }

	void SetColor(Color diffuseColor)					{ _diffuseColor = diffuseColor; }
	void SetSpecularIntensity(float specularIntensity)	{ _specularIntensity = specularIntensity; }
	void SetReflectivity(float reflectivity)			{ _reflectivity = reflectivity; }
	void SetRefractivity(float refractivity)			{ _refractivity = refractivity; }

	~Material()
	{
	}

private:

	Color		_diffuseColor;
	float		_specularIntensity;
	float		_reflectivity;
	float		_refractivity;
};