#pragma once

#include "SceneObject.h"

class Sphere : public SceneObject
{
public:

// Functions

	Sphere() : SceneObject(Material(Color(0.0, 0.0, 0.0), 0.0f, 0.0f, 0.0f, 0.0f)),
		_center(0.0, 0.0, 0.0), _radius(1.0f)
	{
	}

	Sphere(Vect center, double radius, Material material) : 
		SceneObject(material), _center(center), _radius(radius)
	{
		_radius = radius;
		_material = material;
	}

	Vect GetCenter()							{ return _center; }
	double GetRadius()							{ return _radius; }

	void SetCenter(Vect center)					{ _center = center; }
	void SetRadius(double radius)				{ _radius = radius; }

	virtual Vect GetNormalAt(Vect point)
	{
		// Normal always points away from the center.
		Vect normalVect = point.Add(_center.Negative()).Normalize();
		return normalVect;
	}

	virtual double FindIntersection(Ray ray)
	{
		Vect rayOrigin = ray._origin;
		double rayOriginX = rayOrigin._x;
		double rayOriginY = rayOrigin._y;
		double rayOriginZ = rayOrigin._z;

		Vect rayDirection = ray._direction;
		double rayDirectionX = rayDirection._x;
		double rayDirectionY = rayDirection._y;
		double rayDirectionZ = rayDirection._z;

		Vect sphereCenter = _center;
		double sphereCenterX = sphereCenter._x;
		double sphereCenterY = sphereCenter._y;
		double sphereCenterZ = sphereCenter._z;

		double a = 1; // Normalized.
		double b = (2 * (rayOriginX - sphereCenterX) * rayDirectionX) + (2 * (rayOriginY - sphereCenterY) * rayDirectionY) + (2 * (rayOriginZ - sphereCenterZ) * rayDirectionZ);
		double c = pow(rayOriginX - sphereCenterX, 2) + pow(rayOriginY - sphereCenterY, 2) + pow(rayOriginZ - sphereCenterZ, 2) - (_radius * _radius);

		double discriminant = (b * b) - (4 * a * c);

		if (discriminant > 0)
		{
			// The ray intersects the sphere.

			// The first root.
			double root1 = (((-1 * b) - sqrt(discriminant)) / (2 * a)) - 0.0000001;
			
			if (root1 > 0)
			{
				// The first root is the smallest positive root.
				return root1;
			}
			else
			{
				// The second root is the smallest positive root.
				double root2 = (((-1 * b) + sqrt(discriminant)) / (2 * a)) - 0.0000001;
				return root2;
			}
		}
		else
		{
			// The ray missed the sphere.
			return -1.0;
		}
	}

	virtual void Translate(Vect translation)
	{
		_center = _center.Add(translation);
	}

	~Sphere()
	{
	}

// Variables

	Vect		_center;
	double		_radius;
};