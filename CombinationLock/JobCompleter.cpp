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
		threadArray.push_back(std::thread(&JobCompleter::acquireJob, this));
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

		//TODO: Work out if this is the correct way to go about this.
		if (func) func();
		else std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void JobCompleter::GiveJob(std::function<void()>& job){

	jobQueueMut.lock();
	jobQueue.push(job);
	jobQueueMut.unlock();
}

bool JobCompleter::JobsLeft(){

	//No need for lock here as queues are internally thread safe
	return !jobQueue.empty();
}