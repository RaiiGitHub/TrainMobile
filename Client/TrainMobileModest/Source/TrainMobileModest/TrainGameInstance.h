// As Part of GuangZhou Training.

#pragma once

#include "Engine/GameInstance.h"
#include "TrainOnlineInterface.h"
#include "GameUtilityBlueprintStruct.h"
#include "TrainGameInstance.generated.h"
/**
 * 
 */
UCLASS()
class TRAINMOBILEMODEST_API UTrainGameInstance 
	: public UGameInstance
	, public FTrainOnlineInterface
{
	GENERATED_BODY()

public:
	//TrainOnlineInterface
	virtual void NotifyHttpRequestErr(const FString& msg, const FString& from_msg_type);
	virtual void NotifyLoginResponse(LoginStatus ls);
	virtual void NotifyUserRankDataReady();
	virtual void NotifyUserIDExistVerify(const FString& id, bool exist);
	virtual void NotifyHandshake(const FString& msg, bool shake);
	virtual void NotifyRegisterResponse(bool success);
	virtual void NotifyUpdateUserExp();
	virtual void NotifyRespondUserCommand(const FString& cmd, bool exist);
	virtual void NotifyUserRankDataUploaded();

public:

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetTrainMode(ETrainMode tm);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	ETrainMode GetTrainMode() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetTaskPool( class ATaskPool* task_pool);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetCallbackActor(class ACallbackActor* callback_actor);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsGameReady() const;

	void Init();
	void Shutdown();

public:
	//read or save config,using the ini file of userSettings.
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString ReadConfig(const FString& section,const FString& field);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool WriteConfig(const FString& section, const FString& field,const FString& value);

public:
	//htter server
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ServerInit(const FString& addr);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool UserLogin(const FString& id, const FString& psw);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool UserRegister();//register current user.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool UserAcceptDetailInfo(const FString& id);//accept detail info of the specific user. 

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool UserExistVerify(const FString& id);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Handshake();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SubmitCurrentActionExp(int add_exp,int add_exp_duration);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool UserHttpCommandRecieve();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ClearCookies();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool VersionVerify();//may trigger update,true for needing to update,otherwise don't.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsOfflineMode();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetOfflineMode();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RequestCurrentLobbyResult();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetCurrentLobbyResult();//will not be sure whether the result is ok.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool VisitUrlWithGetMethod(const FString& url,const FString& method);

public:
	//socket server
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ConnectSocketServer(const FString& socket_name, const FString server_ip, int32 port);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SendDataToSocketServer(const FString& socket_name, const FString& content, const FString proto_request=TEXT(""),bool using_sproto = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetupSocketReciever(const FString& socket_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString BuildHandshakeString();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool CreateLobby(const FString& name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool LeaveCurrentLobby();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool MatchLobby();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool LockLobby(bool lock = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ReadyForPlay(bool ready = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AssignLobbyUserActions();//send the user-actions to the server.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void UpdateLobbyUserAction(int nid,int tid,int aid,int status);//send the actioin-changing-status to the server.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AddUserPlayerRole(const FString& uid, const FString& role_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void RemoveUserPlayerRole(const FString& uid, const FString& role_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetUserPlayerRoles(const FString& uid, FUserAsRoles& roles);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool CheckUserRole(const FString& role_name,bool& is_local_role);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool CheckUserFinishRoleSelect(const FString& user_id);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int CheckUserRoleAllSelect();//will return the count of unselected roles.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetUserNameAsRole(const FString& role_name,FString& user_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetRoleNameAsUserID(const FString& user_id, FString& role_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetUserIDAsRoleName(const FString& role_name, FString& user_id);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetProperIndexOfRoleAsFirstRole(const FString& user_id,const TArray<int>& match_rule);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool DispatchMsgToLobbyMember(const FString& msg,bool as_host = false);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool DispatchMsgToWorld(const FString& msg);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AcceptActionStatusChange();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AcceptUserRoleSelectChange();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AddGameDescription(const FString& key,const FString& value);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetGameDescription(const FString& key,FString& value);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	ETaskActionPassbyResultType PassbyRoleTaskActionStatus(const int task_index,const int item_index,const int act_index, const int status);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SendCharacterTransform(const FString& pawn_name);

private:
	bool AcceptPassbyRoleTaskActionStatus(const FString& what);

public:
	static bool WrapperOfSocketPackageReciever(const uint8* data, const int32 len, const int32 nt);
	static void HandleSprotoDataFromQueue();
	static void LobbyStateCallFunc(class LobbyState* ls);

	FString BuildMovementString(const FVector& location, const FRotator& rotation, const FVector& scale, const FVector& velocity, const FString& animation);
	FString BuildMovementSendString(const FString& move_actor_name, const TArray<FString>& data, TArray<FString>* towhom = nullptr);//Read the fuck manaul!
	
	class ATaskPool* GetTaskPool();
private:
	ETrainMode TrainMode;
	class ATaskPool* TaskPool;
	class ACallbackActor* CallbackActor;
	TMap<FString, TPair<bool,TArray<FString>>> UserPlayRoles;
	TMap<FString, FString> GameDescriptions;
	bool GameReady = false;
	int TryMatchMaxTime = 100;
public:
	static UTrainGameInstance* Instance;
};
