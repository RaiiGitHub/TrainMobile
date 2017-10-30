#pragma once
//callback functions
typedef void(*LobbyStateCallFunc)(class LobbyState* ls);
//background lobby supported.
enum ELobbyStage
{
	//noted that these stage is an asssistance of lobby synchroizating to both clients and the server.
	LOBBY_ERROR,
	LOBBY_CREATE_REQUESTING = 1,//notify socket server to create a lobby(host)
	LOBBY_HOSTING = 2,//waiting for other clients to join in.
	LOBBY_LAUNCHING = 3,//all ready and should launch in the game.
	LOBBY_ROLE_SELECTING = 4,//role menu poping out to select, and result should be sent to the server. the server should be dispatch this message to all clients.
	LOBBY_DESTROY = 5,//1.lobby has been done,datas will be analysing;2.no members in this lobby at the stage of hosting.
	//above is the host sequence of state machine.
	//below is the despersed stages.
	LOBBY_JOIN_REQUESTING = 6,//notify socket server to join in an exist lobby(non-host),especially,"matchup" for auto join in the most-near average ability one of the lobbies.
	LOBBY_HOST_CHANGING = 7,//host can be transmitted to one of the visitors if the previous host quit this lobby.
	LOBBY_LOCKING = 10,//lock this lobby,others will not be allowed to join in.
	LOBBY_LEAVE = 8,//leave this lobby,if he who leaves is the host,should notify the server to changing the host.
	LOBBY_IN_GAME = 9//this lobby is now in game.
};

enum EMemberStage
{
	MEMBER_NOT_READY,
	MEMBER_READY,
	MEMBER_CONNECTED,
	MEMBER_DISCONNECTED
};

struct SLobbyTaskAction {
	int task_id_;
	int item_id_;
	int action_id_;
	int action_status_;//see enum EBaseState
	SLobbyTaskAction() :task_id_(0), item_id_(0), action_id_(0), action_status_(0){}
	bool operator < (const SLobbyTaskAction& ta) const
	{
		if (task_id_ < ta.action_id_)
			return true;
		if (item_id_ < ta.item_id_)
			return true;
		if (action_id_ < ta.item_id_)
			return true;
		return false;
	}
	bool operator == (const SLobbyTaskAction& ta) const
	{
		return task_id_ == ta.task_id_ && item_id_ == ta.item_id_ && action_id_ == ta.action_id_;
	}
	FString Serialize() const
	{
		return FString::Printf(TEXT("{task_id=%d,item_id=%d,action_id=%d,action_status=%d}"),
			task_id_, item_id_, action_id_, action_status_);
	}
};

struct SLobbyMember {
	SLobbyMember() :stage_(MEMBER_NOT_READY),is_host_(false), ability_(0.f), exp_gain_(0),cur_exp_(0),cur_ability_(0.f),time_spend_(0.f), rank_data_ready_(false){}
	SLobbyMember(const FString& uid) :user_id_(uid),stage_(MEMBER_NOT_READY), is_host_(false), ability_(0.f),exp_gain_(0), cur_exp_(0), cur_ability_(0.f), time_spend_(0.f), rank_data_ready_(false){}
	FString user_id_;
	FString user_name_;
	EMemberStage stage_;
	TArray<SLobbyTaskAction> task_actions_;//actions to complete.
	bool is_host_;
	float ability_;
	//for ranking.(data is changing. or should send a request to the server to read.)
	int exp_gain_;
	int cur_exp_;
	double cur_ability_;
	float time_spend_;
	bool rank_data_ready_;

