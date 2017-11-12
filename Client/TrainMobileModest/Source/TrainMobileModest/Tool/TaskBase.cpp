// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TaskBase.h"
#include "TaskStateMachine.h"
#include "SpecificDataPool.h"


void RUNTIME_TASK_BASE::Task::ProduceFSMComponent(TaskStateMachine * fsm, const STATIC_TASK_BASE::TaskItemPtr& ti)
{
	if (nullptr == fsm || 0 == task_item_list_.Num())
		return;
	//normaly begin from the first taskitem
	//normal setup
	fsm->Reset(false);
	SpecificDataPool::Get()[TEXT("normal")](fsm, ti);//setup it!
}

void RUNTIME_TASK_BASE::Task::AddTaskItem(const STATIC_TASK_BASE::TaskItemPtr& ti)
{
	task_item_list_.AddTail(ti);
}

void RUNTIME_TASK_BASE::Task::BindRole(Role * role)
{
	bind_role_.Reset();
	bind_role_ = TSharedPtr<Role>(role);
}

void RUNTIME_TASK_BASE::Task::BindTaskScript(TaskScript * task_script)
{
	bind_script_.Reset();
	bind_script_ = TSharedPtr<TaskScript>(task_script);
}

bool RUNTIME_TASK_BASE::Role::CollectTasks(const TaskList& task_list, TaskList& task_list_of_role)
{
	bool atleast1 = false;
	for (auto& task : task_list)
	{
		if (task->bind_role_.Get() == this)
		{
			task_list_of_role.AddTail(task);
			atleast1 = true;
		}
	}
	return atleast1;
}

void TaskReader::Reset()
{
	for (auto& task : task_list_)
	{
		task->state = EBaseState::BState_INACTIVED;
		for (auto& task_item : task->task_item_list_)
		{
			task_item->state = EBaseState::BState_INACTIVED;
			for (auto& task_item_act : task_item->action_list_)
			{
				task_item_act->state = EBaseState::BState_INACTIVED;
				task_item_act->action_operation_start_time_ = 0.f;
				task_item_act->action_operation_end_time_ = 0.f;
			}
		}
	}
}

int TaskReader::GetTaskListByScriptName(const FString & script_name, RUNTIME_TASK_BASE::TaskList& list_to_fill) const
{
	// TODO: insert return statement here
	int pre_num = list_to_fill.Num();
	for (auto& task : task_list_)
	{
		if (task->bind_script_.Get() 
			&& task->bind_script_->task_script_name_.Equals(script_name,ESearchCase::IgnoreCase))
			list_to_fill.AddTail(task);
	}
	return list_to_fill.Num() - pre_num;
}

RUNTIME_TASK_BASE::TaskScriptPtr TaskReader::GetTaskScriptByName(const FString & script_name) const
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->task_script_name_.Equals(script_name, ESearchCase::IgnoreCase))
		{
			return script;
		}
	}
	return RUNTIME_TASK_BASE::TaskScriptPtr();
}

RUNTIME_TASK_BASE::TaskScriptPtr TaskReader::GetTaskScriptByIndex(const int index) const
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->task_script_index_ == index )
		{
			return script;
		}
	}
	return RUNTIME_TASK_BASE::TaskScriptPtr();
}

RUNTIME_TASK_BASE::RolePtr TaskReader::GetTaskRoleByName(const FString & script_name, const FString & role_name) const
{
	for (auto& task : task_list_)
	{
		if (task->bind_script_.Get()
			&& task->bind_script_->task_script_name_.Equals(script_name, ESearchCase::IgnoreCase))
		{
			if (task->bind_role_.Get()
				&& task->bind_role_->role_name_.Equals(role_name, ESearchCase::IgnoreCase))
			{
				return task->bind_role_;
			}
		}
	}
	return RUNTIME_TASK_BASE::RolePtr();
}
