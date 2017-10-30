// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "TaskBase.h"
#include "TaskData.h"
#include "TaskSequencer.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ATaskSequencer 
	: public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATaskSequencer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	//setup action sprite to click while active actions.
	bool SetupActionSpritToClick(const FTaskItemAction& act);
	//Finish clicked action sprite.
	bool FinishActionSpritClicked(const FTaskItemAction& act);
public:
	//specific task respond content.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchTaskNotify")
	bool SetAwakeCurrentTaskFromFsm();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchTaskNotify")
	bool SetActiveCurrentTaskFromFsm();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchTaskNotify")
	bool SetFinishCurrentTaskFromFsm();
public:
	//update status of actions in items
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	bool UpdateStatusOfItemAction(class ATaskPool* task_pool,int task_index,int item_index, int act_index,EBaseState base_state,const FString op_desc=TEXT(""));

	//action sequencer. active one by one in current active task item.
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	int ActiveNextTaskItemAction(class ATaskPool* task_pool);

	//action sequencer. record the time elaspe value in the world,as the starting-operation time of the current action.the action may be awaked or actived.
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	bool BeginToCountTimeForCurAction(class ATaskPool* task_pool,int page_size,int index_in_page);

	//action sequencer. finish current actived action
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	int FinishCurrentTaskItemAction(class ATaskPool* task_pool, const FString& op_desc = TEXT(""));

	//update current active action in item
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	bool UpdateStatusOfCurrentActiveAction(class ATaskPool* taskPool, EBaseState base_state, const FString op_desc = TEXT(""));

	//action sequencer.forcely setup sprit to click.
	UFUNCTION(BlueprintCallable, Category = "TaskSequencer")
	bool ForceSetupActionSpritToClick(class ATaskPool* task_pool, int task_index, int item_index, int act_index, const FString op_desc = TEXT(""));
};
