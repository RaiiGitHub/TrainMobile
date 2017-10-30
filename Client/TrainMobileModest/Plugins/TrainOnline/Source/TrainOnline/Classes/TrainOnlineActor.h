// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "Http.h"
#include "Json.h"
#include "TrainOnlineActor.generated.h"

USTRUCT(BlueprintType)
struct FUserInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FUserInfo() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString PSW;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString LastLoginTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString Version;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString InnerVersion;//app inner version
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		float Ability;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		FString SocketServerIp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		int SocketServerPort;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		int Rank;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		int Exp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		int ExpDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserInfo)
		TMap<FString, FString> OtherFiledValue;
};

UENUM(BlueprintType)
enum ELoginStatus
{
	LS_PERMISSION_FAILED,
	LS_PERMISSION_SUCCESS,
	LS_PERMISSION_TOTAL
};

UENUM(BlueprintType)
enum ELobbyMemberStatus
{
	LMS_NOT_READY,
	LMS_READY,
	LMS_CONNECTED,
	LMS_DISCONNECTED
};

USTRUCT(BlueprintType)
struct FMemberInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FMemberInfo() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMemberInfo)
		FString ID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMemberInfo)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMemberInfo)
		bool IsHost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FMemberInfo)
		TEnumAsByte<ELobbyMemberStatus> Status;
};

USTRUCT(BlueprintType)
struct FLobbyInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FLobbyInfo() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLobbyInfo)
		FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLobbyInfo)
		float AverAbility;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLobbyInfo)
		TArray<FMemberInfo> Members;
};

UCLASS()
class TRAINONLINE_API ATrainOnlineActor : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupHandler();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FUserInfo GetUserInfo() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetHttpServerAddr() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetSokcetServerName() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int ExpToLevel(int exp) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void PutKeyValInPendingUser(const FString& key,const FString& val);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FLobbyInfo GetCurrentLobby() const;

public:
	static ATrainOnlineActor* GetHandler();
	void HttpCompleteCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void PutInHttpMethodDesc(const FString& desc);
	FString PopOutHttpMethodDesc(bool pop = true);
private:
	static ATrainOnlineActor* Handler;
private:
	TQueue<FString> HttpMethodQueue;
	TSharedPtr<class FSocketEnvWorker> SocketEnvWorker;
public:
	//login page
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyLoginMsg(ELoginStatus ls);

	//register page
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyRegisterMsg(bool success);

	//throw error.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyHttpRequestErr(const FString& msg);

	//verify user id exist
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyUserIDExistVerify(const FString& id,bool exist);

	//notify handshake.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyHandshake(const FString& msg,bool shake);

	//notify update exp.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyUpdateUserExp();

	//notify user command.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyRespondUserCommand(const FString& cmd,bool exist);

	//notify connected socketserver(skynet). as an event.do not has return value.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifySocketServerConnected(const FString& user_id,const FString& socket_name,bool recon);

	//notify newer version of APK
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifyNewerAPKVersion(const FString& APKUrl,const FString& confirm_msg);

	//notify user rank-data is ready.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifyUserRankDataReady();

	//notify user rank-data is uploaded.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifyUserRankDataUploaded();

};
