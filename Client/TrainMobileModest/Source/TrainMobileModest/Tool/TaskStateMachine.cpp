// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TaskStateMachine.h"
#include "TaskFunction.h"
#include "TaskState.h"

TaskStateMachine::TaskStateMachine()
{
}

TaskStateMachine::~TaskStateMachine()
{
}

void TaskStateMachine::SetupFSM(const RUNTIME_TASK_BASE::TaskPtr& task_ptr, ATaskPool* task_pool)
{
	Reset();
	task_ptr_ = task_ptr;
	task_pool_ = task_pool;
	if (nullptr == task_ptr.Get() || nullptr == task_pool)
		return;
	//skip to the first unfinished task-item
	task_ptr_->state = EBaseState::BState_ACTIVED;
	STATIC_TASK_BASE::TaskItemPtr task_item;
	for (auto& item : task_ptr_->task_item_list_)
	{
		if (EBaseState::BState_FINISHED != item->state)
		{
			task_item = item;
			break;
		}
	}
	task_ptr_->ProduceFSMComponent(this,task_item);
}

void TaskStateMachine::Reset(bool bResetTask)
{
	FSM_Object::Clear();
	if (bResetTask)
		task_ptr_.Reset();
}

const RUNTIME_TASK_BASE::TaskPtr & TaskStateMachine::GetTask() const
{
	// TODO: insert return statement here
	return task_ptr_;
}

ATaskPool * TaskStateMachine::GetTaskPool()
{
	return task_pool_;
}

void TaskStateMachine::SetNextStateFunction(const FString & state_name)
{
	for (auto& state : finite_state_list_)
	{
		if (state->state_name_.Equals(state_name))
		{
			input_next_state_func_.Reset();
			input_next_state_func_ = TaskFunction_Factory::ProduceTaskFunction(state_name,state);
			FSM_SymbolPtr symbol = MakeShareable(new FSM_Symbol);
			symbol->symbol_ = state_name;
			input_next_state_func_->SetupNextSymbol(symbol);
			break;
		}
	}
}

