// As Part of GuangZhou Training.

#pragma once

/**
 * 
 */

#include "TaskStateMachine.h"

//function factory
class TRAINMOBILEMODEST_API TaskFunction_Factory
{
public:
	static FSM_FunctionPtr ProduceTaskFunction(const FString & state_name, const FSM_StatePtr & own_state);
};

//notify and set sleep task to the task-proxy-center.
class TRAINMOBILEMODEST_API TaskFunction_SetSleepTask
	:public FSM_Function
{
public:
	TaskFunction_SetSleepTask(const FSM_StatePtr& own_state) :FSM_Function(own_state) {}
	~TaskFunction_SetSleepTask() {}
};


//notify and set awake task to the task-proxy-center.
class TRAINMOBILEMODEST_API TaskFunction_SetAwakeTask
	:public FSM_Function
{
public:
	TaskFunction_SetAwakeTask(const FSM_StatePtr& own_state):FSM_Function(own_state) {}
	~TaskFunction_SetAwakeTask() {}
public:
	bool Explain();
};


//notify and set active task to the task-proxy-center.
class TRAINMOBILEMODEST_API TaskFunction_SetActiveTask
	:public FSM_Function
{
public:
	TaskFunction_SetActiveTask(const FSM_StatePtr& own_state) :FSM_Function(own_state) {}
	~TaskFunction_SetActiveTask() {}
public:
	bool Explain();
};

//notify and set suspend task to the task-proxy-center.
class TRAINMOBILEMODEST_API TaskFunction_SetSuspendTask
	:public FSM_Function
{
public:
	TaskFunction_SetSuspendTask(const FSM_StatePtr& own_state) :FSM_Function(own_state) {}
	~TaskFunction_SetSuspendTask() {}
};

//notify and set Finish task to the task-proxy-center.
class TRAINMOBILEMODEST_API TaskFunction_SetFinishTask
	:public FSM_Function
{
public:
	TaskFunction_SetFinishTask(const FSM_StatePtr& own_state) :FSM_Function(own_state) {}
	~TaskFunction_SetFinishTask() {}
public:
	bool Explain();
};

