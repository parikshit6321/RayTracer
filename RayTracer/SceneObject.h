#pragma once

#include "Ray.h"
#include "Vect.h"
#include "Material.h"

class SceneObject
{
public:

// Functions

	SceneObject() : _material(Color(0.0, 0.0, 0.0), 0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	SceneObject(Material material) : _material(material)
	{
	}

	Material GetMaterial(void)			{ return _material; }
	void SetMaterial(Material material)	{ _material = material; }

	virtual Vect GetNormalAt(Vect point) = 0;
	virtual double FindIntersection(Ray ray) = 0;
	virtual void Translate(Vect translation) = 0;

	~SceneObject()
	{
	}

// Variables

	Material	_material;
};