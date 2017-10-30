// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TaskSequencer.h"
#include "TaskPool.h"
#include "CallbackActor.h"
#include "SpriteActor.h"
#include "TrainGameInstance.h"


// Sets default values
ATaskSequencer::ATaskSequencer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATaskSequencer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATaskSequencer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool ATaskSequencer::SetupActionSpritToClick(const FTaskItemAction& act )
{
	if (!act.ActRelateToSpriteLocation)
		return false;
	ASpriteActor* spritActor = FindObject<ASpriteActor>(ANY_PACKAGE, TEXT("ClickSpritActor"));
	if (nullptr == spritActor)
		return false;
	spritActor->SpriteComeOutPlease(FString::Printf(TEXT("Actived-%s"), *act.ActDesc));
	spritActor->SetActorLocation(act.ActSpriteLocation);
	return true;
}

bool ATaskSequencer::FinishActionSpritClicked(const FTaskItemAction & act)
{
	if (!act.ActRelateToSpriteLocation)
		return false;
	ASpriteActor* spritActor = FindObject<ASpriteActor>(ANY_PACKAGE, TEXT("ClickSpritActor"));
	if (nullptr == spritActor)
		return false;
	spritActor->SpriteGetAwayPlease(FString::Printf(TEXT("Finished-%s"), *act.ActDesc));
	return true;
}

bool ATaskSequencer::UpdateStatusOfItemAction(class ATaskPool* task_pool, int task_index, int item_index, int act_index, EBaseState base_state, const FString op_desc)
{
	if (nullptr == task_pool)
		return false;
	STATIC_TASK_BASE::TaskItemActionPtr tia = task_pool->GetSpecificTaskItemAction(task_index, item_index,act_index);
	if (nullptr == tia.Get())
		return false;
	FDateTime now = FDateTime::Now();
	tia->state = base_state;
	tia->action_operation_result_ = op_desc;
	tia->action_operation_date_time_ = FString::Printf(TEXT("%d-%02d-%02d %02d:%02d:%02d")
		,now.GetYear()
		,now.GetMonth()
		,now.GetDay()
		,now.GetHour()
		,now.GetMinute()
		,now.GetSecond());
	if (EBaseState::BState_FINISHED == base_state)
		tia->action_operation_end_time_ = GetWorld()->GetTimeSeconds();
	//multi-player support.
	UTrainGameInstance* ins = static_cast<UTrainGameInstance*>(GetGameInstance());
	if (ins)
		ins->UpdateLobbyUserAction(task_index, item_index, act_index, base_state);
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	30.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("UpdateStatusOfItemAction ~> task index:%d,item index:%d,action index:%d, update status:%s %s"),
	//		task_index,item_index,act_index,
	//		EBaseState::BState_ACTIVED == base_state?TEXT("BState_ACTIVED"):
	//		(EBaseState::BState_FINISHED == base_state ?TEXT("BState_FINISHED"):TEXT("BState_AWAKED") ),*op_desc));


	//notify callback actor.
	ACallbackActor* callbackActor = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
	if (callbackActor)
	{
		callbackActor->SetCallbackType(Callback_NotifyActionRespond);
		callbackActor->NotifyTaskItemActionStateChanged(task_index, item_index, act_index, base_state, op_desc);
	}
	return true;
}

int ATaskSequencer::ActiveNextTaskItemAction(class ATaskPool* task_pool)
{
	if (nullptr == task_pool)
		return 0;
	int act_count = task_pool->GetCurrentTaskItemActionsCount();
	for (int i = 1; i <= act_count; ++i)
	{
		FTaskItemAction act = task_pool->GetCurrentTaskItemAction(i);
		if (act.ActState == EBaseState::BState_AWAKED)
		{
			UpdateStatusOfItemAction(task_pool, act.TaskNodeIndex, act.TaskItemIndex, i, EBaseState::BState_ACTIVED);
			SetupActionSpritToClick(act);
			return i;
		}
	}
	return 0;
}

bool ATaskSequencer::BeginToCountTimeForCurAction(ATaskPool * task_pool, int page_size, int index_in_page)
{
	if (nullptr == task_pool)
		return false;
	int act_index = task_pool->GetTaskItemActionIndexInPage(page_size, index_in_page);
	int task_index = task_pool->GetCurrentTaskIndex();
	int item_index = task_pool->GetCurrentTaskItemIndex();

	STATIC_TASK_BASE::TaskItemActionPtr tia = task_pool->GetSpecificTaskItemAction(task_index, item_index, act_index);
	if (nullptr == tia.Get())
		return false;

	tia->action_operation_start_time_ = GetWorld()->GetTimeSeconds();

	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	30.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("count~>ti%d,ii:%d,ai:%d, ts:%f"),
	//		task_index,item_index, act_index, tia->action_operation_start_time_));

	return true;
}

int ATaskSequencer::FinishCurrentTaskItemAction(ATaskPool * task_pool, const FString & op_desc)
{
	if (nullptr == task_pool)
		return 0;
	int act_count = task_pool->GetCurrentTaskItemActionsCount();
	for (int i = 1; i <= act_count; ++i)
	{
		FTaskItemAction act = task_pool->GetCurrentTaskItemAction(i);
		if (act.ActState == EBaseState::BState_ACTIVED)
		{
			UpdateStatusOfItemAction(task_pool, act.TaskNodeIndex, act.TaskItemIndex, i, EBaseState::BState_FINISHED,op_desc);
			FinishActionSpritClicked(act);
			return i;
		}
	}
	return 0;
}

bool ATaskSequencer::UpdateStatusOfCurrentActiveAction(ATaskPool * task_pool, EBaseState base_state, const FString op_desc)
{
	if (nullptr == task_pool)
		return false;
	int act_index = task_pool->GetCurrentTaskItemActionIndex();
	if (act_index <= 0)
		return false;
	FTaskItemAction action = task_pool->GetCurrentTaskItemAction(act_index);
	return UpdateStatusOfItemAction(task_pool, action.TaskNodeIndex, action.TaskItemIndex, act_index, base_state, op_desc);
}

bool ATaskSequencer::ForceSetupActionSpritToClick(ATaskPool * task_pool, int task_index, int item_index, int act_index, const FString op_desc)
{
	if (nullptr == task_pool)
		return false;
	STATIC_TASK_BASE::TaskItemActionPtr tia = task_pool->GetSpecificTaskItemAction(task_index, item_index, act_index);
	if (nullptr == tia.Get())
		return false;
	FTaskItemAction act;
	act.ActSpriteLocation = tia->action_sprite_location_;
	act.ActSpriteScale = tia->action_sprite_scale_;
	act.ActRelateToSpriteLocation = tia->relate_to_sprite_location_;
	act.ActDesc = op_desc;
	SetupActionSpritToClick(act);
	return true;
}

