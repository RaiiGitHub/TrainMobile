// As Part of GuangZhou Training.

#include "SpecificDataPool.h"
#include "TrainMobileModest.h"
#include "TaskPool.h"
#include "TaskState.h"
#include "TaskFunction.h"

void SpecificDataPool::normal_setup(TaskStateMachine* fsm, const STATIC_TASK_BASE::TaskItemPtr& tip)
{
	if (nullptr == fsm || nullptr == tip.Get() )
		return;
	FSM_StatePtr sleep_state = MakeShareable(new TaskState_Sleep(fsm->GetTask(), tip,fsm->GetTaskPool()));//sleep
	FSM_StatePtr awake_state = MakeShareable(new TaskState_Awake(fsm->GetTask(), tip,fsm->GetTaskPool()));//awake
	FSM_StatePtr active_state = MakeShareable(new TaskState_Active(fsm->GetTask(), tip,fsm->GetTaskPool()));//active
	FSM_StatePtr finish_state = MakeShareable(new TaskState_Finish(fsm->GetTask(), tip,fsm->GetTaskPool()));//finish
	FSM_FunctionPtr next_state_func_;
	fsm->finite_state_list_.AddTail(sleep_state);
	fsm->finite_state_list_.AddTail(awake_state);
	fsm->finite_state_list_.AddTail(active_state);
	fsm->finite_state_list_.AddTail(finish_state);
	//calculating current task-item state.
	switch (tip->state)
	{
	case EBaseState::BState_INACTIVED:
		fsm->begining_state_ = sleep_state;
		next_state_func_ = MakeShareable(new TaskFunction_SetAwakeTask(awake_state));
		break;
	case EBaseState::BState_AWAKED:
		fsm->begining_state_ = awake_state;
		next_state_func_ = MakeShareable(new TaskFunction_SetActiveTask(active_state));
		break;
	case EBaseState::BState_ACTIVED:
		fsm->begining_state_ = active_state;
		next_state_func_ = MakeShareable(new TaskFunction_SetFinishTask(finish_state));
		break;
	case EBaseState::BState_FINISHED:
		fsm->begining_state_ = finish_state;
		next_state_func_ = MakeShareable(new TaskFunction_SetSleepTask(sleep_state));
		break;
	}
	fsm->current_state_ = fsm->begining_state_;
	fsm->input_next_state_func_ = next_state_func_;
}
///////////
const static SpecificDataPool static_spd;
SpecificDataPool::SpecificDataPool()
{
	specific_func_pool_.Add(TEXT("normal"), normal_setup);
}

const SpdMap & SpecificDataPool::Get()
{
	// TODO: insert return statement here
	return static_spd.specific_func_pool_;
}
