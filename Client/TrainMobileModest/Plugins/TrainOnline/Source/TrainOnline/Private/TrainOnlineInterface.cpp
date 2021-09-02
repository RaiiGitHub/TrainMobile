#include "TrainOnlineInterface.h"
#include "TrainOnlinePrivatePCH.h"
#include "TrainOnline.h"
#include "HttpCompleteFunctionHolder.h"

void FTrainOnlineInterface::PutKeyValInPendingUser(const FString & key, const FString & val)
{
	TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
	if (u.Get())
	{
		if (0 == key.Compare(TEXT("psw"), ESearchCase::IgnoreCase))
			u->psw_ = val;
		else if (0 == key.Compare(TEXT("id"), ESearchCase::IgnoreCase))
			u->id_ = val;
		else if (0 == key.Compare(TEXT("name"), ESearchCase::IgnoreCase))
			u->name_ = val;
		else
			u->other_field_value_.Add(key, val);
	}
}

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
