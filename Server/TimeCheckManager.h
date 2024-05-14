#pragma once

#include <iostream>
#include <chrono>

class TimeCheckManager
{
public:
	

	void StartTime();
	void EndTime();

	void PrintTime();




private:
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _end;
};

extern TimeCheckManager GTimeCheckManager;