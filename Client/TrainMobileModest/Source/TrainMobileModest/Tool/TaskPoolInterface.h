//// As Part of GuangZhou Training.
//
#pragma once
#include "TaskData.h"
#include "TaskPoolInterface.generated.h"
//
UINTERFACE(MinimalAPI)
//
class UTaskPoolInterface :public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class TRAINMOBILEMODEST_API ITaskPoolInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	//script statistic
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	int GetTaskScriptCount();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	FScriptTaskData GetTaskScript(const int& script_index);

	//task content
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable,Category = "DispatchNotify")
	FTaskData GetTaskDataTableAssetAtRowName(const FString& table_name,const FString& row_name);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	FTaskData GetTaskDataTableAssetAtRow(const FString& table_name,const int& row_index);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	int GetTaskDataTableAssetRowCount(const FString& table_name);

	//task item texture
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	FRoleGoodsTexture GetRoleGoodsAtRoleName(const FString& table_name,const FString& role_name);

	//task item actions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	FRoleTaskItemAction GetRoleActionsAtRoleName(const FString& table_name,const FString& role_name);

	//role first sight data.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	FRoleFirstSight GetRoleFirstSightAtRoleName(const FString& table_name,const FString& role_name);

	//finite state machine restoring.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyRestorFsmFunction();
};