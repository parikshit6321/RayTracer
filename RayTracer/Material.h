#pragma once

#include "Color.h"

class Material
{
public:

// Functions

	Material() : _diffuseColor(0, 0, 0), _specularIntensity(0.0f),
		_reflectivity(0.0f), _refractivity(0.0f), _refractiveIndex(0.0f)
	{
	}

	Material(Color diffuseColor, float specularIntensity, float reflectivity, float refractivity, float refractiveIndex)
		: _diffuseColor(diffuseColor), _specularIntensity(specularIntensity),
		_reflectivity(reflectivity), _refractivity(refractivity), _refractiveIndex(refractiveIndex)
	{
	}

	~Material()
	{
	}

// Variables

	Color		_diffuseColor;
	float		_specularIntensity;
	float		_reflectivity;
	float		_refractivity;
	float		_refractiveIndex;
	float		_criticalAngle;
};