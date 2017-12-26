#pragma once
#include "Http.h"
#include "Json.h"
#include "UObject/NoExportTypes.h"
#include "HttpCompleteFunctionHolder.generated.h"

/**
*
*/
UCLASS()
class UHttpCompleteFunctionHolder 
	:public UObject
{
	GENERATED_BODY()
public:
	friend class FTrainOnlineInterface;
	void HttpCompleteCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
protected:
	class FTrainOnlineInterface* LogicHolder = nullptr;
};