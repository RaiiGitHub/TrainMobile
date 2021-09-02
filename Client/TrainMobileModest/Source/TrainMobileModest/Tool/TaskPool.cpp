// As Part of GuangZhou Training.

#include "TaskPool.h"
#include "TrainMobileModest.h"
#include "TaskState.h"
#include "TaskSequencer.h"
#include "ActorFinder.h"
#include "TrainGameInstance.h"
#include "TrainOnline.h"
#include "Engine/DataTable.h"

// Sets default values
ATaskPool::ATaskPool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATaskPool::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATaskPool::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}


void ATaskPool::ReadTask()
{
	task_list_.Empty();
	FDateTime now = FDateTime::Now();
	FString now_str= FString::Printf(TEXT("%d-%02d-%02d %02d:%02d:%02d")
			, now.GetYear()
			, now.GetMonth()
			, now.GetDay()
			, now.GetHour()
			, now.GetMinute()
			, now.GetSecond());
	FString script_path = TEXT("DataTable'/Game/TaskData/SriptTaskData.SriptTaskData'");
	UDataTable* scripts = LoadObject<UDataTable>(NULL, *script_path);
	if (nullptr != scripts)
	{
		static const FString ContextString(TEXT("ATaskPool::ReadTask::"));
		TArray<FScriptTaskData*> script_data;
		scripts->GetAllRows<FScriptTaskData>(ContextString+TEXT("script"), script_data);
		int script_index = 1;
		for (auto& script_unit : script_data)
		{
			RUNTIME_TASK_BASE::TaskScriptPtr ts = MakeShareable(new RUNTIME_TASK_BASE::TaskScript(script_index++));
			ts->script_memo_ = script_unit->Memo;
			ts->task_script_name_ = script_unit->ScriptName;
			ts->active_ = false;
			ts->script_type_name_ = script_unit->TypeName;
			ts->script_icon_path_ = script_unit->IconPath;
			ts->script_main_level_name_ = script_unit->MainLevelName;
			task_script_list_.AddTail(ts);

			TArray<FTaskData*> task_rows;
			TArray<FRoleGoodsTexture*> task_goods;
			TArray<FRoleTaskItemAction*> task_actions;
			TArray<FRoleFirstSight*> task_first_sights;
			for (auto& meta : script_unit->TaskDataMeta)
			{
				ts->meta_tables_.Add(meta.MetaType, meta.MetaName);
				UDataTable* data_table = LoadObject<UDataTable>(NULL, *meta.MetaName);
				if (nullptr != data_table)
				{
					switch (meta.MetaType)
					{
					case ETaskDataMetaType::TaskDataMeta_TaskData:
						data_table->GetAllRows<FTaskData>(ContextString + meta.MetaName, task_rows);
						break;
					case ETaskDataMetaType::TaskDataMeta_RoleGoodsTexture:
						data_table->GetAllRows<FRoleGoodsTexture>(ContextString + meta.MetaName, task_goods);
						break;
					case ETaskDataMetaType::TaskDataMeta_RoleTaskItemAction:
						data_table->GetAllRows<FRoleTaskItemAction>(ContextString + meta.MetaName, task_actions);
						break;
					case ETaskDataMetaType::TaskDataMeta_RoleFirstSight:
						data_table->GetAllRows<FRoleFirstSight>(ContextString + meta.MetaName, task_first_sights);
						break;
					}
				}
			}

			int task_index = 1;
			for (auto& task : task_rows)
			{
				RUNTIME_TASK_BASE::TaskPtr tp = MakeShareable(new RUNTIME_TASK_BASE::Task(task_index++));
				tp->task_name_ = task->Name;
				tp->task_welcome_tips_offline = task->WelcomTipsOffline;
				tp->task_welcome_tips_online = task->WelcomTipsOnline;
				tp->suggest_order_ = task->SuggestOrder;

				//read task_item_goods_tex
				FRoleGoodsTexture* _rgt = nullptr;
				for (auto& rgt : task_goods)
				{
					if (rgt->Role.Equals(task->Role, ESearchCase::IgnoreCase))
					{
						_rgt = rgt;
						break;
					}
				}
				FRoleTaskItemAction* _rta = nullptr;
				for (auto& rta : task_actions)
				{
					if (rta->Role.Equals(task->Role, ESearchCase::IgnoreCase))
					{
						_rta = rta;
						break;
					}
				}
				FRoleFirstSight* _rfs = nullptr;
				for (auto& rfs : task_first_sights)
				{
					if (rfs->Role.Equals(task->Role, ESearchCase::IgnoreCase))
					{
						_rfs = rfs;
						break;
					}
				}
				int ti_index = 1;
				for (auto& _content : task->Content)
				{
					STATIC_TASK_BASE::TaskItemPtr item = MakeShareable(new STATIC_TASK_BASE::TaskItem(ti_index++));
					item->task_content_ = _content;
					if (_rgt)
					{
						int ig_index = 1;
						for (auto& ig : _rgt->Texs)
						{
							if (ig.TaskItemIndex == item->item_index_
								&& ig.TaskNodeIndex == tp->task_index_)
							{
								STATIC_TASK_BASE::TaskItemGameToolPtr itemTool = MakeShareable(new STATIC_TASK_BASE::TaskItemGameTool(ig_index++));
								itemTool->tool_desc_ = ig.TexDesc;
								itemTool->tool_name_ = ig.TexName;
								itemTool->tool_tex_ = ig.TexPath;
								itemTool->bind_act_index = ig.TaskItemActionIndex;
								item->game_tool_list_.AddTail(itemTool);
							}
						}
					}
					
					if (_rta)
					{
						int ia_index = 1;
						for (auto& ia : _rta->Actions)
						{
							if (ia.TaskItemIndex == item->item_index_
								&& ia.TaskNodeIndex == tp->task_index_)
							{
								STATIC_TASK_BASE::TaskItemActionPtr itemAction = MakeShareable(new STATIC_TASK_BASE::TaskItemAction(ia_index++));
								itemAction->action_desc_ = ia.ActDesc;
								itemAction->action_name_ = ia.ActName;
								itemAction->state = (EBaseState)ia.ActState;
								itemAction->action_operation_result_ = ia.ActResult;
								itemAction->relate_to_location_ = ia.ActRelateToLocation;
								itemAction->action_location_ = ia.ActLocation;
								itemAction->relate_to_sprite_location_ = ia.ActRelateToSpriteLocation;
								itemAction->action_sprite_location_ = ia.ActSpriteLocation;
								itemAction->action_sprite_scale_ = ia.ActSpriteScale;
								itemAction->action_exp_benchmark_ = ia.ActExpBenchmark;
								itemAction->action_suggest_order_ = ia.ActSuggestOrder;
								itemAction->action_operation_date_time_ = now_str;
								item->action_list_.AddTail(itemAction);
							}
						}
					}
					//
					tp->task_item_list_.AddTail(item);
				}
					
				RUNTIME_TASK_BASE::RolePtr bind_role = GetTaskRoleByName(script_unit->ScriptName, task->Role);

				if (nullptr == bind_role.Get())
				{
					bind_role = MakeShareable(new RUNTIME_TASK_BASE::Role(tp->task_index_));
					bind_role->role_name_ = task->Role;
					if (_rfs)
					{
						bind_role->first_sight_aspect_ration_ = _rfs->aspectRation;
						bind_role->first_sight_enable_ = _rfs->bEnable;
						bind_role->first_sight_fov_ = _rfs->fov;
						bind_role->first_sight_rotation = _rfs->sightRotation;
					}
				}
				tp->bind_role_ = bind_role;
				tp->bind_script_ = ts;
				task_list_.AddTail(tp);
			}
		}
	}
}

