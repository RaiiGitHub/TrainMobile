#include "TrainOnlinePrivatePCH.h"
#include "LobbyProducer.h"
#include "LobbyStateMachine.h"

FLobbyProducer::FLobbyProducer()
{
}

FLobbyProducer::~FLobbyProducer()
{
}

SLobby * FLobbyProducer::CreateLobby(const FString& name)
{
	if (CurrentLobby.Get())
		return nullptr;//already exist,need to quit first.
	SLobby* lobby = new SLobby();
	lobby->name_ = name;
	lobby->LobbyStateMachine = MakeShareable<FLobbyStateMachine>(new FLobbyStateMachine());
	lobby->stage_ = LOBBY_CREATE_REQUESTING;
	lobby->averability_ = GetUserProducer()->GetClientUser()->ability_;
	SLobbyMember lm;
	lm.is_host_ = true;
	lm.user_id_ = GetUserProducer()->GetClientUser()->id_;
	lm.user_name_ = GetUserProducer()->GetClientUser()->name_;
	lm.ability_ = GetUserProducer()->GetClientUser()->ability_;
	lobby->members_.Add(lm);
	CurrentLobby = MakeShareable<SLobby>(lobby);
	//meanwhile,the outside should send a createlobby request to the server.
	//the lobby-monitor should handle the result echoed from the server.
	return lobby;
}

SLobby* FLobbyProducer::JoinLobby()
{
	//current lobby should be empty.
	if (CurrentLobby.Get())
		return nullptr;
	SLobby* lobby = new SLobby();
	lobby->LobbyStateMachine = MakeShareable<FLobbyStateMachine>(new FLobbyStateMachine());
	CurrentLobby = MakeShareable<SLobby>(lobby);
	return lobby;
}

bool FLobbyProducer::LeaveCurrentLobby()
{
	if (nullptr == CurrentLobby.Get())
		return false;
	CurrentLobby.Reset();
	return true;
}

bool FLobbyProducer::PromoteAsHost()
{
	if (nullptr == CurrentLobby.Get())
		return false;
	FString uid = GetUserProducer()->GetClientUser()->id_;
	for (auto& member : CurrentLobby->members_)
	{
		if (uid.Equals(member.user_id_))
		{
			member.is_host_ = true;
			return true;
		}
	}
	return false;
}

SLobby * FLobbyProducer::GetCurrentLobby()
{
	return CurrentLobby.Get();
}

