#include "JobCompleter.h"

typedef unsigned int uint;

JobCompleter::JobCompleter()
{
	Initialize(std::thread::hardware_concurrency());
}

JobCompleter::JobCompleter(uint threads){
	Initialize(threads);
}


JobCompleter::~JobCompleter()
{
}

void JobCompleter::Initialize(uint requestedThreads){

	maxSupportedThreads = std::thread::hardware_concurrency();

	if (requestedThreads <= 1 || maxSupportedThreads < 1) activeThreads = 1;
	else if (requestedThreads > maxSupportedThreads) activeThreads = maxSupportedThreads;
	else activeThreads = requestedThreads;

	for (uint i = 0; i < activeThreads; ++i){
		threadArray.emplace_back(&JobCompleter::acquireJob, this);
	}
}

void JobCompleter::acquireJob(){

	while (!toTerminate){

		std::function<void()> func = std::function<void()>();

		jobQueueMut.lock();
		if (!jobQueue.empty()){
			func = jobQueue.front();
			jobQueue.pop();
		}
		jobQueueMut.unlock();

		if (func) func();
		else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void JobCompleter::GiveJob(const std::function<void()>& job){

	jobQueueMut.lock();
	jobQueue.push(job);
	jobQueueMut.unlock();
}

void JobCompleter::CloseOnceComplete(){

	//While there are jobs left wait
	while (!jobQueue.empty()){
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	//Once jobs are all allocated, indicate to threads they have
	//completed their last job
	toTerminate = true;

	//Attempt to join to all threads
	for (uint i = 0; i < threadArray.size(); ++i)
		if (threadArray[i].joinable()) threadArray[i].join();
	
	//Empty the thread array, (if it isnt already empty)
	while (!threadArray.empty()){
		threadArray.pop_back();
	}
}