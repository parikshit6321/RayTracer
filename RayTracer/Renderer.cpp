#include "Renderer.h"
#include <string>

Renderer::Renderer() : _X(1.0, 0.0, 0.0), _Y(0.0, 1.0, 0.0), _Z(0.0, 0.0, 1.0), _O(0.0, 0.0, 0.0)
{
	_dpi = 72;
	_aspectRatio = (double)cWindowWidth / (double)cWindowHeight;

	_numOfPixels = cWindowHeight * cWindowWidth;
	_pixels = new Color[_numOfPixels];

	Vect campos(0, 0, -5);
	Vect look_at(0, 0, 0);

	Vect diff_btw(look_at.GetX() - campos.GetX(), look_at.GetY() - campos.GetY(),
				  look_at.GetZ() - campos.GetZ());

	Vect camdir = diff_btw.Normalize();
	Vect camright = _Y.Cross(camdir).Normalize();
	Vect camdown = camright.Cross(camdir);

	_camera.SetCamPos(campos);
	_camera.SetCamDir(camdir);
	_camera.SetCamRight(camright);
	_camera.SetCamDown(camdown);

	_numOfThreads = std::thread::hardware_concurrency();
}

void Renderer::Initialize(void)
{
	Material prettyGreenMaterial;
	prettyGreenMaterial._diffuseColor = Color(0.5, 1.0, 0.5);
	prettyGreenMaterial._specularIntensity = 0.3f;

	Material maroonMaterial;
	maroonMaterial._diffuseColor = Color(0.5, 0.25, 0.25);

	Material reflectiveMaterial;
	reflectiveMaterial._diffuseColor = Color(0.8, 0.8, 0.8);
	reflectiveMaterial._reflectivity = 1.0f;
	reflectiveMaterial._specularIntensity = 0.1f;

	Material refractiveMaterial;
	refractiveMaterial._diffuseColor = Color(0.0, 0.0, 0.0);
	refractiveMaterial._refractivity = 1.0f;
	refractiveMaterial._refractiveIndex = 1.5f;
	refractiveMaterial._criticalAngle = 40.8f;

	_lights.push_back(new Light(Vect(-7, 10, 10), Color(1.0, 1.0, 1.0)));
	_sceneObjects.push_back(new Sphere(_O.Add(Vect(0.0, 0.02, 0.0)), 1.0f, prettyGreenMaterial));
	_sceneObjects.push_back(new Sphere(_O.Add(Vect(-2.0, 0.0, 2.0)), 1.0f, reflectiveMaterial));
	_sceneObjects.push_back(new Sphere(_O.Add(Vect(0.0, 0.0, -2.0)), 1.0f, refractiveMaterial));
	_sceneObjects.push_back(new Plane(_Y, -1.0, maroonMaterial));

	Logger::Log("Renderer initialized.");
}

void Renderer::MoveCamera(MoveDirection direction)
{
	switch (direction)
	{
	case MoveDirection::MOVE_FORWARD:
		_camera._camPos = _camera._camPos.Add(Vect(0.0, 0.0, 0.1));
		break;
	
	case MoveDirection::MOVE_BACKWARD:
		_camera._camPos = _camera._camPos.Add(Vect(0.0, 0.0, -0.1));
		break;

	case MoveDirection::MOVE_LEFT:
		_camera._camPos = _camera._camPos.Add(Vect(-0.1, 0.0, 0.0));
		break;
		
	case MoveDirection::MOVE_RIGHT:
		_camera._camPos = _camera._camPos.Add(Vect(0.1, 0.0, 0.0));
		break;
	}
}

