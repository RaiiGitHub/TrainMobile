// As Part of GuangZhou Training.

#pragma once

/**
 * state machine of tasks. 
 */

#include "TaskBase.h"
#include "StateMachine.h"
//
class ATaskPool;
class TRAINMOBILEMODEST_API TaskStateMachine:
	public FSM_Object
{
public:
	TaskStateMachine();
	~TaskStateMachine();
public:
	void SetupFSM(const RUNTIME_TASK_BASE::TaskPtr& task_ptr, ATaskPool* task_pool);
	void Reset(bool bResetTask = true);
	const RUNTIME_TASK_BASE::TaskPtr& GetTask()	const;
	ATaskPool* GetTaskPool();
	void SetNextStateFunction(const FString& state_name);
protected:
	RUNTIME_TASK_BASE::TaskPtr task_ptr_;
	ATaskPool* task_pool_;
};
