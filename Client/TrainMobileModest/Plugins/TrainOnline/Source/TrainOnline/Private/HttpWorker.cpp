#include "TrainOnlinePrivatePCH.h"
#include "HttpWorker.h"
#include "PlatformHttp.h"

FHttpWorker::FHttpWorker()
	:HttpServerAddress(TEXT("http://127.0.0.1:80/"))
{
}


FHttpWorker::~FHttpWorker()
{
}

void FHttpWorker::InitHttpServerAddr(const FString & addr)
{
	HttpServerAddress = addr;
}

void FHttpWorker::VisitUrl(const FString & url)
{
	if (nullptr == ATrainOnlineActor::GetHandler())
		return;
	//we need to url encode for special characters (especially other languages)
	FString UrlEncodedString = HttpServerAddress + FPlatformHttp::UrlEncode(url);
	TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("text/plain; charset=utf-8"));
	HttpRequest->SetURL(UrlEncodedString);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->OnProcessRequestComplete().BindUObject(ATrainOnlineActor::GetHandler(), &ATrainOnlineActor::HttpCompleteCallback);
	HttpRequest->ProcessRequest();
}
