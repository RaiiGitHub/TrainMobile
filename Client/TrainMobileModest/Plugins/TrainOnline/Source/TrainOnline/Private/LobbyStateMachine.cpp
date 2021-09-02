#include "LobbyStateMachine.h"
#include "TrainOnlinePrivatePCH.h"
#include "TrainOnlineActor.h"

FLobbyStateMachine::FLobbyStateMachine()
	:BindFunc(nullptr), LobbyHost(nullptr)
{
}


FLobbyStateMachine::~FLobbyStateMachine()
{
}

void FLobbyStateMachine::SetupFSM(SLobby* lb, LobbyStateCallFunc bind_func)
{
	if (nullptr == lb)
		return;
	Reset();
	LobbyHost = lb;
	BindFunc  = bind_func;
	ProduceFSMComponent();
}

void FLobbyStateMachine::Reset()
{
	FSM_Object::Clear();
	LobbyHost = nullptr;
}

bool FLobbyStateMachine::ExplainCurrentFunction( )
{
	if (input_next_state_func_.Get())
		return input_next_state_func_->Explain();
	return false;
}

bool FLobbyStateMachine::IsFSMSetuped()
{
	return nullptr != LobbyHost;
}

void FLobbyStateMachine::SetNextStateFunction(const FString & state_name)
{
	for (auto& state : finite_state_list_)
	{
		if (state->state_name_.Equals(state_name))
		{
			input_next_state_func_.Reset();
			input_next_state_func_ = FLobbyStateMachine::ProduceLobbyFuction(state_name, state);
			FSM_SymbolPtr symbol = MakeShareable(new FSM_Symbol);
			symbol->symbol_ = state_name;
			input_next_state_func_->SetupNextSymbol(symbol);
			break;
		}
	}
}

FSM_FunctionPtr FLobbyStateMachine::ProduceLobbyFuction(const FString & state_name, const FSM_StatePtr & own_state)
{
	//if (state_name.Equals(TEXT("state_create_requesting")))
	//	return MakeShareable(new LobbyFunction_Create_Requesting(own_state));
	//else if (state_name.Equals(TEXT("state_hosting")))
	//	return MakeShareable(new LobbyFunction_Hosting(own_state));
	//else if (state_name.Equals(TEXT("state_launching")))
	//	return MakeShareable(new LobbyFunction_Launching(own_state));
	//else if (state_name.Equals(TEXT("state_role_selecting")))
	//	return MakeShareable(new LobbyFunction_Role_Selecting(own_state));
	//else if (state_name.Equals(TEXT("state_destroy")))
	//	return MakeShareable(new LobbyFunction_Destroy(own_state));
	//else if (state_name.Equals(TEXT("state_join_requesting")))
	//	return MakeShareable(new LobbyFunction_Join_Requesting(own_state));
	//else if (state_name.Equals(TEXT("state_host_changing")))
	//	return MakeShareable(new LobbyFunction_Host_Changing(own_state));
	//else if (state_name.Equals(TEXT("state_locking")))
	//	return MakeShareable(new LobbyFunction_Locking(own_state));
	//else if (state_name.Equals(TEXT("state_leave")))
	//	return MakeShareable(new LobbyFunction_Leave(own_state));
	return MakeShareable(new LobbyFunction_Super(own_state));
}

void FLobbyStateMachine::ProduceFSMComponent()
{
	if (nullptr == LobbyHost)
		return;
	FSM_StatePtr state_create_requesting = MakeShareable(new LobbyState_Create_Requesting(LobbyHost));
	FSM_StatePtr state_hosting = MakeShareable(new LobbyState_Hosting(LobbyHost));
	FSM_StatePtr state_launching = MakeShareable(new LobbyState_Launching(LobbyHost));
	FSM_StatePtr state_role_selecting = MakeShareable(new LobbyState_Role_Selecting(LobbyHost));
	FSM_StatePtr state_destroy = MakeShareable(new LobbyState_Destroy(LobbyHost));
	FSM_StatePtr state_join_requesting = MakeShareable(new LobbyState_Join_Requesting(LobbyHost));
	FSM_StatePtr state_host_changing = MakeShareable(new LobbyState_Host_Changing(LobbyHost));
	FSM_StatePtr state_locking = MakeShareable(new LobbyState_Locking(LobbyHost));
	FSM_StatePtr state_leave = MakeShareable(new LobbyState_Leave(LobbyHost));
	FSM_StatePtr state_in_game = MakeShareable(new LobbyState_InGame(LobbyHost));

	//a dictionary
	finite_state_list_.AddTail(state_create_requesting);
	finite_state_list_.AddTail(state_hosting);
	finite_state_list_.AddTail(state_launching);
	finite_state_list_.AddTail(state_role_selecting);
	finite_state_list_.AddTail(state_destroy);
	finite_state_list_.AddTail(state_join_requesting);
	finite_state_list_.AddTail(state_host_changing);
	finite_state_list_.AddTail(state_locking);
	finite_state_list_.AddTail(state_leave);
	finite_state_list_.AddTail(state_in_game);

	begining_state_ = state_create_requesting;
	input_next_state_func_ = MakeShareable(new LobbyFunction_Create_Requesting(begining_state_));
	current_state_ = begining_state_;
}

bool LobbyFunction_Super::Explain()
{
	if (own_state_.Get())
	{
		LobbyState* ls = static_cast<LobbyState*>(own_state_.Get());
		if (ls->lobby_host_)
		{
			FLobbyStateMachine* lsm = ls->lobby_host_->LobbyStateMachine.Get();
			if (lsm && lsm->BindFunc)
			{
				lsm->BindFunc(ls);
				return true;
			}
		}
	}
	return false;
}

bool LobbyFunction_Create_Requesting::Explain()
{
	//oops,just don't want to process here.
	//prefer to the callback funcion instead.
	return Super::Explain();
}

bool LobbyFunction_Hosting::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Host_Changing::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Locking::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Launching::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Role_Selecting::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Destroy::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Join_Requesting::Explain()
{
	return Super::Explain();
}

bool LobbyFunction_Leave::Explain()
{
	return Super::Explain();
}

