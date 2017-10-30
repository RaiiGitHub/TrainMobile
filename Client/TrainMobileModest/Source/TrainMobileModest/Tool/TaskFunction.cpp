// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TaskStateMachine.h"
#include "TaskFunction.h"
#include "TaskState.h"
#include "TaskSequencer.h"
#include "TaskPool.h"
#include "TrainGameInstance.h"

bool TaskFunction_SetAwakeTask::Explain()
{
	if( nullptr == own_state_.Get() )
		return false;
	TaskState_Awake* awake = static_cast<TaskState_Awake*>(own_state_.Get());
	ATaskPool* task_pool = awake->GetTaskPool();
	if (awake->GetTaskItem().Get())
	{
		awake->GetTaskItem()->state = EBaseState::BState_AWAKED;
		//awake all actions,at the very begin.
		UTrainGameInstance* ins = static_cast<UTrainGameInstance*>(task_pool->GetGameInstance());
		FDateTime now = FDateTime::Now();
		FString now_str = FString::Printf(TEXT("%d-%02d-%02d %02d:%02d:%02d")
			, now.GetYear()
			, now.GetMonth()
			, now.GetDay()
			, now.GetHour()
			, now.GetMinute()
			, now.GetSecond());
		for (auto& act : awake->GetTaskItem()->action_list_)
		{
			act->state = EBaseState::BState_AWAKED;
			act->action_operation_date_time_ = now_str;
			if (ins)
			{
				ins->UpdateLobbyUserAction(
					awake->GetTask()->task_index_,
					awake->GetTaskItem()->item_index_,
					act->action_index_,
					act->state);
			}
		}	
	}
	if (nullptr == task_pool || nullptr == task_pool->GetTaskSequencer() )
		return false;
	if (next_symbol_.Get())
	{
		task_pool->task_fsm_.current_state_.Reset();
		task_pool->task_fsm_.current_state_ = own_state_;
		task_pool->task_fsm_.SetNextStateFunction(next_symbol_->symbol_);
	}
	task_pool->GetTaskSequencer()->SetAwakeCurrentTaskFromFsm();
	return true;
}

bool TaskFunction_SetActiveTask::Explain()
{
	if (nullptr == own_state_.Get())
		return false;
	TaskState_Active* active = static_cast<TaskState_Active*>(own_state_.Get());
	ATaskPool* task_pool = active->GetTaskPool();
	if (active->GetTaskItem().Get())
		active->GetTaskItem()->state = EBaseState::BState_ACTIVED;
	if (nullptr == task_pool || nullptr == task_pool->GetTaskSequencer())
		return false;
	if (next_symbol_.Get())
	{
		task_pool->task_fsm_.current_state_.Reset();
		task_pool->task_fsm_.current_state_ = own_state_;
		task_pool->task_fsm_.SetNextStateFunction(next_symbol_->symbol_);		
	}
	task_pool->GetTaskSequencer()->SetActiveCurrentTaskFromFsm();
	return true;
}

bool TaskFunction_SetFinishTask::Explain()
{
	if (nullptr == own_state_.Get())
		return false;
	TaskState_Finish* finish = static_cast<TaskState_Finish*>(own_state_.Get());
	ATaskPool* task_pool = finish->GetTaskPool();
	if (finish->GetTaskItem().Get())
	{
		finish->GetTaskItem()->state = EBaseState::BState_FINISHED;
		//finish all actions.
		for (auto& act : finish->GetTaskItem()->action_list_)
			act->state = EBaseState::BState_FINISHED;
	}
	if (nullptr == task_pool || nullptr == task_pool->GetTaskSequencer())
		return false;
	if (next_symbol_.Get())
	{
		task_pool->task_fsm_.current_state_.Reset();
		task_pool->task_fsm_.current_state_ = own_state_;
		task_pool->task_fsm_.SetNextStateFunction(next_symbol_->symbol_);
		bool meet_one = false;
		for (
			STATIC_TASK_BASE::TaskItemNode* tiNode = finish->GetTask()->task_item_list_.GetHead();
			nullptr != tiNode;
			tiNode = tiNode->GetNextNode())
		{
			//get next task item
			if (tiNode->GetValue() == finish->GetTaskItem())
			{
				//setup next.
				tiNode = tiNode->GetNextNode();
				if (nullptr != tiNode)
					finish->GetTask()->ProduceFSMComponent(&task_pool->task_fsm_, tiNode->GetValue());
				meet_one = true;
				break;
			}
		}
		if (!meet_one)
		{
			//next task in the role-active-task-list.
			finish->GetTask()->state = EBaseState::BState_FINISHED;
			for (
				RUNTIME_TASK_BASE::TaskNode* tNode = task_pool->GetCurrentActiveTaskList().GetHead();
				tNode != nullptr;
				tNode = tNode->GetNextNode()
				)
			{
				if (tNode->GetValue() == finish->GetTask())
				{
					tNode = tNode->GetNextNode();
					if( nullptr != tNode )
						task_pool->SetCurrentTask(tNode->GetValue());
					break;
				}
			}

		}	
	}
	task_pool->GetTaskSequencer()->SetFinishCurrentTaskFromFsm();
	return true;
}

FSM_FunctionPtr TaskFunction_Factory::ProduceTaskFunction(const FString & state_name, const FSM_StatePtr & own_state)
{
	if (state_name.Equals(TEXT("state_sleep")))
		return MakeShareable(new TaskFunction_SetSleepTask(own_state));
	else if (state_name.Equals(TEXT("state_awake")))
		return MakeShareable(new TaskFunction_SetAwakeTask(own_state));
	else if (state_name.Equals(TEXT("state_active")))
		return MakeShareable(new TaskFunction_SetActiveTask(own_state));
	else if (state_name.Equals(TEXT("state_suspend")))
		return MakeShareable(new TaskFunction_SetSuspendTask(own_state));
	else if (state_name.Equals(TEXT("state_finish")))
		return MakeShareable(new TaskFunction_SetFinishTask(own_state));
	return FSM_FunctionPtr();
}


