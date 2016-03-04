#pragma once

#include "SceneObject.h"

class Sphere : public SceneObject
{
public:

	Sphere() : SceneObject(Material(Color(0.0, 0.0, 0.0), 0.0f, 0.0f, 0.0f)),
		_center(0.0, 0.0, 0.0), _radius(1.0f)
	{
	}

	Sphere(Vect center, float radius, Material material) : 
		SceneObject(material), _center(center), _radius(radius)
	{
		_radius = radius;
		_material = material;
	}

	Vect GetCenter()							{ return _center; }
	float GetRadius()							{ return _radius; }

	void SetCenter(Vect center)					{ _center = center; }
	void SetRadius(float radius)				{ _radius = radius; }

	virtual Vect GetNormalAt(Vect point)
	{
		// Normal always points away from the center.
		Vect normalVect = point.Add(_center.Negative()).Normalize();
		return normalVect;
	}

	virtual double FindIntersection(Ray ray)
	{
		Vect rayOrigin = ray.GetOrigin();
		double rayOriginX = rayOrigin.GetX();
		double rayOriginY = rayOrigin.GetY();
		double rayOriginZ = rayOrigin.GetZ();

		Vect rayDirection = ray.GetDirection();
		double rayDirectionX = rayDirection.GetX();
		double rayDirectionY = rayDirection.GetY();
		double rayDirectionZ = rayDirection.GetZ();

		Vect sphereCenter = _center;
		double sphereCenterX = sphereCenter.GetX();
		double sphereCenterY = sphereCenter.GetY();
		double sphereCenterZ = sphereCenter.GetZ();

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

private:

	Vect		_center;
	float		_radius;
};