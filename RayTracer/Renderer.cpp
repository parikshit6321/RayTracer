#include "Renderer.h"
#include <string>

Renderer::Renderer() : _X(1.0, 0.0, 0.0), _Y(0.0, 1.0, 0.0), _Z(0.0, 0.0, 1.0), _O(0.0, 0.0, 0.0)
{
	_dpi = 72;
	_aspectRatio = (double)cWindowWidth / (double)cWindowHeight;

	_currentBMPName = new char[20];
	strcpy_s(_currentBMPName, 19, cBuffer1);

	_numOfPixels = cWindowHeight * cWindowWidth;
	_pixels = new Color[_numOfPixels];

	Vect campos(3, 1.5, 4);
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

	Logger::Log("Renderer initialized.");
}

void Renderer::Initialize(void)
{
	Material prettyGreenMaterial;
	prettyGreenMaterial.SetColor(Color(0.5, 1.0, 0.5));
	prettyGreenMaterial.SetSpecularIntensity(0.3f);

	Material maroonMaterial;
	maroonMaterial.SetColor(Color(0.5, 0.25, 0.25));

	Material reflectiveMaterial;
	reflectiveMaterial.SetColor(Color(0.8, 0.8, 0.8));
	reflectiveMaterial.SetReflectivity(1.0f);
	reflectiveMaterial.SetSpecularIntensity(0.1f);

	_lights.push_back(new Light(Vect(-7, 10, 10), Color(1.0, 1.0, 1.0)));
	_sceneObjects.push_back(new Sphere(_O.Add(Vect(0.0, 0.02, 0.0)), 1.0f, prettyGreenMaterial));
	_sceneObjects.push_back(new Sphere(_O.Add(Vect(2.0, 0.0, -3.0)), 1.0f, reflectiveMaterial));
	_sceneObjects.push_back(new Plane(_Y, -1.0, maroonMaterial));
}

void Renderer::SwapBuffers(void)
{
	if (strcmp(_currentBMPName, cBuffer1) == 0)
		strcpy_s(_currentBMPName, 19, cBuffer2);
	else
		strcpy_s(_currentBMPName, 19, cBuffer1);
}

// Applies Kernel blur effect to smooth out the frame.
void Renderer::PostProcess(void)
{
	int thisone = 0;

	const int offset = 1;

	int xOffsets[9] = { -offset, 0, offset, -offset, 0, offset, -offset, 0., offset };
	int yOffsets[9] = { offset, offset, offset, 0, 0, 0, -offset, -offset, -offset };

	float kernel[9] = { 
		(1.0f / 16.0f), (2.0f / 16.0f), (1.0f / 16.0f),
		(2.0f / 16.0f), (4.0f / 16.0f), (2.0f / 16.0f),
		(1.0f / 16.0f), (2.0f / 16.0f), (1.0f / 16.0f) 
	};

	for (int passes = 0; passes < cNumOfPostProcessPasses; ++passes)
	{
		for (int x = 0; x < cWindowWidth; ++x)
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

				_pixels[thisone].SetColor(finalColor.GetRed(), finalColor.GetGreen(), finalColor.GetBlue());
			}
		}
	}
}

void Renderer::SetPixelColor(int x, int y)
{
	
}

void Renderer::Render(int frameNo)
{
	Logger::Log("Rendering frame.");

	int thisone;

	_sceneObjects.at(0)->Translate(Vect(0.0, (0.02 * sin(frameNo / 50)), 0.0));

	for (int x = 0; x < cWindowWidth; ++x)
	{
		for (int y = 0; y < cWindowHeight; ++y)
		{
			double xamnt, yamnt;
			Vect camRayOrigin = _camera.GetCamPos();

			std::vector<double> intersections;

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

			Vect camRayDirection = _camera.GetCamDir().Add(_camera.GetCamRight().ScalarMult(xamnt - 0.5).Add(_camera.GetCamDown().ScalarMult(yamnt - 0.5))).Normalize();

			Ray camRay(camRayOrigin, camRayDirection);

			for (int objectIndex = 0; objectIndex < _sceneObjects.size(); ++objectIndex)
			{
				intersections.push_back(_sceneObjects.at(objectIndex)->FindIntersection(camRay));
			}

			int indexOfWinningObject = WinningObjectIndex(intersections);

			if (indexOfWinningObject >= 0)
			{
				if (intersections.at(indexOfWinningObject) > cAccuracy)
				{
					// Determine the position and direction vectors at the point of intersection.

					Vect intersectionPosition = camRayOrigin.Add(camRayDirection.ScalarMult(intersections.at(indexOfWinningObject)));
					Vect intersectionRayDirection = camRayDirection;

					Color intersectionColor = GetColorAt(intersectionPosition, intersectionRayDirection, indexOfWinningObject, cAccuracy, cAmbientLight);

					_pixels[thisone].SetColor(intersectionColor.GetRed(), intersectionColor.GetGreen(), intersectionColor.GetBlue());
				}
			}
			else
			{
				_pixels[thisone].SetColor(0.0, 0.0, 0.0);
			}
		}
	}

	PostProcess();

	SaveBMP(_currentBMPName, cWindowWidth, cWindowHeight, _dpi, _pixels);
}

