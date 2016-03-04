#pragma once

#include <fstream>
#include "Timer.h"

class Logger
{
public:

	Logger()	{}

	static void Initialize()
	{
		std::ofstream outFile;
		outFile.open("log.log", std::ios::out | std::ios::trunc);
		outFile.close();
	}

	static void Log(const char* message)
	{
		std::ofstream outFile;
		outFile.open("log.log", std::ios::app);
		outFile << "( " << Timer::GetTime() << " sec ) \t:\t";
		outFile << message;
		outFile << "\n";
		outFile.close();
	}

	~Logger()	{}

private:
	static int currentLine;
};