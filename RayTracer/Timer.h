#pragma once

#include <ctime>

class Timer
{
public:
	Timer()		{}

	static void Initialize()
	{
		beginningTime = clock();
		lastTime = beginningTime;
	}
	static double GetTime()
	{
		lastTime = clock();
		double elapsedTime = ((double)(lastTime - beginningTime)) / CLOCKS_PER_SEC;
		return elapsedTime;
	}

	~Timer()	{}

private:
	static clock_t	beginningTime;
	static clock_t	lastTime;
};