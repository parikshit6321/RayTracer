#pragma once

#include "Constants.h"
#include "Material.h"
#include "Logger.h"
#include "Ray.h"
#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"
#include "Settings.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <thread>

enum MoveDirection
{
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT
};

class Renderer
{
public:
	Renderer();
	void Initialize(void);
	void Render(int frameNo);
	double * GetDepthBuffer(void);
	void MoveCamera(MoveDirection direction);
	~Renderer();

private:

	Color GetColorAt(Vect intersectionPosition, Vect intersectionRayDirection, int indexOfWinningObject,
		double accuracy, double ambientLight, int bounce);
	Ray ComputeRefractionRay(int indexOfWinningObject, Vect intersectionPosition, Vect intersectionRayDirection);
	void SetPixels(int threadIndex);
	int CalculateOffset(double depth);
	void PostProcessPixels(int threadIndex);
	void PostProcess(void);
	void SaveBMP(const char* filename, int w, int h, int dpi, Color * data);
	int WinningObjectIndex(double intersections[]);

	int							_numOfPixels;
	Color *						_pixels;
	double *					_depthBuffer;

	Camera						_camera;
	std::vector<Light *>		_lights;
	std::vector<SceneObject *>	_sceneObjects;

	Vect						_X;
	Vect						_Y;
	Vect						_Z;
	Vect						_O;

	int							_dpi;
	double						_aspectRatio;

	std::vector<std::thread *>	_threads;
	int							_numOfThreads;
};