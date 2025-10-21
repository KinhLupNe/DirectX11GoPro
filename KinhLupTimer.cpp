#include"KinhLupTimer.h"
using namespace std::chrono;

KinhLupTimer::KinhLupTimer()
{
	last = steady_clock::now();
}
float KinhLupTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}
float KinhLupTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}