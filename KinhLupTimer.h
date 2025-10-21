#pragma once
#include<chrono>

class KinhLupTimer
{
public:
	KinhLupTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};