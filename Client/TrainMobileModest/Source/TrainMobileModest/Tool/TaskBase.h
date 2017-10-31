// As Part of GuangZhou Training.

#pragma once

/**
 * 
 */

UENUM(BlueprintType)
enum EBaseState
{
	BState_INACTIVED,
	BState_AWAKED,
	BState_ACTIVED,
	BState_FINISHED
};

namespace STATIC_TASK_BASE
{
	class TRAINMOBILEMODEST_API	TaskItemGameTool
	{
	public:
		TaskItemGameTool(int index):tool_index_(index), bind_act_index(0){};
		~TaskItemGameTool() {};
	public:
		int     tool_index_;
		int     bind_act_index;//bind tool to the specific index action
		FString tool_name_;
		FString tool_desc_;
		FString tool_tex_;
	};
	typedef TSharedPtr<TaskItemGameTool> TaskItemGameToolPtr;
	typedef TDoubleLinkedList<TaskItemGameToolPtr> TaskItemGameToolList;

	class TRAINMOBILEMODEST_API TaskItemAction
	{
	public:																						  
		TaskItemAction(int index):action_exp_benchmark_(0), action_suggest_order_(0),action_operation_start_time_(0.f), action_operation_end_time_(0.f),action_index_(index), state(BState_INACTIVED), action_location_(FVector::ZeroVector), relate_to_location_(false), action_sprite_location_(FVector::ZeroVector), action_sprite_scale_(FVector(1.f,1.f,1.f)),relate_to_sprite_location_(false){}
		~TaskItemAction() {}
	public:
		int     action_index_;
		EBaseState state;
		FString action_name_;
		FString action_desc_;
		FString action_operation_result_;
		FString action_operation_date_time_;
		FVector action_location_;//location to place the task-mark-scene-effect.
		FVector action_sprite_location_;//location to place the sprite to click.
		FVector action_sprite_scale_;//scale of the sprite.
		bool    relate_to_location_;//whether relate to location or not.
		bool    relate_to_sprite_location_;//whether relate to the sprite location or not.
		int		action_exp_benchmark_;//benchmark
		int     action_suggest_order_;//suggest operation order.
		float   action_operation_start_time_;//correctly,this should be the one of the run-time attribute.
		float   action_operation_end_time_;//correctly,this should be the one of the run-time attribute.
	};
	typedef TSharedPtr<TaskItemAction> TaskItemActionPtr;
	typedef TDoubleLinkedList<TaskItemActionPtr> TaskItemActionList;
	typedef TDoubleLinkedList<TaskItemActionPtr>::TDoubleLinkedListNode TaskItemActionNode;

	class TRAINMOBILEMODEST_API TaskItem
	{
	public:
		TaskItem(int index):item_index_(index), state(BState_INACTIVED){};
		~TaskItem() {};
	public:
		FString task_content_;
		int     item_index_;
		EBaseState state;
		TaskItemGameToolList game_tool_list_;
		TaskItemActionList action_list_;
	};
	typedef TSharedPtr<TaskItem> TaskItemPtr;
	typedef TDoubleLinkedList<TaskItemPtr> TaskItemList;
	typedef TDoubleLinkedList<TaskItemPtr>::TDoubleLinkedListNode TaskItemNode;
}

//runtime setting.
class TaskStateMachine;
namespace RUNTIME_TASK_BASE
{
	class Task;
	typedef TSharedPtr<Task> TaskPtr;
	typedef TDoubleLinkedList<TaskPtr> TaskList;
	typedef TDoubleLinkedList<TaskPtr>::TDoubleLinkedListNode TaskNode;

	class TRAINMOBILEMODEST_API TaskScript
	{
	public:
		TaskScript(int index):task_script_index_(index), active_(false){}
		~TaskScript() {}
	public:
		bool active_;
		int task_script_index_;
		FString task_script_name_;
		FString script_memo_;
		FString script_type_name_;
	};
	typedef TSharedPtr<TaskScript> TaskScriptPtr;
	typedef TDoubleLinkedList<TaskScriptPtr> TaskScriptList;

	class TRAINMOBILEMODEST_API Role
	{
	public:
		Role(int index):role_index_(index), first_sight_enable_(false),first_sight_fov_(90.f), first_sight_aspect_ration_(1.778f){}
		~Role() {}
	public:
		bool CollectTasks(const TaskList& task_list, TaskList& task_list_of_role);
	public:
		FString role_name_;
		int role_index_;
	public:
		//fist sight config
		bool first_sight_enable_;
		float first_sight_aspect_ration_;
		float first_sight_fov_;
		FRotator first_sight_rotation;
	};
	typedef TSharedPtr<Role> RolePtr;
	typedef TDoubleLinkedList<RolePtr> RoleList;

	class TRAINMOBILEMODEST_API Task
	{
	public:
		Task(int index):task_index_(index), state(BState_INACTIVED){}
		~Task() {}
	public:
		void ProduceFSMComponent(TaskStateMachine* fsm, const STATIC_TASK_BASE::TaskItemPtr& ti );//auto produce components for the fsm
	public:
		void AddTaskItem(const STATIC_TASK_BASE::TaskItemPtr& ti);
		void BindRole(Role* role);
		void BindTaskScript(TaskScript* task_script);
	public:
		STATIC_TASK_BASE::TaskItemList	task_item_list_;
		FString task_name_;
		RolePtr bind_role_;
		TaskScriptPtr bind_script_;
		EBaseState state;
		int task_index_;
	};
}

class TRAINMOBILEMODEST_API TaskReader
{
public:
	virtual void ReadTask() = 0;
	virtual void Reset();
	virtual ~TaskReader() {}
public:
	int GetTaskListByScriptName(const FString& script_name, RUNTIME_TASK_BASE::TaskList& list_to_fill) const;
	RUNTIME_TASK_BASE::TaskScriptPtr GetTaskScriptByName(const FString& script_name) const;
	RUNTIME_TASK_BASE::TaskScriptPtr GetTaskScriptByIndex(const int index) const;
	RUNTIME_TASK_BASE::RolePtr GetTaskRoleByName(const FString& script_name,const FString& role_name) const;//just try to find the role
public:
	RUNTIME_TASK_BASE::TaskList task_list_;
};

