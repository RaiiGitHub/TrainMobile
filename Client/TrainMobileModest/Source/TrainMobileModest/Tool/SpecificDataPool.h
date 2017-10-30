// As Part of GuangZhou Training.

#pragma once

/**
 * 
 */
 //a class that specifically produced different components of fsm.
#include "TaskStateMachine.h"
typedef void(*SpecificFunc)(TaskStateMachine* fsm, const STATIC_TASK_BASE::TaskItemPtr& tip);
typedef TMap<FString, SpecificFunc> SpdMap;
class TRAINMOBILEMODEST_API SpecificDataPool
{
public:
	SpecificDataPool();
	~SpecificDataPool() {}
public:
	static const SpdMap& Get();
private:
	SpdMap specific_func_pool_;
public:
	static void normal_setup(TaskStateMachine* fsm, const STATIC_TASK_BASE::TaskItemPtr& tip);
};