	bool operator < (const SLobbyMember& lm) const
	{
		return user_id_ < lm.user_id_;
	}
	bool operator == (const SLobbyMember& lm) const
	{
		return user_id_ == lm.user_id_;
	}
	FString Serialize() const
	{
		static TMap<EMemberStage, FString> member_stage;
		if (0 == member_stage.Num())
		{
			member_stage.Add(MEMBER_NOT_READY, TEXT("nr"));
			member_stage.Add(MEMBER_READY, TEXT("r"));
			member_stage.Add(MEMBER_CONNECTED, TEXT("gc"));
			member_stage.Add(MEMBER_DISCONNECTED, TEXT("gd"));
		}

		int* _a = (int*)&ability_;
		FString seri_member = FString::Printf(TEXT("{user_id='%s',is_host=%s,ability=%d,exp_gain=%d,time_spend=%d,stage='%s',actions={"),
			*user_id_, is_host_?TEXT("true"):TEXT("false"), *_a ,exp_gain_,(int)time_spend_, *member_stage[stage_]);
		for (auto& act : task_actions_)
		{
			seri_member.Append(act.Serialize());
			seri_member.Append(",");
		}
		seri_member.RemoveFromEnd(",");
		seri_member.Append("}}");
		return seri_member;
	}
	FString ActionSerialize() const
	{
		FString a = TEXT("{");
		for (auto& act : task_actions_)
		{
			a.Append(act.Serialize());
			a.Append(",");
		}
		a.RemoveFromEnd(",");
		a.Append("}");
		return a;
	}
	FString RankJsonSerialize() const
	{
		return FString::Printf(TEXT("{\"id\":\"%s\",\"name\":\"%s\",\"task_count\":%d,\"exp_gain\":%d,\"cur_exp\":%d,\"cur_ability\":%.2f,\"cur_ability_changed\":%.2f,\"time_spend\":%d}")
			, *user_id_,*user_name_, task_actions_.Num(), exp_gain_,cur_exp_, cur_ability_,cur_ability_ - ability_, (int)time_spend_);
	}
};

struct SLobby {
	//note: lobby data will be storaged a complete version in the server.
	SLobby() :stage_(LOBBY_ERROR), averability_(0.f){}
	FString name_;
	ELobbyStage stage_;
	float averability_;
	TArray<SLobbyMember> members_;
	TSharedPtr<class FLobbyStateMachine> LobbyStateMachine;//a state machine to run the lobby.
	bool operator < (const SLobby& lb) const
	{
		return name_ < lb.name_;
	}
	bool operator == (const SLobby& lb) const
	{
		return name_ == lb.name_;
	}
	FString Serialize() const
	{
		int* _a = (int*)&averability_;
		FString seri_lobby = FString::Printf(TEXT("{lobbyname='%s',stage=%d,averability=%d,members={"),
			*name_, (int)stage_,*_a);
		for (auto& member : members_)
		{
			seri_lobby.Append(member.Serialize());
			seri_lobby.Append(TEXT(","));
		}
		seri_lobby.RemoveFromEnd(TEXT(","));
		seri_lobby.Append("}}");
		return seri_lobby;
	}
	void ReCalcuAverAbility()
	{
		float a = 0.f;
		for (auto& member : members_)
			a += member.ability_;
		averability_ = a/ members_.Num();
	}
	void Clear()
	{
		stage_ = LOBBY_ERROR;
		averability_ = 0.f;
		name_.Empty();
		members_.Empty();
	}
	void PendingToRank()
	{
		for (auto& member : members_)
		{
			member.rank_data_ready_ = false;
		}
	}
	bool CheckRankReady()
	{
		for (auto& member : members_)
		{
			if (!member.rank_data_ready_)
				return false;
		}
		return true;
	}
	FString RankJsonSerialize()
	{
		FString r = FString::Printf(TEXT("{\"teamid\":\"%s\",\"members\":["),*name_);
		for (auto& member : members_)
		{
			r.Append(member.RankJsonSerialize());
			r.Append(TEXT(","));
		}
		r.RemoveFromEnd(TEXT(","));
		r.Append(TEXT("]}"));
		PendingToRank();
		return r;
	}
	SLobbyMember* FindMember(const FString& uid)
	{
		for (auto& member : members_)
		{
			if (member.user_id_ == uid)
				return &member;
		}
		return nullptr;
	}
};

typedef TSharedPtr<SLobby> LobbyPtr;
typedef TDoubleLinkedList<LobbyPtr> LobbyList;
typedef TDoubleLinkedList<LobbyPtr>::TDoubleLinkedListNode LobbyNode;

namespace sproto {
	struct lobby;
};

class TRAINONLINE_API FLobbyProducer
{
public:
	FLobbyProducer();
	~FLobbyProducer();
public:
	//Should be created by the host,the creator is the host.Created as current lobby.
	SLobby* CreateLobby(const FString& name);
	//create and join a lobby for outside calling.
	SLobby* JoinLobby();
	//just leave current lobby.
	bool LeaveCurrentLobby();
	//promote by the server that should become the host of the current lobby.
	bool PromoteAsHost();
	//for changing the lobby itself.
	SLobby* GetCurrentLobby();
private:
	LobbyPtr CurrentLobby;
};

