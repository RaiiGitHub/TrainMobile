#include "TrainOnlinePrivatePCH.h"
#include "TrainOnline.h"
#include "TrainOnlineInterface.h"
#include "HttpCompleteFunctionHolder.h"

void FTrainOnlineInterface::SetupHttpFunction(UObject* outer)
{
	if (nullptr == HttpFunction)
	{
		HttpFunction = NewObject<UHttpCompleteFunctionHolder>(outer);
		HttpFunction->LogicHolder = this;
	}
}

void FTrainOnlineInterface::PutInHttpMethodDesc(const FString & desc)
{
	//method may probably come in in a very short time. so here's a list as a queue.
	HttpMethodQueue.Enqueue(desc);
}

FString FTrainOnlineInterface::PopOutHttpMethodDesc(bool pop)
{
	FString desc;
	pop ? HttpMethodQueue.Dequeue(desc) : HttpMethodQueue.Peek(desc);
	return desc;
}