bool ATaskPool::PushToRolePool(const FString & role_name)
{
	RUNTIME_TASK_BASE::RolePtr rp = GetRoleByRoleName(role_name);
	if (rp.Get())
	{
		if (current_role_list_.Contains(rp))
			return false;//contained already.
		return current_role_list_.AddTail(rp);
	}
	return false;
}

void ATaskPool::EmptyRolePool()
{
	current_role_list_.Empty();
}

bool ATaskPool::ActiveRole(const RUNTIME_TASK_BASE::RolePtr & rp)
{
	if (nullptr == rp.Get())
		return false;
	current_active_task_list_.Empty();
	if (rp->CollectTasks(task_list_, current_active_task_list_))
	{
		//first one that is not been completed.
		for (auto& task : current_active_task_list_)
		{
			if (task.Get())
			{
				if (EBaseState::BState_FINISHED != task->state)
				{
					SetCurrentTask(task);
					return true;
				}

			}
		}
	}
	return false;
}

RUNTIME_TASK_BASE::TaskPtr ATaskPool::GetSpecificTask(int task_index)
{
	for (auto& task : task_list_)
	{
		if (task.Get() 
			&& task->bind_script_.Get() 
			&& task->bind_script_->active_
			&& task_index == task->task_index_)
			return task;
	}
	return RUNTIME_TASK_BASE::TaskPtr();
}

STATIC_TASK_BASE::TaskItemPtr ATaskPool::GetSpecificTaskItem(int task_index, int item_index)
{
	for (auto& task : task_list_)
	{
		if (task.Get() 
			&& task->bind_script_.Get() && task->bind_script_->active_
			&& task_index == task->task_index_ )
		{
			for (auto& item : task->task_item_list_)
			{
				if (item.Get() && item_index == item->item_index_)
					return item;
			}
			break;
		}
	}
	return STATIC_TASK_BASE::TaskItemPtr();
}

STATIC_TASK_BASE::TaskItemActionPtr ATaskPool::GetSpecificTaskItemAction(int task_index, int item_index, int act_index)
{
	for (auto& task : task_list_)
	{
		if (task.Get()
			&& task->bind_script_.Get() && task->bind_script_->active_
			&& task_index == task->task_index_)
		{
			for (auto& item : task->task_item_list_)
			{
				if (item.Get() && item_index == item->item_index_)
				{
					for (auto& act : item->action_list_)
					{
						if (act.Get() && act_index == act->action_index_)
							return act;
					}
					break;
				}
			}
			break;
		}
	}
	return STATIC_TASK_BASE::TaskItemActionPtr();
}

void ATaskPool::SetCurrentTask(const RUNTIME_TASK_BASE::TaskPtr & task_ptr)
{
	current_task_ptr_.Reset();
	current_task_ptr_ = task_ptr;
	task_fsm_.SetupFSM(task_ptr,this);
}

RUNTIME_TASK_BASE::TaskPtr ATaskPool::GetCurrentTask()
{
	// TODO: insert return statement here
	return current_task_ptr_;
}

