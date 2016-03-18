#include "Settings.h"
#include "Logger.h"
#include <fstream>
#include <string>

int Settings::cWindowWidth = 352;
int Settings::cWindowHeight = 240;
bool Settings::cPostProcessingEnabled = false;
int Settings::cNumOfPostProcessPasses = 0;
int Settings::cNumOfBounces = 4;

Settings::Settings()
{
}

void Settings::Initialize(void)
{
	std::ifstream inputFile;
	inputFile.open("Config.cfg", std::ios::in);

	if (inputFile.is_open())
	{
		int numbers[4];
		std::string line;

		for (int i = 0; i < 2; ++i)
		{
			std::getline(inputFile, line);
			Logger::Log(line.c_str());
		}

		int count = 0;

		while (std::getline(inputFile, line))
		{
			Logger::Log(line.c_str());

			int index;
			for (index = 0; index < line.length(); ++index)
			{
				if (line[index] == '=')
				{
					index += 2;
					break;
				}
			}

			std::string temp = "";
			
			while (index < line.length())
			{
				temp += line[index];
				++index;
			}

			numbers[count] = std::stoi(temp);
			++count;
		}

		cWindowWidth = numbers[0];
		cWindowHeight = numbers[1];
		cNumOfPostProcessPasses = numbers[2];
		cNumOfBounces = numbers[3];

		if (cNumOfPostProcessPasses > 0)
			cPostProcessingEnabled = true;

		inputFile.close();
	}
	else
	{
		Logger::Log("Settings file could not be opened.");
	}
}

Settings::~Settings()
{
}