void Renderer::PostProcessPixels(int threadIndex)
{
	int thisone = 0;

	const int offset = 1;

	int xOffsets[9] = { -offset, 0, offset, -offset, 0, offset, -offset, 0., offset };
	int yOffsets[9] = { offset, offset, offset, 0, 0, 0, -offset, -offset, -offset };

	double kernel[9] = {
		(1.0f / 16.0f), (2.0f / 16.0f), (1.0f / 16.0f),
		(2.0f / 16.0f), (4.0f / 16.0f), (2.0f / 16.0f),
		(1.0f / 16.0f), (2.0f / 16.0f), (1.0f / 16.0f)
	};

	for (int x = (int)(0 + (((double)threadIndex / (double)_numOfThreads) * cWindowWidth)); x < (int)(cWindowWidth * ((double)(threadIndex + 1) / (double)_numOfThreads)); ++x)
	{
		for (int y = 0; y < cWindowHeight; ++y)
		{
			Color finalColor;

			thisone = y * cWindowWidth + x;

			for (int i = 0; i < 9; ++i)
			{
				int xNew = x + xOffsets[i];
				int yNew = y + yOffsets[i];

				if (xNew >= 0 && xNew < cWindowWidth && yNew >= 0 && yNew < cWindowHeight)
				{
					finalColor = finalColor.Add(_pixels[yNew * cWindowWidth + xNew].MultiplyScalar(kernel[i]));
				}
			}

			_pixels[thisone].SetColor(finalColor._r, finalColor._g, finalColor._b);
		}
	}
}

// Applies Kernel blur effect to smooth out the frame.
void Renderer::PostProcess(void)
{
	for (int passes = 0; passes < cNumOfPostProcessPasses; ++passes)
	{
		_threads.clear();

		for (int threadIndex = 0; threadIndex < _numOfThreads; ++threadIndex)
		{
			_threads.push_back(new std::thread(&Renderer::PostProcessPixels, this, threadIndex));
		}

		for (int threadIndex = 0; threadIndex < _numOfThreads; ++threadIndex)
		{
			if (_threads.at(threadIndex)->joinable())
				_threads.at(threadIndex)->join();
		}
	}
}

void Renderer::SetPixels(int threadIndex)
{
	for (int x = (int)(0 + (((double)threadIndex / (double)_numOfThreads) * cWindowWidth)); x < (int)(cWindowWidth * ((double)(threadIndex + 1) / (double)_numOfThreads)); ++x)
	{
		for (int y = 0; y < cWindowHeight; ++y)
		{
			double xamnt, yamnt;
			Vect camRayOrigin = _camera._camPos;

			double intersections[MAX_OBJECTS];

			int thisone = y * cWindowWidth + x;

			// Start with no anti-aliasing.
			if (cWindowWidth > cWindowHeight)
			{
				// The image is wider than it is tall.
				xamnt = ((x + 0.5) / cWindowWidth) * _aspectRatio - (((cWindowWidth - cWindowHeight) / (double)cWindowHeight) / 2);
				yamnt = ((cWindowHeight - y) + 0.5) / cWindowHeight;
			}
			else if (cWindowHeight > cWindowWidth)
			{
				// The image is taller than it is wide.
				xamnt = (x + 0.5) / cWindowWidth;
				yamnt = (((cWindowHeight - y) + 0.5) / cWindowHeight) / _aspectRatio - (((cWindowHeight - cWindowWidth) / (double)cWindowWidth) / 2);
			}
			else
			{
				// The image is as tall as it is wide.
				xamnt = (x + 0.5) / cWindowWidth;
				yamnt = ((cWindowHeight - y) + 0.5) / cWindowHeight;
			}

			Vect camRayDirection = _camera._camDir.Add(_camera._camRight.ScalarMult(xamnt - 0.5).Add(_camera._camDown.ScalarMult(yamnt - 0.5))).Normalize();

			Ray camRay(camRayOrigin, camRayDirection);

			for (int objectIndex = 0; objectIndex < _sceneObjects.size(); ++objectIndex)
			{
				intersections[objectIndex] = _sceneObjects.at(objectIndex)->FindIntersection(camRay);
			}

			int indexOfWinningObject = WinningObjectIndex(intersections);

			if (indexOfWinningObject >= 0)
			{
				if (intersections[indexOfWinningObject] > cAccuracy)
				{
					// Determine the position and direction vectors at the point of intersection.

					Vect intersectionPosition = camRayOrigin.Add(camRayDirection.ScalarMult(intersections[indexOfWinningObject]));
					Vect intersectionRayDirection = camRayDirection;

					Color intersectionColor = GetColorAt(intersectionPosition, intersectionRayDirection, 
						indexOfWinningObject, cAccuracy, cAmbientLight, 1);

					_pixels[thisone].SetColor(intersectionColor._r, intersectionColor._g, intersectionColor._b);
				}
			}
			else
			{
				_pixels[thisone].SetColor(0.0, 0.0, 0.0);
			}
		}
	}
}