RUNTIME_TASK_BASE::TaskList & ATaskPool::GetCurrentActiveTaskList()
{
	// TODO: insert return statement here
	return current_active_task_list_;
}

RUNTIME_TASK_BASE::RolePtr  ATaskPool::GetRoleByRoleName(const FString & name)
{
	// TODO: insert return statement here
	for (auto& task : task_list_)
	{
		if (task->bind_role_.Get() 
			&& task->bind_script_.Get() && task->bind_script_->active_)
		{
			if (task->bind_role_->role_name_.Equals(name, ESearchCase::IgnoreCase))
			{
				if ( current_task_script_ptr_.Get() )
				{
					if (current_task_script_ptr_ == task->bind_script_)
						return task->bind_role_;
				}
				else
					return task->bind_role_;
			}
				
		}
	}
	return RUNTIME_TASK_BASE::RolePtr();
}

FTaskData ATaskPool::ReadDataTableRow(const FString & table_name, const int& row_index)
{
	if (GetClass()->ImplementsInterface(UTaskPoolInterface::StaticClass()))
		return Execute_GetTaskDataTableAssetAtRow(this, table_name, row_index);
	return FTaskData();
}

void ATaskPool::PullDataTable()
{
	//call by blueprint.
	ReadTask();
}

FTaskData ATaskPool::GetTaskRowAt(const FString& script_name, int index)
{
	RUNTIME_TASK_BASE::TaskList task_script_list;
	int num = GetTaskListByScriptName(script_name, task_script_list);
	static FTaskData fd;
	fd.Action.Empty();
	fd.Content.Empty();
	fd.Memo.Empty();
	fd.Name.Empty();
	fd.Position.Empty();
	fd.Role.Empty();

	if (index >= 0 && index < num)
	{
		int i = 0;
		for (auto& a : task_script_list )
		{
			if (i == index)
			{
				fd.Name = a->task_name_;
				fd.Role = a->bind_role_->role_name_;
				fd.WelcomTipsOffline = a->task_welcome_tips_offline;
				fd.WelcomTipsOnline = a->task_welcome_tips_online;
				for (auto& item : a->task_item_list_)
					fd.Content.Add(item->task_content_.Replace(TEXT("$"),TEXT("")));
				break;
			}
			++i;
		}
	}
	return fd;
}

int ATaskPool::GetTaskRowCount(const FString & script_name)
{
	RUNTIME_TASK_BASE::TaskList task_script_list;
	return GetTaskListByScriptName(script_name, task_script_list);
}

bool ATaskPool::IsExistExecutingTask()
{
	return nullptr != GetCurrentTask().Get();
}

bool ATaskPool::EnterTaskScript(const FString & script_name)
{
	current_task_script_ptr_.Reset();
	current_task_script_ptr_ = TaskReader::GetTaskScriptByName(script_name);
	return nullptr != current_task_script_ptr_.Get();
}

bool ATaskPool::EnterTaskPoolAsClaimRole(const FString & role_name)
{
	RUNTIME_TASK_BASE::RolePtr role = GetRoleByRoleName(role_name);
	if (nullptr == role.Get())
		return false;
	return ActiveRole(role);
}

bool ATaskPool::AddRoleToPool(const FString & role_name)
{
	return PushToRolePool(role_name);
}

void ATaskPool::EmptyRoleFromPool()
{
	task_fsm_.Clear();
	current_task_ptr_.Reset();
	current_active_task_list_.Empty();
	current_role_list_.Empty();
}

void ATaskPool::Reset()
{
	ResetActions();
	EmptyRoleFromPool();
}

void ATaskPool::ResetActions()
{
	TaskReader::Reset();
}

bool ATaskPool::SwitchRoleInRolePool(const FString & role_name)
{
	for (auto& rp : current_role_list_)
	{
		if (rp.Get())
		{
			if(rp->role_name_.Equals(role_name))
				return ActiveRole(rp);
		}
	}
	return false;
}

int ATaskPool::GetCurrentTaskIndex()
{
	if (current_task_ptr_.Get())
		return current_task_ptr_->task_index_;
	return 0;
}

int ATaskPool::GetCurrentTaskItemIndex()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
			return ts->GetTaskItem()->item_index_;
	}
	return 0;
}

int ATaskPool::GetCurrentTaskItemActionIndex()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (BState_ACTIVED == act->state)
					return act->action_index_;
			}
		}
	}
	return 0;
}

int ATaskPool::GetCurrentTaskItemAwakeActionIndex()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (BState_AWAKED == act->state)
					return act->action_index_;//should be return when met the first one.
			}
		}
	}
	return 0;
}

int ATaskPool::GetCurrentTaskItemActionFinishedIndex()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			int max_finished = 0;
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (BState_FINISHED == act->state)
					max_finished = FMath::Max(max_finished, act->action_index_);
			}
			return max_finished;
		}
	}
	return 0;
}

int ATaskPool::GetTaskItemActionIndexInPage(int pageSize, int indexInPage)
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			int action_index = 0;
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (BState_AWAKED == act->state)
				{
					action_index = act->action_index_;
					break;
				}
			}
			if (0 == action_index)
			{
				//not any awaked actions,refered to the most recent finished.
				//most recent actived should be took consideration first.
				for (auto& act : ts->GetTaskItem()->action_list_)
				{
					if (BState_FINISHED == act->state || BState_ACTIVED == act->state)
						action_index = FMath::Max(action_index, act->action_index_);
				}
			}
			if (0 != action_index)
			{
				int kz = action_index % pageSize;
				kz = 0 == kz ? pageSize : kz;
				return action_index + indexInPage - kz;
			}
		}
	}
	return 0;
}

