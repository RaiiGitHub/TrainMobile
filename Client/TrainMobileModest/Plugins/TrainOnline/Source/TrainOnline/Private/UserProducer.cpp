#include "UserProducer.h"
#include "TrainOnlinePrivatePCH.h"

FString FUserProducer::InnerVersion = TEXT("v1.2.0608");

FUserProducer::FUserProducer()
	:TrainOnlineInterface(nullptr)
{
	ClientUser = MakeShareable(new SUser);
}


FUserProducer::~FUserProducer()
{
}

void FUserProducer::Init(const FString & server_addr, FTrainOnlineInterface* pInterface)
{
	if ( nullptr == HttpWorker.Get())
		HttpWorker = MakeShareable(new FHttpWorker);
	TrainOnlineInterface = pInterface;
	HttpWorker->InitHttpServerAddr(server_addr);
	PendingGetVersion();
}

bool FUserProducer::IsInit()
{
	return nullptr != HttpWorker.Get();
}

void FUserProducer::AcceptLogin(const SUser & u)
{
	check(ClientUser.Get());
	*ClientUser.Get() = u;
}

bool FUserProducer::PendingLogin(const FString & id, const FString & psw)
{
	if (nullptr == HttpWorker.Get() 
		|| nullptr == TrainOnlineInterface)
		return false;
	SUser u(*ClientUser.Get());
	if (u.logining_)
		return true;//already pending...
	u.id_ = id;
	u.psw_ = psw;
	u.permission_ = false;
	u.logining_ = true;
	AcceptLogin(u);
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("user_login"));
	HttpWorker->VisitUrl(FString::Printf(TEXT("fs_user_login.php?id=%s&psw=%s"), *id, *psw), TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingAcceptDetailInfo(const FString& id)
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get())
		return false;
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("user_info"));
	HttpWorker->VisitUrl(FString::Printf(TEXT("fs_user_get_info.php?id=%s"), *id), TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingGetVersion()
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface)
		return false;
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("version"));
	HttpWorker->VisitUrl(FString::Printf(TEXT("fs_version.php")), TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingVerifyUserExist(const FString& id)
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface)
		return false;
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("user_id_exist_verify"));
	HttpWorker->VisitUrl(FString::Printf(TEXT("fs_user_id_exist_verify.php?id=%s"),*id), TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingHandshake()
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface)
		return false;
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("handshake"));
	HttpWorker->VisitUrl(TEXT("fs_handshake.php"), TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingRegisterCurrentUser()
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get())
		return false;

	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("user_register"));
	FString url = FString::Printf(TEXT("fs_user_register.php?id=%s&psw=%s&name=%s")
		, *ClientUser->id_, *ClientUser->psw_, *ClientUser->name_);
	for (auto& kv : ClientUser->other_field_value_)
	{
		url.Append(FString::Printf(TEXT("&%s=%s"), *kv.Key, *kv.Value));
	}
	HttpWorker->VisitUrl(url, TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingSumbitCurrentActionExp(int add_exp, int add_exp_duration)
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get()
		|| !ClientUser->permission_
		|| ClientUser->id_.Equals(TEXT("su")))
		return false;//offline mode will not be allowed.
	ClientUser->exp_ += add_exp;
	ClientUser->exp_duration_ += add_exp_duration;
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("user_update_info"));
	FString url = FString::Printf(TEXT("fs_user_update_info.php?id=%s&psw=%s&exp=%d&exp_duration=%d")
		,*ClientUser->id_, *ClientUser->psw_, 
		FPlatformMath::FloorToInt(ClientUser->exp_), FPlatformMath::FloorToInt(ClientUser->exp_duration_));
	HttpWorker->VisitUrl(url, TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingRecieveUserCmd()
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get()
		|| !ClientUser->permission_
		|| ClientUser->id_.Equals(TEXT("su")))
		return false;//offline mode will not be allowed.
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("respond_command"));
	FString url = FString::Printf(TEXT("fs_respond_command.php?id=%s&psw=%s&clear_cmd=1")
		, *ClientUser->id_, *ClientUser->psw_);
	HttpWorker->VisitUrl(url, TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingReadUserRankResult(const FString& id)
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get()
		|| !ClientUser->permission_
		|| ClientUser->id_.Equals(TEXT("su")))
		return false;//offline mode will not be allowed.
	TrainOnlineInterface->PutInHttpMethodDesc(TEXT("respond_user_rank_result"));
	FString url = FString::Printf(TEXT("fs_user_get_info.php?id=%s")
		, *id);
	HttpWorker->VisitUrl(url, TrainOnlineInterface);
	return true;
}

bool FUserProducer::PendingVisitUrl(const FString & url,const FString& method)
{
	if (nullptr == HttpWorker.Get()
		|| nullptr == TrainOnlineInterface
		|| nullptr == ClientUser.Get()
		|| !ClientUser->permission_)
		return false;
	ATrainOnlineActor::GetHandler()->PutInHttpMethodDesc(method);
	HttpWorker->VisitUrl(url, TrainOnlineInterface);
	return true;
}

TSharedPtr<SUser>& FUserProducer::GetClientUser()
{
	// TODO: insert return statement here
	return ClientUser;
}

TSharedPtr<FHttpWorker>& FUserProducer::GetHttpWorker()
{
	// TODO: insert return statement here
	return HttpWorker;
}