void Renderer::Render(int frameNo)
{
	int thisone;

	_sceneObjects.at(0)->Translate(Vect(0.0, (0.02 * sin(frameNo / 50)), 0.0));

	_threads.clear();

	for (int threadIndex = 0; threadIndex < _numOfThreads; ++threadIndex)
	{
		_threads.push_back(new std::thread(&Renderer::SetPixels, this, threadIndex));
	}
	for (int threadIndex = 0; threadIndex < _numOfThreads; ++threadIndex)
	{
		if (_threads.at(threadIndex)->joinable())
			_threads.at(threadIndex)->join();
	}

	SaveBMP(cBuffer, cWindowWidth, cWindowHeight, _dpi, _pixels);
}

Color Renderer::GetColorAt(Vect intersectionPosition, Vect intersectionRayDirection, int indexOfWinningObject,
	double accuracy, double ambientLight, int bounce)
{
	if (bounce > cTotalBounces)
		return Color(0.0, 0.0, 0.0);

	Color winningObjectColor = _sceneObjects.at(indexOfWinningObject)->_material._diffuseColor;
	Vect winningObjectNormal = _sceneObjects.at(indexOfWinningObject)->GetNormalAt(intersectionPosition);

	Color finalColor = winningObjectColor.MultiplyScalar(cAmbientLight);

	// Handle refractions
	float refractivity = _sceneObjects.at(indexOfWinningObject)->_material._refractivity;
	if (refractivity > 0.0f && refractivity <= 1.0f)
	{
		Ray refractionRay = ComputeRefractionRay(indexOfWinningObject, intersectionPosition,
			intersectionRayDirection);

		// Determine what the ray intersects with first.
		double refractionIntersections[MAX_OBJECTS];

		for (int refractionIndex = 0; refractionIndex < _sceneObjects.size(); ++refractionIndex)
		{
			refractionIntersections[refractionIndex] = _sceneObjects.at(refractionIndex)->FindIntersection(refractionRay);
		}

		int indexOfWinningObjectWithRefraction = WinningObjectIndex(refractionIntersections);

		if (indexOfWinningObjectWithRefraction >= 0)
		{
			// Ray missed everything.
			if (refractionIntersections[indexOfWinningObjectWithRefraction] > cAccuracy)
			{
				// Determine the position and direction of the point of intersection with the refraction ray.
				
				Vect refractionIntersectionPosition = intersectionPosition.Add(refractionRay._direction.ScalarMult(refractionIntersections[indexOfWinningObjectWithRefraction]));
				Vect refractionIntersectionDirection = refractionRay._direction;

				Color refractionIntersectionColor = GetColorAt(refractionIntersectionPosition, refractionIntersectionDirection, indexOfWinningObjectWithRefraction, cAccuracy, cAmbientLight, (bounce + 1));

				finalColor = finalColor.Add(refractionIntersectionColor.MultiplyScalar(refractivity));
			}
		}
	}

	double reflectivity = _sceneObjects.at(indexOfWinningObject)->_material._reflectivity;
	if (reflectivity > 0.0f && reflectivity <= 1.0f)
	{
		double dot1 = winningObjectNormal.Dot(intersectionRayDirection.Negative());
		Vect scalar1 = winningObjectNormal.ScalarMult(dot1);
		Vect add1 = scalar1.Add(intersectionRayDirection);

		Vect scalar2 = add1.ScalarMult(2);
		Vect add2 = intersectionRayDirection.Negative().Add(scalar2);
		Vect reflectionDirection = add2.Normalize();

		Ray reflectionRay(intersectionPosition, reflectionDirection);

		// Determine what the ray intersects with first.
		double reflectionIntersections[MAX_OBJECTS];

		for (int reflectionIndex = 0; reflectionIndex < _sceneObjects.size(); ++reflectionIndex)
		{
			reflectionIntersections[reflectionIndex] = _sceneObjects.at(reflectionIndex)->FindIntersection(reflectionRay);
		}

		int indexOfWinningObjectWithReflection = WinningObjectIndex(reflectionIntersections);

		if (indexOfWinningObjectWithReflection >= 0)
		{
			// Ray missed everything.
			if (reflectionIntersections[indexOfWinningObjectWithReflection] > cAccuracy)
			{
				// Determine the position and direction of the point of intersection with the reflection ray.
				// The ray only affects the color if it reflected off something.

				Vect reflectionIntersectionPosition = intersectionPosition.Add(reflectionDirection.ScalarMult(reflectionIntersections[indexOfWinningObjectWithReflection]));
				Vect reflectionIntersectionDirection = reflectionDirection;

				Color reflectionIntersectionColor = GetColorAt(reflectionIntersectionPosition, reflectionIntersectionDirection, indexOfWinningObjectWithReflection, cAccuracy, cAmbientLight, (bounce + 1));
			
				finalColor = finalColor.Add(reflectionIntersectionColor.MultiplyScalar(reflectivity));
			}
		}
	}

	for (int lightIndex = 0; lightIndex < _lights.size(); ++lightIndex)
	{
		Vect lightDirection = _lights.at(lightIndex)->_position.Add(intersectionPosition.Negative()).Normalize();
		double cosineAngle = winningObjectNormal.Dot(lightDirection);
		
		if (cosineAngle > 0)
		{
			// Test for shadow.
			bool bShadowed = false;

			Vect distanceToLight = _lights.at(lightIndex)->_position.Add(intersectionPosition.Negative());
			double distanceToLightMagnitude = distanceToLight._magnitude;

			Ray shadowRay(intersectionPosition, lightDirection);

			double secondaryIntersections[MAX_OBJECTS];

			for (int objectIndex = 0; objectIndex < _sceneObjects.size() && !bShadowed; ++objectIndex)
			{
				secondaryIntersections[objectIndex] = _sceneObjects.at(objectIndex)->FindIntersection(shadowRay);
			}

			for (int c = 0; c < _sceneObjects.size(); ++c)
			{
				if (secondaryIntersections[c] > cAccuracy)
				{
					if (secondaryIntersections[c] <= distanceToLightMagnitude && _sceneObjects.at(c)->_material._refractivity <= 0.0f)
					{
						bShadowed = true;
					}
				}
			}

			if (!bShadowed)
			{
				finalColor = finalColor.Add(winningObjectColor.MultiplyColor(_lights.at(lightIndex)->_color).MultiplyScalar(cosineAngle));

				// Add shininess
				double specularIntensity = _sceneObjects.at(indexOfWinningObject)->_material._specularIntensity;
				if (specularIntensity > 0 && specularIntensity <= 1)
				{
					double dot1 = winningObjectNormal.Dot(intersectionRayDirection.Negative());
					Vect scalar1 = winningObjectNormal.ScalarMult(dot1);
					Vect add1 = scalar1.Add(intersectionRayDirection);
					
					Vect scalar2 = add1.ScalarMult(2);
					Vect add2 = intersectionRayDirection.Negative().Add(scalar2);
					Vect reflectionDirection = add2.Normalize();

					double specular = reflectionDirection.Dot(lightDirection);
					if (specular > 0)
					{
						specular = pow(specular, 10);
						finalColor = finalColor.Add(_lights.at(lightIndex)->_color.MultiplyScalar(specular * specularIntensity));
					}
				}
			}
		}
	}

	return finalColor.Clip();
}