bool ATaskPool::IsCurrentTaskItemActionsFinished()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (BState_FINISHED != act->state)
					return false;
			}
		}
	}
	return true;
}

bool ATaskPool::IsCurrentTaskItemsFinished()
{
	if (nullptr == current_task_ptr_.Get())
		return false;
	for (auto& item : current_task_ptr_->task_item_list_)
	{
		if (EBaseState::BState_FINISHED != item->state)
			return false;
	}
	return true;
}

TArray<FString>& ATaskPool::GetCurrentTaskItemGoodsTex()
{
	// TODO: insert return statement here
	static TArray<FString> texs;
	texs.Empty();
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			texs.Empty();
			for (auto& tex : ts->GetTaskItem()->game_tool_list_)
				texs.Add(tex->tool_tex_);
			return texs;
		}
	}
	return texs;
}

FTaskItemTex& ATaskPool::GetCurrentTaskItemGoods(int goodsIndex)
{
	static FTaskItemTex tit;
	tit.TaskItemIndex = 0;
	tit.TaskNodeIndex = 0;
	tit.TaskItemActionIndex = 0;
	tit.TexName.Empty();
	tit.TexDesc.Empty();
	tit.TexPath.Empty();
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			for (auto& tex : ts->GetTaskItem()->game_tool_list_)
			{
				if (tex->tool_index_ == goodsIndex)
				{
					tit.TaskItemIndex = ts->GetTaskItem()->item_index_;
					tit.TaskNodeIndex = ts->GetTask()->task_index_;
					tit.TaskItemActionIndex = tex->bind_act_index;
					tit.TexName = tex->tool_name_;
					tit.TexDesc = tex->tool_desc_;
					tit.TexPath = tex->tool_tex_;
					break;
				}
			}
		}
	}
	return tit;
}

FTaskItemAction & ATaskPool::GetCurrentTaskItemAction(int actIndex)
{
	// TODO: insert return statement here
	//warning: the static value 'action',
	//may probably been called in different threads, 
	//please CONSTRUCT another value while you get the return result. 
	static FTaskItemAction action;
	action.TaskItemIndex = 0;
	action.TaskNodeIndex = 0;
	action.ActDesc.Empty();
	action.ActName.Empty();
	action.ActResult.Empty();
	action.ActRelateToLocation = false;
	action.ActLocation = FVector::ZeroVector;
	action.ActRelateToSpriteLocation = false;
	action.ActSpriteLocation = FVector::ZeroVector;
	action.ActSpriteScale = FVector(1.f, 1.f, 1.f);
	action.ActState = EBaseState::BState_INACTIVED;
	action.ActExpBenchmark = 0;
	action.ActExpActualDuration = 0;
	action.ActSuggestOrder = 0;
	
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
		{
			for (auto& act : ts->GetTaskItem()->action_list_)
			{
				if (act->action_index_ == actIndex)
				{
					action.TaskItemIndex = ts->GetTaskItem()->item_index_;
					action.TaskNodeIndex = ts->GetTask()->task_index_;
					action.ActName = act->action_name_;
					action.ActDesc = act->action_desc_;
					action.ActResult = act->action_operation_result_;
					action.ActState = act->state;
					action.ActRelateToLocation = act->relate_to_location_;
					action.ActLocation = act->action_location_;
					action.ActRelateToSpriteLocation = act->relate_to_sprite_location_;
					action.ActSpriteLocation = act->action_sprite_location_;
					action.ActSpriteScale = act->action_sprite_scale_;
					action.ActExpBenchmark = act->action_exp_benchmark_;
					action.ActSuggestOrder = act->action_suggest_order_;
					action.ActExpActualDuration = FPlatformMath::FloorToInt(act->action_operation_end_time_ - act->action_operation_start_time_);
					break;
				}
			}
		}
	}
	return action;
}

FRoleFirstSight & ATaskPool::GetCurrentRoleFirstSight()
{
	// TODO: insert return statement here
	static FRoleFirstSight rfs;
	rfs.aspectRation = 1.778;
	rfs.bEnable = false;
	rfs.fov = 90.f;
	rfs.Role.Empty();
	rfs.sightRotation = FRotator::ZeroRotator;
	RUNTIME_TASK_BASE::RolePtr rp = GetRoleByRoleName(GetCurrentRoleName());
	if (rp.Get())
	{
		rfs.Role = rp->role_name_;
		rfs.aspectRation = rp->first_sight_aspect_ration_;
		rfs.bEnable = rp->first_sight_enable_;
		rfs.fov = rp->first_sight_fov_;
		rfs.sightRotation = rp->first_sight_rotation;
	}
	return rfs;
}

int ATaskPool::GetCurrentTaskItemActionsCount()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
			return ts->GetTaskItem()->action_list_.Num();
	}
	return 0;
}

int ATaskPool::GetCurrentRoleCount()
{
	return current_role_list_.Num();
}

