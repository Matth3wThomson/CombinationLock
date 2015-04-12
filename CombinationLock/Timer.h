#pragma once

#include <chrono>

class Timer
{
public:
	Timer(){ Reset();  };
	~Timer(){};

	void Reset(){ start = std::chrono::high_resolution_clock::now(); };

	inline std::chrono::milliseconds getTimeMilli() const{ 
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
	}

	inline std::chrono::nanoseconds getTimeNano() const{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start);
	}

private:
	std::chrono::high_resolution_clock::time_point start;
};

