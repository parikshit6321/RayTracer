#pragma once

#include "SceneObject.h"

class Plane : public SceneObject
{
public:

	Plane() : SceneObject(Material(Color(0.0, 0.0, 0.0), 0.0f, 0.0f, 0.0f)), _normal(Vect(0.0, 1.0, 0.0)),
		_distance(0.0)
	{
	}

	Plane(Vect normal, double distance, Material material) : SceneObject(material), 
		_normal(normal), _distance(distance)
	{
	}

	Vect GetNormal()							{ return _normal; }
	double GetDistance()						{ return _distance; }

	void SetNormal(Vect normal)					{ _normal = normal; }
	void SetDistance(double distance)			{ _distance = distance; }

	virtual Vect GetNormalAt(Vect point)		{ return _normal; }
	virtual double FindIntersection(Ray ray)
	{
		Vect rayDirection = ray.GetDirection();
		double a = rayDirection.Dot(_normal);

		if (a == 0)
		{
			// Ray is parallel to the plane.
			return -1.0;
		}
		else
		{
			double b = _normal.Dot(ray.GetOrigin().Add(_normal.ScalarMult(_distance).Negative()));
			return -1 * (b / a);
		}
	}
	virtual void Translate(Vect translation)
	{
	}

	~Plane()
	{
	}

private:

	Vect		_normal;
	double		_distance;
};