int ATaskPool::GetRoleIndex(int index_of_role)
{
	int role_count = current_role_list_.Num();
	if (index_of_role >= 0 && index_of_role < role_count)
	{
		int _index = 0;
		for (auto& rp : current_role_list_)
		{
			if (_index == index_of_role)
				return rp->role_index_;
			++_index;
		}
	}
	return -1;
}

int ATaskPool::GetIndexOfRole(int role_index)
{
	int role_count = current_role_list_.Num();
	int _index = 0;
	for (auto& rp : current_role_list_)
	{
		if (role_index == rp->role_index_)
			return _index;
		++_index;
	}
	return -1;
}

int ATaskPool::GetIndexOfRoleByRoleName(const FString& role_name)
{
	int role_count = current_role_list_.Num();
	int _index = 0;
	for (auto& rp : current_role_list_)
	{
		if (rp->role_name_ == role_name)
			return _index;
		++_index;
	}
	return -1;
}

FString ATaskPool::GetRoleName(int index_of_role)
{
	int role_count = current_role_list_.Num();
	if (index_of_role >= 0 && index_of_role < role_count)
	{
		int _index = 0;
		for (auto& rp : current_role_list_)
		{
			if (_index == index_of_role)
				return rp->role_name_;
			++_index;
		}
	}
	return FString();
}

FString ATaskPool::GetCurrentRoleName()
{
	if (current_task_ptr_.Get())
		return current_task_ptr_->bind_role_->role_name_;
	return FString();
}

FString ATaskPool::GetCurrentTaskScriptName()
{

	return FString();
}

FString ATaskPool::GetCurrentTaskItemContent()
{
	if (task_fsm_.current_state_.Get())
	{
		TaskState* ts = static_cast<TaskState*>(task_fsm_.current_state_.Get());
		if (ts->GetTaskItem().Get())
			return ts->GetTaskItem()->task_content_.Replace(TEXT("$"),TEXT("\n"));
	}
	return FString();
}

bool ATaskPool::GetTaskItemContentByRoleName(const FString & role_name, FString & task_node, TArray<FString>& task_item) const
{
	task_node.Empty();
	task_item.Empty();
	for (auto& task : task_list_)
	{
		if (task.Get() 
			&& task->bind_script_.Get() && task->bind_script_->active_)
		{
			if (task->bind_role_.Get() && task->bind_role_->role_name_.Equals(role_name) )
			{
				task_node = task->task_name_;
				for (auto& item : task->task_item_list_)
					task_item.Add(item->task_content_.Replace(TEXT("$"),TEXT("")));
				return true;
			}
		}
	}
	return false;
}

bool ATaskPool::GetTaskItemActionByRoleName(const FString & role_name, TArray<FTaskItemAction>& actions) const
{
	actions.Empty();
	for (auto& task : task_list_)
	{
		if (task.Get() 
			&& task->bind_script_.Get() && task->bind_script_->active_)
		{
			if (task->bind_role_.Get() && task->bind_role_->role_name_.Equals(role_name))
			{
				for (auto& item : task->task_item_list_)
				{
					if (item.Get())
					{
						if (item.Get())
						{
							for (auto& act : item->action_list_)
							{
								if (act.Get())
								{
									FTaskItemAction tia;
									tia.ActionIndex = act->action_index_;
									tia.TaskItemIndex = item->item_index_;
									tia.TaskNodeIndex = task->task_index_;
									tia.ActName = act->action_name_;
									tia.ActDesc = act->action_desc_;
									tia.ActResult = act->action_operation_result_;
									tia.ActState = act->state;
									tia.ActRelateToLocation = act->relate_to_location_;
									tia.ActLocation = act->action_location_;
									tia.ActRelateToSpriteLocation = act->relate_to_sprite_location_;
									tia.ActSpriteLocation = act->action_sprite_location_;
									tia.ActSpriteScale = act->action_sprite_scale_;
									tia.ActExpBenchmark = act->action_exp_benchmark_;
									tia.ActSuggestOrder = act->action_suggest_order_;
									tia.ActExpActualDuration = FPlatformMath::FloorToInt(act->action_operation_end_time_ - act->action_operation_start_time_);
									actions.Add(tia);
								}
							}
						}
					}
				}
				return true;
			}
		}
	}
	return false;
}

TArray<FRoleImageData> ATaskPool::GetRoleImageDataByRoleName(const FString & role_name) const
{
	TArray<FRoleImageData> _rid;
	FScriptTaskData _std = GetTaskScriptByName(GetActiveScriptName());
	for (auto& m : _std.TaskDataMeta)
	{
		if (m.MetaType == ETaskDataMetaType::TaskDataMeta_RoleImageData)
		{
			static const FString ContextString(TEXT("ATaskPool::ReadTask::"));
			TArray<FRoleImageData*> img_data;
			UDataTable* data_table = LoadObject<UDataTable>(NULL, *m.MetaName);
			if (data_table)
			{
				data_table->GetAllRows<FRoleImageData>(ContextString + m.MetaName, img_data);
				for (auto& i : img_data)
				{
					if (i->Role == role_name)
					{
						_rid.Add(*i);
					}
				}
			}
		}
	}
	return _rid;
}