Color Renderer::GetColorAt(Vect intersectionPosition, Vect intersectionRayDirection, int indexOfWinningObject,
	double accuracy, double ambientLight)
{
	Color winningObjectColor = _sceneObjects.at(indexOfWinningObject)->GetMaterial().GetColor();
	Vect winningObjectNormal = _sceneObjects.at(indexOfWinningObject)->GetNormalAt(intersectionPosition);

	Color finalColor = winningObjectColor.MultiplyScalar(cAmbientLight);

	float reflectivity = _sceneObjects.at(indexOfWinningObject)->GetMaterial().GetReflectivity();
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
		std::vector<double> reflectionIntersections;

		for (int reflectionIndex = 0; reflectionIndex < _sceneObjects.size(); ++reflectionIndex)
		{
			reflectionIntersections.push_back(_sceneObjects.at(reflectionIndex)->FindIntersection(reflectionRay));
		}

		int indexOfWinningObjectWithReflection = WinningObjectIndex(reflectionIntersections);

		if (indexOfWinningObjectWithReflection >= 0)
		{
			// Ray missed everything.
			if (reflectionIntersections.at(indexOfWinningObjectWithReflection) > cAccuracy)
			{
				// Determine the position and direction of the point of intersection with the reflection ray.
				// The ray only affects the color if it reflected off something.

				Vect reflectionIntersectionPosition = intersectionPosition.Add(reflectionDirection.ScalarMult(reflectionIntersections.at(indexOfWinningObjectWithReflection)));
				Vect reflectionIntersectionDirection = reflectionDirection;

				Color reflectionIntersectionColor = GetColorAt(reflectionIntersectionPosition, reflectionIntersectionDirection, indexOfWinningObjectWithReflection, cAccuracy, cAmbientLight);
			
				finalColor = finalColor.Add(reflectionIntersectionColor.MultiplyScalar(reflectivity));
			}
		}
	}

	for (int lightIndex = 0; lightIndex < _lights.size(); ++lightIndex)
	{
		Vect lightDirection = _lights.at(lightIndex)->GetPosition().Add(intersectionPosition.Negative()).Normalize();
		float cosineAngle = winningObjectNormal.Dot(lightDirection);
		
		if (cosineAngle > 0)
		{
			// Test for shadow.
			bool bShadowed = false;

			Vect distanceToLight = _lights.at(lightIndex)->GetPosition().Add(intersectionPosition.Negative());
			double distanceToLightMagnitude = distanceToLight.GetMagnitude();

			Ray shadowRay(intersectionPosition, lightDirection);

			std::vector<double> secondaryIntersections;

			for (int objectIndex = 0; objectIndex < _sceneObjects.size() && !bShadowed; ++objectIndex)
			{
				secondaryIntersections.push_back(_sceneObjects.at(objectIndex)->FindIntersection(shadowRay));
			}

			for (int c = 0; c < secondaryIntersections.size(); ++c)
			{
				if (secondaryIntersections.at(c) > cAccuracy)
				{
					if (secondaryIntersections.at(c) <= distanceToLightMagnitude)
					{
						bShadowed = true;
					}
				}
			}

			if (!bShadowed)
			{
				finalColor = finalColor.Add(winningObjectColor.MultiplyColor(_lights.at(lightIndex)->GetColor()).MultiplyScalar(cosineAngle));

				// Add shininess
				float specularIntensity = _sceneObjects.at(indexOfWinningObject)->GetMaterial().GetSpecularIntensity();
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
						finalColor = finalColor.Add(_lights.at(lightIndex)->GetColor().MultiplyScalar(specular * specularIntensity));
					}
				}
			}
		}
	}

	return finalColor.Clip();
}

int Renderer::WinningObjectIndex(std::vector<double> intersections)
{
	// Return the index of the winning intersection.
	int indexOfMinimumValue;

	// Prevent unnecessary calculations.
	if (intersections.size() == 0)
	{
		// If there are no intersections.
		return -1;
	}
	else if (intersections.size() == 1)
	{
		if (intersections.at(0) > 0)
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
		for (int i = 0; i < intersections.size(); ++i)
		{
			if (intersections.at(i) > max)
			{
				max = intersections.at(i);
			}
		}

		if (max > 0)
		{
			double min = max;
			for (int i = 0; i < intersections.size(); ++i)
			{
				if (intersections.at(i) > 0 && intersections.at(i) <= min)
				{
					min = intersections.at(i);
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

const char* Renderer::GetRenderedFrame(void)
{
	if (strcmp(_currentBMPName, cBuffer1) == 0)
		return cBuffer2;
	else
		return cBuffer1;
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
		double red = (data[i].GetRed()) * 255;
		double green = (data[i].GetGreen()) * 255;
		double blue = (data[i].GetBlue()) * 255;

		unsigned char color[3] = { (int)floor(blue), (int)floor(green), (int)floor(red) };

		fwrite(color, 1, 3, f);
	}

	fclose(f);
}

Renderer::~Renderer()
{
}
