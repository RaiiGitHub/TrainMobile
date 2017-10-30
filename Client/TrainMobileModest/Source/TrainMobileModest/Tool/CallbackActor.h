// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "TaskBase.h"
#include "GameUtilityBlueprintStruct.h"
#include "CallbackActor.generated.h"

UENUM(BlueprintType)
enum ECallbackType
{
	Callback_Unkown,
	Callback_NotifyHitResult,
	Callback_NotifyFirePercent,
	Callback_NotifyActionRespond,
	Callback_NotifyReachedTarget,
	Callback_NotifyCameraFlyToDefault,
	Callback_NotifyCameraFlyNeedToReset,
	Callback_NotifyLobbyUpdate,
	Callback_NotifyLobbyReadyAsked,
	Callback_NotifyLobbyLaunchedOn,
	Callback_NotifyLobbyRoleSelectFinished,
	Callback_NotifyScrollMessage,
	Callback_NotifyActorMovementDone,
	Callback_NotifyActionEffectReplication,
	Callback_NotifyMatchLobbyTryMaxTime,
	Callback_NotifySocketServerReconnect,
	Callback_NotifyMessageRecieve,
	Callback_NotifyTaskCanPassbyRecieve,
	Callback_NotifyHappyEnding
};

UCLASS()
class TRAINMOBILEMODEST_API ACallbackActor : 
	public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACallbackActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	ECallbackType CallbackType;
public:
	void SetCallbackType(ECallbackType type);
	void SetServerReconnect(const ESocketServerReconnect& ssr);
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetCallbackDataHitResult(FHitResult& hr);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	ECallbackType GetCallbackType();

private:
	//some callback functions that should call in the gamethread.
	ESocketServerReconnect ServerReconnect;
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyHitResult();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyHitResultFree(const FHitResult& hr);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyFirePercent(float percent, int ReachTargetPercent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyTaskItemActionStateChanged(int task_index,int item_index,int act_index,EBaseState es,const FString& op_desc = TEXT(""));

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyReachedTarget(int task_index, int item_index, int act_index,const FString& aspect_name,const FString& mesh_name);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyCameraFlyToDefaultDone();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyCameraFlyNeedToReset();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyLobbyUpdate();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyLobbyReadyAsked();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyLobbyLaunchedOn();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyLobbyRoleSelectFinished(bool finished);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyScrollMessage(const FString& msg,bool positive = true,float scroll_rate = 0.1f);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyActorMovementDone(AActor* actor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyActionEffectReplication(int task_index, int item_index, int act_index, EBaseState es);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyMatchLobbyTryMaxTime();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifySocketServerReconnect(ESocketServerReconnect essr);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyMessageRecieve(const FString& msg, const FString& from_uid,const FString& from_uname,ETalkMessageType em);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyTaskCanPassbyRecieve(int task_index,int item_index,int act_index, EBaseState pending_es);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyHappyEnding();
};