TArray<FRoleImageData> ATaskPool::GetRoleImageData() const
{
	TArray<FRoleImageData> _rid;
	FScriptTaskData _std = GetTaskScriptByName(GetActiveScriptName());
	for (auto& m : _std.TaskDataMeta)
	{
		if (m.MetaType == ETaskDataMetaType::TaskDataMeta_RoleImageData)
		{
			static const FString ContextString(TEXT("ATaskPool::ReadTask::"));
			TArray<FRoleImageData*> img_data;
			UDataTable* data_table = LoadObject<UDataTable>(NULL, *m.MetaName);
			if (data_table)
			{
				data_table->GetAllRows<FRoleImageData>(ContextString + m.MetaName, img_data);
				for (auto& i : img_data)
					_rid.Add(*i);
			}
		}
	}
	return _rid;
}

TArray<FRoleMovieData> ATaskPool::GetRoleMovieDataByRoleName(const FString& role_name) const
{
	TArray<FRoleMovieData> _rmd;
	FScriptTaskData _std = GetTaskScriptByName(GetActiveScriptName());
	for (auto& m : _std.TaskDataMeta)
	{
		if (m.MetaType == ETaskDataMetaType::TaskDataMeta_RoleMovieData)
		{
			static const FString ContextString(TEXT("ATaskPool::ReadTask::"));
			TArray<FRoleMovieData*> movie_data;
			UDataTable* data_table = LoadObject<UDataTable>(NULL, *m.MetaName);
			if (data_table)
			{
				data_table->GetAllRows<FRoleMovieData>(ContextString + m.MetaName, movie_data);
				for (auto& i : movie_data)
				{
					if (i->Role == role_name)
					{
						_rmd.Add(*i);
					}
				}
			}
		}
	}
	return _rmd;
}

TArray<FRoleMovieData> ATaskPool::GetRoleMovieData() const
{
	TArray<FRoleMovieData> _rmd;
	FScriptTaskData _std = GetTaskScriptByName(GetActiveScriptName());
	for (auto& m : _std.TaskDataMeta)
	{
		if (m.MetaType == ETaskDataMetaType::TaskDataMeta_RoleMovieData)
		{
			static const FString ContextString(TEXT("ATaskPool::ReadTask::"));
			TArray<FRoleMovieData*> movie_data;
			UDataTable* data_table = LoadObject<UDataTable>(NULL, *m.MetaName);
			if (data_table)
			{
				data_table->GetAllRows<FRoleMovieData>(ContextString + m.MetaName, movie_data);
				for (auto& i : movie_data)
					_rmd.Add(*i);
			}
		}
	}
	return _rmd;
}

int ATaskPool::GetCurrentRoleIndex()
{
	if (current_task_ptr_.Get())
		return current_task_ptr_->bind_role_->role_index_;
	return 0;
}

int ATaskPool::GetCurrentRoleIndexOfRole()
{
	if (current_task_ptr_.Get())
	{
		int index = 0;
		for (auto& role : current_role_list_)
		{
			if (role == current_task_ptr_->bind_role_)
				return index;
			++index;
		}
	}
	return -1;
}

bool ATaskPool::ExecuteFsmNextFunction(const FString& next_symbol)
{
	if (task_fsm_.input_next_state_func_.Get())
	{
		FSM_SymbolPtr symbol = MakeShareable(new FSM_Symbol);
		symbol->symbol_ = next_symbol;
		task_fsm_.input_next_state_func_->SetupNextSymbol(symbol);
		task_fsm_.input_next_state_func_->Explain();
		return true;
	}
	return false;
}

bool ATaskPool::ExecuteFsmOutputFunction(const FString& next_symbol)
{
	if (task_fsm_.output_func_.Get())
	{
		FSM_SymbolPtr symbol = MakeShareable(new FSM_Symbol);
		symbol->symbol_ = next_symbol;
		task_fsm_.output_func_->SetupNextSymbol(symbol);
		task_fsm_.output_func_->Explain();
		return true;
	}
	return false;
}

bool ATaskPool::RestoreFsmCurrentFunction()
{
	if ( GetFsmCurrentFunctionStateName().Equals(TEXT("state_sleep")))
		return ExecuteFsmNextFunction(TEXT("state_active"));//at the very begining call.
	else
	{
		//notify restoring fsm function
		if (GetClass()->ImplementsInterface(UTaskPoolInterface::StaticClass()))
			Execute_NotifyRestorFsmFunction(this);
		return true;
	}
	return false;
}

FString ATaskPool::GetFsmCurrentFunctionStateName()
{
	if (task_fsm_.current_state_.Get())
	{
		return task_fsm_.current_state_->state_name_;
	}
	return FString();
}

FString ATaskPool::GetFsmNextFunctionStateName()
{
	if (task_fsm_.input_next_state_func_.Get()
		&& task_fsm_.input_next_state_func_->next_symbol_.Get())
		return task_fsm_.input_next_state_func_->next_symbol_->symbol_;
	return FString();
}

ATaskSequencer * ATaskPool::GetTaskSequencer()
{
	ATaskSequencer* ts = FindObject<ATaskSequencer>(ANY_PACKAGE, TEXT("TaskSequencerBp"));
	if (nullptr == ts)
		return nullptr;
	return ts;
}

