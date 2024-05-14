#include "pch.h"
#include "TimeCheckManager.h"

TimeCheckManager GTimeCheckManager;

void TimeCheckManager::StartTime()
{
	_start = std::chrono::steady_clock::now();

	std::cout << "Start Time" << std::endl;

}

void TimeCheckManager::EndTime()
{
	_end = std::chrono::steady_clock::now();

	std::cout << "End Time" << std::endl;
}

void TimeCheckManager::PrintTime()
{
	// 시간 차이 계산
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start);
	std::cout << "Duration Time : " << duration.count() << " milliseconds\n";
}
