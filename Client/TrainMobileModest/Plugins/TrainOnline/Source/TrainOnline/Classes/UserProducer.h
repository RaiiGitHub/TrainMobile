#pragma once
//using http request to communicate with server.
#include "HttpWorker.h"
struct SUser {
public:
	SUser() :rank_(0), exp_(0.f), exp_duration_(0.f), ability_(0.f), exp_to_lv_param1_(2.f), exp_to_lv_param2_(1.f), permission_(false), logining_(false), sokcet_server_permission_(false),socket_server_port_(0){}
	FString id_;
	FString psw_;
	FString name_;
	FString last_login_time_;
	FString version_;
	FString apk_url_;
	FString socket_server_ip_;
	FString download_tips_;
	FString no_wifi_tips_;
	FString download_caption_;
	int socket_server_port_;
	int rank_;
	double exp_;
	double exp_duration_;
	double exp_to_lv_param1_;
	double exp_to_lv_param2_;//y = p1*ln(p2*x)
	double  ability_;
	bool permission_;//false if login failed.
	bool logining_;//indicated that wether is loging in.
	bool sokcet_server_permission_;//false if socket login failed.
	TMap<FString, FString> other_field_value_;
};

class TRAINONLINE_API FUserProducer
{
public:
	FUserProducer();
	~FUserProducer();
public:
	void Init(const FString& server_addr);
	void AcceptLogin(const SUser& u);
	bool PendingLogin(const FString& id, const FString& psw);
	bool PendingAcceptDetailInfo(const FString& id);
	bool PendingGetVersion();
	bool PendingVerifyUserExist(const FString& id);
	bool PendingHandshake();
	bool PendingRegisterCurrentUser();
	bool PendingSumbitCurrentActionExp(int add_exp,int add_exp_duration);//can be below zero.
	bool PendingRecieveUserCmd();
	bool PendingReadUserRankResult(const FString& id);
	bool PendingVisitUrl(const FString& url,const FString& method);
	TSharedPtr<SUser>& GetClientUser();
	TSharedPtr<FHttpWorker>& GetHttpWorker();
	static FString InnerVersion;
protected:
	TSharedPtr<FHttpWorker> HttpWorker;
	TSharedPtr<SUser> ClientUser;
};