FTaskItemActionStatistics ATaskPool::GetStatisticsItemActionsByRoleName(const FString & role_name)
{
	FTaskItemActionStatistics tias;
	tias.RoleName = role_name;
	tias.CurFinishedActionCount = 0;
	tias.CurPeriodExp = 0;
	tias.CurPeriodPlayTime = 0.f;
	tias.CurTotalActionCount = 0;
	tias.CurTaskItemExp = 0;
	tias.CurTaskItemPlayTime = 0;

	//first one that is not been completed.
	for (auto& task : current_active_task_list_)
	{
		if (task.Get() 
			&& task->bind_script_.Get() && task->bind_script_->active_)
		{
			if (task->bind_role_.Get())
			{
				if (0 == task->bind_role_->role_name_.Compare(role_name, ESearchCase::IgnoreCase))
				{
					for (auto& item : task->task_item_list_)
					{
						if (item.Get())
						{
							for (auto& act : item->action_list_)
							{
								if (act.Get())
								{
									tias.CurTotalActionCount++;
									if (EBaseState::BState_FINISHED == act->state)
									{
										tias.CurFinishedActionCount++;
										tias.CurPeriodExp += act->action_exp_benchmark_;
										tias.CurTaskItemExp = act->action_exp_benchmark_;
										tias.CurTaskItemPlayTime = act->action_operation_end_time_ > act->action_operation_start_time_ ? act->action_operation_end_time_ - act->action_operation_start_time_ : 0;
										tias.CurPeriodPlayTime += tias.CurTaskItemPlayTime;
										tias.CurActionName = act->action_name_;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return tias;
}

void ATaskPool::GetAllTaskItemActionWithSuggestOrder(TArray<FTaskItemAction>& actions, const int script_index)
{
	for (auto& task : task_list_)
	{
		if (task->bind_script_->task_script_index_ == script_index)
		{
			FString user_name;
			FString role_name = task->bind_role_->role_name_;
			if (UTrainGameInstance::Instance)
				UTrainGameInstance::Instance->GetUserNameAsRole(role_name, user_name);
			for (auto& item : task->task_item_list_)
			{
				for (auto& action : item->action_list_)
				{
					FTaskItemAction tia;
					tia.TaskItemIndex = item->item_index_;
					tia.TaskNodeIndex = task->task_index_;
					tia.ActName = action->action_name_;
					tia.ActDesc = action->action_desc_;
					tia.ActResult = FString::Printf(TEXT("%s %s@%s"), *action->action_operation_date_time_, *user_name,*role_name);
					tia.ActState = action->state;
					tia.ActRelateToLocation = action->relate_to_location_;
					tia.ActLocation = action->action_location_;
					tia.ActRelateToSpriteLocation = action->relate_to_sprite_location_;
					tia.ActSpriteLocation = action->action_sprite_location_;
					tia.ActSpriteScale = action->action_sprite_scale_;
					tia.ActExpBenchmark = action->action_exp_benchmark_;
					tia.ActSuggestOrder = action->action_suggest_order_;
					tia.ActExpActualDuration = FPlatformMath::FloorToInt(action->action_operation_end_time_ - action->action_operation_start_time_);
					actions.Add(tia);
				}
			}
		}
	}
	if (actions.Num() > 1)
	{
		actions.Sort([](const FTaskItemAction& LHS, const FTaskItemAction& RHS) {
			return LHS.ActSuggestOrder < RHS.ActSuggestOrder;
		});
	}

}

void ATaskPool::GetRoleTaskItemActionWithSuggestOrder(TArray<FTaskItemAction>& actions, const int script_index, const int role_index)
{
	for (auto& task : task_list_)
	{
		if (task->bind_script_->task_script_index_ == script_index
			&& task->bind_role_->role_index_ == role_index )
		{
			FString user_name;
			FString role_name = task->bind_role_->role_name_;
			if (UTrainGameInstance::Instance)
				UTrainGameInstance::Instance->GetUserNameAsRole(role_name, user_name);
			for (auto& item : task->task_item_list_)
			{
				for (auto& action : item->action_list_)
				{
					FTaskItemAction tia;
					tia.TaskItemIndex = item->item_index_;
					tia.TaskNodeIndex = task->task_index_;
					tia.ActName = action->action_name_;
					tia.ActDesc = action->action_desc_;
					tia.ActResult = FString::Printf(TEXT("%s %s@%s"), *action->action_operation_date_time_, *user_name, *role_name);
					tia.ActState = action->state;
					tia.ActRelateToLocation = action->relate_to_location_;
					tia.ActLocation = action->action_location_;
					tia.ActRelateToSpriteLocation = action->relate_to_sprite_location_;
					tia.ActSpriteLocation = action->action_sprite_location_;
					tia.ActSpriteScale = action->action_sprite_scale_;
					tia.ActExpBenchmark = action->action_exp_benchmark_;
					tia.ActSuggestOrder = action->action_suggest_order_;
					tia.ActExpActualDuration = FPlatformMath::FloorToInt(action->action_operation_end_time_ - action->action_operation_start_time_);
					actions.Add(tia);
				}
			}
		}
	}
	if (actions.Num() > 1)
	{
		actions.Sort([](const FTaskItemAction& LHS, const FTaskItemAction& RHS) {
			return LHS.ActSuggestOrder < RHS.ActSuggestOrder;
		});
	}
}

FString ATaskPool::GetTaskScriptTableNameByMeta(const TArray<FTaskDataMeta>& tdm, ETaskDataMetaType type) const
{
	for (auto& dm : tdm)
	{
		if (dm.MetaType == type)
			return dm.MetaName;
	}
	return FString();
}

FScriptTaskDataStatistics ATaskPool::GetTaskScriptStatistics(const FString & script_name)
{
	FScriptTaskDataStatistics sds;
	sds.TaskItemNum = 0;
	sds.TaskNodeNum = 0;
	sds.TaskRoleNum = 0;
	sds.SuggestCostTimeToPlay = 0.f;
	sds.ScriptName = script_name;
	RUNTIME_TASK_BASE::TaskList tl;
	GetTaskListByScriptName(script_name, tl);
	for (auto& task : tl)
	{
		sds.TaskNodeNum = FMath::Max(sds.TaskNodeNum,task->task_index_);
		sds.TaskItemNum += task->task_item_list_.Num();
		sds.TaskRoleNum = FMath::Max(sds.TaskRoleNum, task->bind_role_->role_index_);
	}
	sds.SuggestCostTimeToPlay = FMath::CeilToFloat(sds.TaskItemNum * 0.618f * 2.f);
	return sds;
}

int ATaskPool::GetTaskScriptNum()
{
	return task_script_list_.Num();
}

FScriptTaskData ATaskPool::GetScriptData(const RUNTIME_TASK_BASE::TaskScriptPtr & sp) const
{
	FScriptTaskData st;
	if (sp.Get())
	{
		st.Memo = sp->script_memo_;
		st.ScriptName = sp->task_script_name_;
		st.IconPath = sp->script_icon_path_;
		st.TypeName = sp->script_type_name_;
		st.MainLevelName = sp->script_main_level_name_;
		for (auto& meta : sp->meta_tables_)
		{
			FTaskDataMeta tdm;
			tdm.MetaType = (ETaskDataMetaType)meta.Key;
			tdm.MetaName = meta.Value;
			st.TaskDataMeta.Add(tdm);
		}
	}
	return st;
}


FScriptTaskData ATaskPool::GetTaskScriptByName(const FString& script_name) const
{
	RUNTIME_TASK_BASE::TaskScriptPtr ts = TaskReader::GetTaskScriptByName(script_name);
	return GetScriptData(ts);
}

FScriptTaskData ATaskPool::GetTaskScriptByIndex(const int index)
{
	RUNTIME_TASK_BASE::TaskScriptPtr ts = TaskReader::GetTaskScriptByIndex(index);
	return GetScriptData(ts);
}

FScriptTaskData ATaskPool::GetActiveScript()
{
	for (auto& ts : task_script_list_)
	{
		if (ts.Get() && ts->active_)
			return GetScriptData(ts);
	}
	return FScriptTaskData();
}

int ATaskPool::GetActiveScriptIndex() const
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->active_ )
		{
			return script->task_script_index_;
		}
	}
	return 0;
}

void ATaskPool::SetActiveScript(const FString & script_name, bool active)
{
	for (auto& script : task_script_list_)
	{
		if (script.Get())
		{
			script->active_ = script->task_script_name_.Equals(script_name) & active;
		}
	}
}

EScriptState ATaskPool::IsActiveScript(const FString & script_name)
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->task_script_name_.Equals(script_name))
		{
			return script->active_? EScriptState_Active: EScriptState_Inactive;
		}
	}
	return EScriptState_Unvalid;
}

FString ATaskPool::GetActiveScriptName() const
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->active_)
		{
			return script->task_script_name_;
		}
	}
	return FString();
}

