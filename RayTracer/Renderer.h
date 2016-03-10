#pragma once

#include "Constants.h"
#include "Material.h"
#include "Logger.h"
#include "Ray.h"
#include "Camera.h"
#include "Light.h"
#include "Sphere.h"
#include "Plane.h"

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

class Renderer
{
public:
	Renderer();
	void Initialize(void);
	void Render(int frameNo);
	const char* GetRenderedFrame(void);
	~Renderer();

private:

	Color GetColorAt(Vect intersectionPosition, Vect intersectionRayDirection, int indexOfWinningObject,
		double accuracy, double ambientLight);
	void SetPixelColor(int x, int y);
	void PostProcess(void);
	void SwapBuffers(void);
	void SaveBMP(const char* filename, int w, int h, int dpi, Color * data);
	int WinningObjectIndex(std::vector<double> intersections);

	int							_numOfPixels;
	Color *						_pixels;

	Camera						_camera;
	std::vector<Light *>		_lights;
	std::vector<SceneObject *>	_sceneObjects;

	char *						_currentBMPName;

	Vect						_X;
	Vect						_Y;
	Vect						_Z;
	Vect						_O;

	int							_dpi;
	double						_aspectRatio;
};