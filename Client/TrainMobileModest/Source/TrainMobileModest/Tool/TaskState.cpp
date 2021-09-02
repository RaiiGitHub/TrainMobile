// As Part of GuangZhou Training.

#include "TaskState.h"
#include "TrainMobileModest.h"

const RUNTIME_TASK_BASE::TaskPtr & TaskState::GetTask() const
{
	// TODO: insert return statement here
	return task_ptr_;
}

const STATIC_TASK_BASE::TaskItemPtr & TaskState::GetTaskItem() const
{
	// TODO: insert return statement here
	return task_item_ptr_;
}

ATaskPool * TaskState::GetTaskPool()
{
	return task_pool_;
}
