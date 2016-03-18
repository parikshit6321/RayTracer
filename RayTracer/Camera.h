#pragma once

#include "Ray.h"

class Camera
{
public:

// Functions

	Camera() : _camPos(0, 0, 0), _camDir(0, 0, 1), _camRight(1, 0, 0), _camDown(0, -1, 0)
	{
	}

	Camera(const Vect & camPos, const Vect & camDir, const Vect & camRight, const Vect & camDown)
		: _camPos(camPos), _camDir(camDir), _camRight(camRight), _camDown(camDown)
	{
	}

	Vect GetCamPos()						{ return _camPos; }
	Vect GetCamDir()						{ return _camDir; }
	Vect GetCamRight()						{ return _camRight; }
	Vect GetCamDown()						{ return _camDown; }

	void SetCamPos(const Vect & camPos)		{ _camPos = camPos; }
	void SetCamDir(const Vect & camDir)		{ _camDir = camDir; }
	void SetCamRight(const Vect & camRight)	{ _camRight = camRight; }
	void SetCamDown(const Vect & camDown)	{ _camDown = camDown; }

	~Camera()
	{
	}

// Variables

	Vect _camPos, _camDir, _camRight, _camDown;
};