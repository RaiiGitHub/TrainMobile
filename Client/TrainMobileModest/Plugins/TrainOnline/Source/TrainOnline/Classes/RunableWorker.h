#pragma once
//~~~~~ Multi Threading ~~~
class TRAINONLINE_API FRunableWorker : public FRunnable
{
public:
	//Constructor / Destructor
	FRunableWorker();
	virtual ~FRunableWorker();
	/** Thread to run the worker FRunnable on */
	TSharedPtr<FRunnableThread> Thread;
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
public:
	bool IsFinished() const;
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
};