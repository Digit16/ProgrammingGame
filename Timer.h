#pragma once

class Timer {
public:
	Timer(double time);

	void update(double delta);
	double remaining();
	bool done();
	void restart();

private:
	double duration;
	double time;
};
