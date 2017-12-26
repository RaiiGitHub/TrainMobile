#pragma once

class FTrainOnlineInterface;

class TRAINONLINE_API FHttpWorker
{
public:
	FHttpWorker();
	~FHttpWorker();
public:
	void InitHttpServerAddr(const FString& addr);
	void VisitUrl(const FString& url, FTrainOnlineInterface* pInterface);//will connect to server address.
public:
	FString HttpServerAddress;
};

