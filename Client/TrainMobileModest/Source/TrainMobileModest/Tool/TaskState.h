// As Part of GuangZhou Training.

#pragma once

/**
 * 
 */
#include "TaskStateMachine.h"

//state transfering.
//actions are functions,which explain what to do in the input/output symbol descriptions
//sleep(in the repository)
//->awake(task is being pushed in the progress)
//->active(listen from all actions)
//[->suspend(task may suspended by actions)]
//->finish(trigger setup next task-fsm-action)

class ATaskPool;
class TRAINMOBILEMODEST_API TaskState
	:public FSM_State
{
public:
	TaskState(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		: task_ptr_(tp), task_item_ptr_(tip),task_pool_(task_pool) {}
	virtual ~TaskState() {}
	const RUNTIME_TASK_BASE::TaskPtr& GetTask() const;
	const STATIC_TASK_BASE::TaskItemPtr& GetTaskItem() const;
	ATaskPool* GetTaskPool();
protected:
	RUNTIME_TASK_BASE::TaskPtr task_ptr_;
	STATIC_TASK_BASE::TaskItemPtr task_item_ptr_;//task state should connect to the task item.
	ATaskPool* task_pool_;

};

class TRAINMOBILEMODEST_API TaskState_Sleep
	:public TaskState
{
public:
	TaskState_Sleep(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		:TaskState(tp,tip,task_pool)
	{
		state_name_ = TEXT("state_sleep");
	}
	~TaskState_Sleep() {}
};


class TRAINMOBILEMODEST_API TaskState_Awake
	:public TaskState
{
public:
	TaskState_Awake(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		:TaskState(tp,tip,task_pool)
	{
		state_name_ = TEXT("state_awake");
	}
	~TaskState_Awake() {}
};

class TRAINMOBILEMODEST_API TaskState_Active
	:public TaskState
{
public:
	TaskState_Active(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		:TaskState(tp,tip,task_pool)
	{
		state_name_ = TEXT("state_active");
	}
	~TaskState_Active() {}
};

class TRAINMOBILEMODEST_API TaskState_Suspend
	:public TaskState
{
public:
	TaskState_Suspend(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		:TaskState(tp,tip,task_pool)
	{
		state_name_ = TEXT("state_suspend");
	}
	~TaskState_Suspend() {}
};

class TRAINMOBILEMODEST_API TaskState_Finish
	:public TaskState
{
public:
	TaskState_Finish(const RUNTIME_TASK_BASE::TaskPtr& tp, const STATIC_TASK_BASE::TaskItemPtr& tip, ATaskPool* task_pool)
		:TaskState(tp,tip,task_pool)
	{
		state_name_ = TEXT("state_finish");
	}
	~TaskState_Finish() {}
};