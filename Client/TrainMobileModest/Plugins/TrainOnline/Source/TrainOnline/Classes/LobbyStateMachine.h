#pragma once
//lobby state machine
#include "StateMachine.h"
#include "LobbyProducer.h"
struct SLobby;
//lobby states
class TRAINONLINE_API LobbyState
	:public FSM_State
{
public:
	LobbyState(SLobby* lb): lobby_host_(lb), lobby_stage_(LOBBY_ERROR){}
	virtual ~LobbyState() {}
public:
	SLobby* lobby_host_;
	ELobbyStage lobby_stage_;
};

class TRAINONLINE_API LobbyState_Create_Requesting
	:public LobbyState
{
public:
	LobbyState_Create_Requesting(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_create_requesting");
		lobby_stage_ = LOBBY_CREATE_REQUESTING;
	}
	~LobbyState_Create_Requesting() {}
};

class TRAINONLINE_API LobbyState_Join_Requesting
	:public LobbyState
{
public:
	LobbyState_Join_Requesting(SLobby* lb)
		:LobbyState(lb)
	{
		lobby_name_ = TEXT("matchup");
		state_name_ = TEXT("state_join_requesting");
		lobby_stage_ = LOBBY_JOIN_REQUESTING;
	}
	FString lobby_name_;
	~LobbyState_Join_Requesting() {}
};

class TRAINONLINE_API LobbyState_Hosting
	:public LobbyState
{
public:
	LobbyState_Hosting(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_hosting");
		lobby_stage_ = LOBBY_HOSTING;
	}
	~LobbyState_Hosting() {}
};

class TRAINONLINE_API LobbyState_Host_Changing
	:public LobbyState
{
public:
	LobbyState_Host_Changing(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_host_changing");
		lobby_stage_ = LOBBY_HOST_CHANGING;
	}
	~LobbyState_Host_Changing() {}
};

class TRAINONLINE_API LobbyState_Locking
	:public LobbyState
{
public:
	LobbyState_Locking(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_locking");
		lobby_stage_ = LOBBY_LOCKING;
	}
	~LobbyState_Locking() {}
};

class TRAINONLINE_API LobbyState_Launching
	:public LobbyState
{
public:
	LobbyState_Launching(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_launching");
		lobby_stage_ = LOBBY_LAUNCHING;
	}
	~LobbyState_Launching() {}
};

class TRAINONLINE_API LobbyState_Role_Selecting
	:public LobbyState
{
public:
	LobbyState_Role_Selecting(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_role_selecting");
		lobby_stage_ = LOBBY_ROLE_SELECTING;
	}
	~LobbyState_Role_Selecting() {}
};

class TRAINONLINE_API LobbyState_Destroy
	:public LobbyState
{
public:
	LobbyState_Destroy(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_destroy");
		lobby_stage_ = LOBBY_DESTROY;
	}
	~LobbyState_Destroy() {}
};

class TRAINONLINE_API LobbyState_Leave
	:public LobbyState
{
public:
	LobbyState_Leave(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_leave");
		lobby_stage_ = LOBBY_LEAVE;
	}
	~LobbyState_Leave() {}
};

class TRAINONLINE_API LobbyState_InGame
	:public LobbyState
{
public:
	LobbyState_InGame(SLobby* lb)
		:LobbyState(lb)
	{
		state_name_ = TEXT("state_in_game");
		lobby_stage_ = LOBBY_IN_GAME;
	}
	~LobbyState_InGame() {}
};
//lobby functions

class TRAINONLINE_API LobbyFunction_Super
	:public FSM_Function
{
public:
	LobbyFunction_Super(const FSM_StatePtr& own_state) :FSM_Function(own_state) {}
	~LobbyFunction_Super(){}
	typedef LobbyFunction_Super Super;
public:
	virtual bool Explain();
};

class TRAINONLINE_API LobbyFunction_Create_Requesting
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Create_Requesting(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Create_Requesting() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Hosting
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Hosting(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Hosting() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Launching
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Launching(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Launching() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Role_Selecting
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Role_Selecting(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Role_Selecting() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Destroy
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Destroy(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Destroy() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Join_Requesting
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Join_Requesting(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Join_Requesting() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Host_Changing
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Host_Changing(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Host_Changing() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Locking
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Locking(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Locking() {}
public:
	bool Explain();
};

class TRAINONLINE_API LobbyFunction_Leave
	:public LobbyFunction_Super
{
public:
	LobbyFunction_Leave(const FSM_StatePtr& own_state) :LobbyFunction_Super(own_state) {}
	~LobbyFunction_Leave() {}
public:
	bool Explain();
};

class TRAINONLINE_API FLobbyStateMachine
	:public FSM_Object
{
public:
	FLobbyStateMachine();
	~FLobbyStateMachine();
public:
	void SetupFSM(SLobby* lb, LobbyStateCallFunc bind_func);
	void Reset();
	bool ExplainCurrentFunction();
	bool IsFSMSetuped();
	void SetNextStateFunction(const FString& state_name);
	static FSM_FunctionPtr ProduceLobbyFuction(const FString & state_name, const FSM_StatePtr & own_state);
private:
	void ProduceFSMComponent();
public:
	SLobby* LobbyHost;
	LobbyStateCallFunc BindFunc;
};


