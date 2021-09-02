#include "RunableWorker.h"
#include "TrainOnlinePrivatePCH.h"

FRunableWorker::FRunableWorker()
	:  StopTaskCounter(0)
{
}

FRunableWorker::~FRunableWorker()
{
}

//Done?
inline bool FRunableWorker::IsFinished() const
{
	return 0 != StopTaskCounter.GetValue();
}

//Init
bool FRunableWorker::Init()
{
	//Init the Data 
	return true;
}

//Run
uint32 FRunableWorker::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);
	//While not told to stop this thread 
	while (StopTaskCounter.GetValue() == 0 && !IsFinished())
	{
		//do as what you want.
	}
	return 0;
}

//stop
void FRunableWorker::Stop()
{
	StopTaskCounter.Increment();
}

void FRunableWorker::EnsureCompletion()
{
	Stop();
	if(Thread.Get())
		Thread->WaitForCompletion();
}
