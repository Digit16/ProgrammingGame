#include <algorithm>;
#include "Timer.h"


Timer::Timer(double time) : duration(time), time(time) {}

void Timer::update(double delta) {
	time -= delta;
}

double Timer::remaining() {
	return std::max(0.0, time);
}

bool Timer::done() {
	return time <= 0;
}

void Timer::restart() {
	time = duration;
}
