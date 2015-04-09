#pragma once
#include <queue>
#include <mutex>
#include <thread>

/**
	The idea of this class is to intrinsically support threading.
	This is done through the use of a job list, an array of threads,
	and a method which allows passing of jobs to the job completer.
*/


class JobCompleter
{
private:
	unsigned int maxSupportedThreads;
	unsigned int activeThreads;

	bool toTerminate;

	std::vector<std::thread> threadArray;
	std::mutex jobQueueMut;
	std::queue<std::function<void()>> jobQueue;


public:
	JobCompleter();
	JobCompleter(unsigned int threads);
	~JobCompleter();

	void GiveJob(std::function<void()>& job);
	bool JobsLeft();

private:
	void Initialize(unsigned int requestedThreads);
	void acquireJob();

};

