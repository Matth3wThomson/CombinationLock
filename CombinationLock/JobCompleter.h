#pragma once
#include <queue>
#include <mutex>
#include <thread>

/**
	Class: JobCompleter
	Author: Matthew Thomson 2015
	
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
	bool* threadsActive;
	std::mutex jobQueueMut;
	std::queue<std::function<void()>> jobQueue;


public:
	JobCompleter();
	JobCompleter(unsigned int threads);
	~JobCompleter();

	void GiveJob(const std::function<void()>& job);

	/* Blocking function that will close all threads once they complete their
	   tasks and render the job completer useless until destruction. */
	void CloseOnceComplete();

	//bool JobsLeft();

private:
	void Initialize(unsigned int requestedThreads);
	void acquireJob();

};