Ray Renderer::ComputeRefractionRay(int indexOfWinningObject, Vect intersectionPosition, 
	Vect intersectionRayDirection)
{
	Vect normal = _sceneObjects.at(indexOfWinningObject)->GetNormalAt(intersectionPosition);
	normal = normal.Normalize();
	intersectionRayDirection = intersectionRayDirection.Normalize();

	Vect refractedRayOrigin;
	Vect refractedRayDirection;

	float ratio = 1.0f;
	float refractiveIndex = _sceneObjects.at(indexOfWinningObject)->_material._refractiveIndex;
	float criticalAngle = _sceneObjects.at(indexOfWinningObject)->_material._criticalAngle;

	// Ray is inside the sphere.
	if (intersectionRayDirection.Dot(normal) >= 0.0f)
	{
		normal = normal.Negative();
		ratio = refractiveIndex;
	}
	// Ray is outside the sphere.
	else
	{
		ratio = 1.0f / refractiveIndex;
	}

	// Calculate cos(theta 1).
	double costheta1 = normal.Negative().Dot(intersectionRayDirection);

	double angleInDegrees = acos(costheta1) * (180.0f / cPI);

	if ((angleInDegrees > criticalAngle) && (ratio > 1.0f))
	{
		// Total internal reflection takes place.
		refractedRayDirection = intersectionRayDirection;
		Vect temp = normal.ScalarMult(2.0 * costheta1);
		refractedRayDirection.Add(temp);
	}
	else
	{
		// Normal refraction takes place.
		refractedRayDirection = (intersectionRayDirection.ScalarMult(ratio)).Add(normal.ScalarMult((ratio * costheta1) - sqrt(1.0f - (ratio * ratio * (1.0f - (costheta1 * costheta1))))));
	}

	refractedRayDirection = refractedRayDirection.Normalize();
	refractedRayOrigin = intersectionPosition.Add(normal.Negative().ScalarMult(cAccuracy));;

	return Ray(refractedRayOrigin, refractedRayDirection);
}

