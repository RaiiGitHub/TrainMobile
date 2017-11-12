// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "TaskBase.h"
#include "TaskStateMachine.h"
#include "TaskPoolInterface.h"
#include "GameUtilityBlueprintStruct.h"
#include "TaskPool.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ATaskPool : 
	public AActor,
	public ITaskPoolInterface,
	public TaskReader
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATaskPool();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	TaskStateMachine task_fsm_;
protected:
	RUNTIME_TASK_BASE::TaskScriptPtr current_task_script_ptr_;
	RUNTIME_TASK_BASE::TaskPtr current_task_ptr_;
	RUNTIME_TASK_BASE::TaskList current_active_task_list_;
	RUNTIME_TASK_BASE::RoleList current_role_list_;//from blueprint
private:
	void ReadTask();
	bool PushToRolePool(const FString& role_name);
	void EmptyRolePool();
	bool ActiveRole(const RUNTIME_TASK_BASE::RolePtr& rp);
public:
	void SetCurrentTask(const RUNTIME_TASK_BASE::TaskPtr& task_ptr);
	RUNTIME_TASK_BASE::TaskPtr GetCurrentTask();
	RUNTIME_TASK_BASE::TaskList& GetCurrentActiveTaskList();
	RUNTIME_TASK_BASE::RolePtr GetRoleByRoleName(const FString& name);
	RUNTIME_TASK_BASE::TaskPtr GetSpecificTask(int task_index);
	STATIC_TASK_BASE::TaskItemPtr GetSpecificTaskItem(int task_index, int item_index);
	STATIC_TASK_BASE::TaskItemActionPtr GetSpecificTaskItemAction(int task_index, int item_index, int act_index);
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FTaskData ReadDataTableRow(const FString& table_name, const int& row_index);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void PullDataTable();//pull data from blueprint, target is taskbase.
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FTaskData GetTaskRowAt(const FString& script_name,int index);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetTaskRowCount(const FString& script_name);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsExistExecutingTask();
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnterTaskScript(const FString& script_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnterTaskPoolAsClaimRole(const FString& role_name);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddRoleToPool(const FString& role_name);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void EmptyRoleFromPool();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();//reset all task datas.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ResetActions();//reset actions. e.g. status and operation records.
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SwitchRoleInRolePool(const FString& role_name);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskItemIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskItemActionIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskItemAwakeActionIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskItemActionFinishedIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetTaskItemActionIndexInPage(int pageSize,int indexInPage);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsCurrentTaskItemActionsFinished();//whether all of the current task item actions finished.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsCurrentTaskItemsFinished();//whether all of the current task items finished.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FString>& GetCurrentTaskItemGoodsTex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FTaskItemTex& GetCurrentTaskItemGoods(int goodsIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FTaskItemAction& GetCurrentTaskItemAction(int actIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FRoleFirstSight& GetCurrentRoleFirstSight();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentTaskItemActionsCount();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentRoleCount();
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetRoleIndex(int index_of_role);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetIndexOfRole(int role_index);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetIndexOfRoleByRoleName(const FString& role_name);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetRoleName(int index_of_role);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetCurrentRoleName();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetCurrentTaskScriptName();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetCurrentTaskItemContent();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetTaskItemContentByRoleName(const FString& role_name,FString& task_node,TArray<FString>& task_item ) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetTaskItemActionByRoleName(const FString& role_name, TArray<FTaskItemAction>& actions ) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FRoleImageData> GetRoleImageDataByRoleName(const FString& role_name) const;//current active script.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FRoleImageData> GetRoleImageData() const;//current active script.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FRoleMovieData> GetRoleMovieDataByRoleName(const FString& role_name) const;//current active script.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FRoleMovieData> GetRoleMovieData() const;//current active script.
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentRoleIndex();
 	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetCurrentRoleIndexOfRole();
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ExecuteFsmNextFunction(const FString& next_symbol);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ExecuteFsmOutputFunction(const FString& next_symbol);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RestoreFsmCurrentFunction();//once the role changed,this function should be call in order to restore the previous fsm function.
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetFsmCurrentFunctionStateName();
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetFsmNextFunctionStateName();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	class ATaskSequencer* GetTaskSequencer();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FTaskItemActionStatistics GetStatisticsItemActionsByRoleName(const FString& role_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void GetAllTaskItemActionWithSuggestOrder(TArray<FTaskItemAction>& actions,const int script_index);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void GetRoleTaskItemActionWithSuggestOrder(TArray<FTaskItemAction>& actions, const int script_index,const int role_index);

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetTaskScriptTableNameByMeta(const TArray<FTaskDataMeta>& tdm,ETaskDataMetaType type) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FScriptTaskDataStatistics GetTaskScriptStatistics(const FString& script_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetTaskScriptNum();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FScriptTaskData GetTaskScriptByName(const FString& script_name) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FScriptTaskData GetTaskScriptByIndex(const int index);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetActiveScript(const FString& script_name,bool active = true );

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	EScriptState IsActiveScript(const FString& script_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetActiveScriptName() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FString> GetScriptTypeNames() const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FString> GetScriptNamesOfType(const FString& script_type_name) const;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<FScriptTaskData> GetScriptEssentialOfType(const FString& script_type_name) const;
};