FString ATaskPool::GetActiveScriptMainLevelName() const
{
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->active_)
		{
			return script->script_main_level_name_;
		}
	}
	return FString();
}

TArray<FString> ATaskPool::GetScriptTypeNames() const
{
	TArray<FString> stn;
	for (auto& script : task_script_list_)
	{
		if (script.Get())
		{
			stn.AddUnique(script->script_type_name_);
		}
	}
	return stn;
}

TArray<FString> ATaskPool::GetScriptNamesOfType(const FString & script_type_name) const
{
	TArray<FString> sn;
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->script_type_name_.Equals(script_type_name))
		{
			sn.AddUnique(script->task_script_name_);
		}
	}
	return sn;
}

TArray<FScriptTaskData> ATaskPool::GetScriptEssentialOfType(const FString & script_type_name) const
{
	TArray<FScriptTaskData> data;
	for (auto& script : task_script_list_)
	{
		if (script.Get() && script->script_type_name_.Equals(script_type_name))
		{
			FScriptTaskData _d;
			_d.IconPath = script->script_icon_path_;
			_d.ScriptName = script->task_script_name_;
			_d.TypeName = script_type_name;
			_d.Memo = script->script_memo_;
			data.Add(_d);
		}
	}
	return data;
}

TArray<int> ATaskPool::SortTaskBySuggetOrder(TArray<int> arrRoleSelected,int cur_sel)
{
	//base sorting.
	if (arrRoleSelected.Num() > 1)
	{
		arrRoleSelected.Sort([=](const int& LHS, const int& RHS) {
			RUNTIME_TASK_BASE::TaskPtr task_ptr_lhs = GetSpecificTask(LHS + 1);
			RUNTIME_TASK_BASE::TaskPtr task_ptr_rhs = GetSpecificTask(RHS + 1);
			if (task_ptr_lhs.Get() && task_ptr_rhs.Get())
			{
				if (task_ptr_lhs->suggest_order_ == task_ptr_rhs->suggest_order_)
					return task_ptr_lhs->task_index_ - 1 == cur_sel;
				else
					return task_ptr_lhs->suggest_order_ < task_ptr_rhs->suggest_order_;
			}
			return false;
		});
	}
	return arrRoleSelected;
}