int Renderer::WinningObjectIndex(double intersections[])
{
	// Return the index of the winning intersection.
	int indexOfMinimumValue;

	// Prevent unnecessary calculations.
	if (_sceneObjects.size() == 0)
	{
		// If there are no intersections.
		return -1;
	}
	else if (_sceneObjects.size() == 1)
	{
		if (intersections[0] > 0)
		{
			// If that intersection is greater than zero then its our index of minimum value.
			return 0;
		}
		else
		{
			// Otherwise the only intersection value is negative.
			return -1;
		}
	}
	else
	{
		// Otherwise there is more than one intersection.
		// First find the maximum value;

		double max = 0;
		for (int i = 0; i < _sceneObjects.size(); ++i)
		{
			if (intersections[i] > max)
			{
				max = intersections[i];
			}
		}

		if (max > 0)
		{
			double min = max;
			for (int i = 0; i < _sceneObjects.size(); ++i)
			{
				if (intersections[i] > 0 && intersections[i] <= min)
				{
					min = intersections[i];
					indexOfMinimumValue = i;
				}
			}

			return indexOfMinimumValue;
		}
		else
		{
			// All the intersections were negative.
			return -1;
		}
	}
}

void Renderer::SaveBMP(const char* filename, int w, int h, int dpi, Color * data)
{
	FILE * f;
	int k = w * h;
	int s = 4 * k;
	int filesize = 54 + s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi * m;
	
	unsigned char bmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unsigned char bmpinfoheader[40] = { 40, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 0, 24, 0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize>>8);
	bmpfileheader[4] = (unsigned char)(filesize>>16);
	bmpfileheader[5] = (unsigned char)(filesize>>24);

	bmpinfoheader[4] = (unsigned char)(w);
	bmpinfoheader[5] = (unsigned char)(w>>8);
	bmpinfoheader[6] = (unsigned char)(w>>16);
	bmpinfoheader[7] = (unsigned char)(w>>24);

	bmpinfoheader[8] = (unsigned char)(h);
	bmpinfoheader[9] = (unsigned char)(h >> 8);
	bmpinfoheader[10] = (unsigned char)(h >> 16);
	bmpinfoheader[11] = (unsigned char)(h >> 24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s >> 8);
	bmpinfoheader[23] = (unsigned char)(s >> 16);
	bmpinfoheader[24] = (unsigned char)(s >> 24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm >> 8);
	bmpinfoheader[27] = (unsigned char)(ppm >> 16);
	bmpinfoheader[28] = (unsigned char)(ppm >> 24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm >> 8);
	bmpinfoheader[31] = (unsigned char)(ppm >> 16);
	bmpinfoheader[32] = (unsigned char)(ppm >> 24);

	fopen_s(&f, filename, "wb");

	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);

	for (int i = 0; i < k; ++i)
	{
		double red = (data[i]._r) * 255;
		double green = (data[i]._g) * 255;
		double blue = (data[i]._b) * 255;

		unsigned char color[3] = { (int)floor(blue), (int)floor(green), (int)floor(red) };

		fwrite(color, 1, 3, f);
	}

	fclose(f);
}

Renderer::~Renderer()
{
}
