#include "LuaProviderPrivatePCH.h"
#include "LuaMachine.h"
#include "Misc/Paths.h"
#include "sprotodef.h"
#include "SprotoPacker.h"

TSharedPtr<FLuaMachine> FSprotoPacker::LuaMachine = MakeShareable<FLuaMachine>(new FLuaMachine);
FSprotoPacker* FSprotoPacker::SprotoPackerIns = nullptr;

int lsproto_fill_heartbeat(lua_State *L) {
	//fill-request:heartbeat method
	//sproto_fill_heartbeat()
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_heartbeat>(new sproto::sproto_heartbeat());
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_handshake(lua_State *L) {
	//fill-request:handshake method
	//sproto_fill_handshake()('msg',len,agent_index,inlobby)
	const char* msg = lua_tostring(L, 1);
	int len = (int)lua_tonumber(L, 2);
	int agent_index = (int)lua_tonumber(L, 3);
	bool in_lobby = 1 == lua_toboolean(L, 4);
	if (nullptr == msg )
		return 0;
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_handshake>(new sproto::sproto_handshake(
		msg, len, agent_index,in_lobby
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_message_recieve(lua_State *L) {
	//fill-recieve: recieve the forwarding message from other clients[s]
	//sproto_fill_message_recieve('msg',msg_len,'uid',uid_len,from_agent_index)
	const char* msg = lua_tostring(L, 1);
	int msg_len = (int)lua_tonumber(L, 2);
	const char* uid = lua_tostring(L, 3);
	int uid_len = (int)lua_tonumber(L, 4);
	int agent_id = (int)lua_tonumber(L, 5);
	if (nullptr == msg )
		return 0;
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_message>(new sproto::sproto_message(
		msg, msg_len,uid,uid_len, agent_id
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

#define release_dirty_sms()\
if(LUA_TTABLE != lt)\
{\
delete []sms;\
return 0;\
}

int lsproto_fill_movement_recieve(lua_State *L) {
	//fill-movement:movement method(response)
	//sproto_fill_movement_recieve('role',[table-len]#movement,[table]movements)
	/*
	.movement {
	location 0 : *integer #position(float,3*4bytes,transimit as int)
	rotation 1 : *integer #rotation(float,3*4bytes,transimit as int)
	scale 2 : *integer #scale(float,3*4bytes,transimit as int)
	velocity 3 : *integer #velocity(float,3*4bytes,transimit as int)
	animation 4 : string #animation name
	}
	*/
	const char* role = lua_tostring(L, 1);
	int movement_len = (int)lua_tonumber(L, 2);
	if (0 >= movement_len)
		return 0;
	int top = 0;
	int lt = 0;
	int index = 3;
	sproto::movement* sms = new sproto::movement[movement_len];
	//gettable,table is at the position 3 in this stack.
	lt = lua_type(L, index);
	top = lua_gettop(L);
	lua_pushnil(L);//nil as an initial key in this table
	while (lua_next(L, index)) {
		//data will be push in the top of the stack.
		//-1 is value, -2 is key
		int mi_ = (int)lua_tonumber(L, -2);
		--mi_;
		lt = lua_type(L, -1);
		release_dirty_sms()//must be a table.

		lua_getfield(L, -1, "location");//push movement[mi_][location],pop location
		top = lua_gettop(L);
		lt = lua_type(L, top);
		release_dirty_sms()//must be a table.
		lua_pushnil(L);
		while (lua_next(L, top))	//surely at the top,location table
		{
			int iv = (int)lua_tonumber(L, -1);
			float* fv = (float*)&iv;
			int ik = (int)lua_tonumber(L, -2);
			if (1 == ik)
				sms[mi_].px_ = *fv;
			else if (2 == ik)
				sms[mi_].py_ = *fv;
			else if (3 == ik)
				sms[mi_].pz_ = *fv;
			lua_pop(L, 1);//pop value,keep key,next key will be filled.
		}
		lua_pop(L, 1);//pop movement[mi_][location]

		lua_getfield(L, -1, "scale");//push movement[mi_][scale],pop scale
		top = lua_gettop(L);
		lt = lua_type(L, top);
		release_dirty_sms()//must be a table.
		lua_pushnil(L);
		while (lua_next(L, top))	//surely at the top,scale table
		{
			int iv = (int)lua_tonumber(L, -1);
			float* fv = (float*)&iv;
			int ik = (int)lua_tonumber(L, -2);
			if (1 == ik)
				sms[mi_].sx_ = *fv;
			else if (2 == ik)
				sms[mi_].sy_ = *fv;
			else if (3 == ik)
				sms[mi_].sz_ = *fv;
			lua_pop(L, 1);//pop value,keep key,next key will be filled.
		}
		lua_pop(L, 1);//pop movement[mi_][scale]

		lua_getfield(L, -1, "rotation");//push movement[mi_][rotation],pop rotation
		top = lua_gettop(L);
		lt = lua_type(L, top);
		release_dirty_sms()//must be a table.
			lua_pushnil(L);
		while (lua_next(L, top))	//surely at the top,rotation table
		{
			int iv = (int)lua_tonumber(L, -1);
			float* fv = (float*)&iv;
			int ik = (int)lua_tonumber(L, -2);
			if (1 == ik)
				sms[mi_].er_ = *fv;
			else if (2 == ik)
				sms[mi_].ep_ = *fv;
			else if (3 == ik)
				sms[mi_].ey_ = *fv;
			lua_pop(L, 1);//pop value,keep key,next key will be filled.
		}
		lua_pop(L, 1);//pop movement[mi_][rotation]

		lua_getfield(L, -1, "velocity");//push movement[mi_][velocity],pop velocity
		top = lua_gettop(L);
		lt = lua_type(L, top);
		release_dirty_sms()//must be a table.
			lua_pushnil(L);
		while (lua_next(L, top))	//surely at the top,velocity table
		{
			int iv = (int)lua_tonumber(L, -1);
			float* fv = (float*)&iv;
			int ik = (int)lua_tonumber(L, -2);
			if (1 == ik)
				sms[mi_].vx_ = *fv;
			else if (2 == ik)
				sms[mi_].vy_ = *fv;
			else if (3 == ik)
				sms[mi_].vz_ = *fv;
			lua_pop(L, 1);//pop value,keep key,next key will be filled.
		}
		lua_pop(L, 1);//pop movement[mi_][velocity]

		lua_getfield(L, -1, "animation");//push movement[mi_][animation],pop animation
		top = lua_gettop(L);
		lt = lua_type(L, top);
		{
			const char* ani = lua_tostring(L, top);
			mem_assign(char, sms[mi_].n_, ani, sms[mi_].n_len_, strlen(ani), 1);
		}
		lua_pop(L, 1);//pop movement[mi_][animation]

		lua_pop(L, 1); //pop table value,keep key,next key will be filled.
	}

	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_movement>(new sproto::sproto_movement(
		role, strlen(role), sms, movement_len
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_get_response(lua_State *L) {
	//fill-response: get method respond
	//sproto_fill_get_response('msg',len)
	const char* msg = lua_tostring(L, 1);
	int len = (int)lua_tonumber(L, 2);
	if (nullptr == msg )
		return 0;
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_get>(new sproto::sproto_get(
		msg, len
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_queryuid_response(lua_State *L) {
	//fill-queryuid-response: recieve the queried user id with the specific agentid.
	//sproto_fill_queryuid_response('uid',uid_len,found)
	const char* uid = lua_tostring(L, 1);
	int uid_len = (int)lua_tonumber(L, 2);
	bool found = 1 == lua_toboolean(L, 3);

	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_queryuid>(new sproto::sproto_queryuid(
		uid,uid_len,found
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

/*
.lobbytaskaction {
task_id 0 : integer
item_id 1 : integer
action_id 2 : integer
action_status 3 : integer
}

.lobbymember {
user_id 0 : string
is_host 1 : boolean
ability 2 : integer #for matching up.
actions 3 : *lobbytaskaction
}

.lobby {
lobbyname 0 : string
stage 1 : integer #may be updated.
averability 2 : integer #for matching,clients should join in the most-close-averablility lobby in the auto matching mode.
members 3 : *lobbymember #may be updated at different time
}
*/
#define ensure_is_table(lt)\
if(LUA_TTABLE != lt)\
{\
return 0;\
}

bool _sproto_recieve_lobby_from_lua(lua_State *L, int index,sproto::lobby*& lo)
{
	//lobby is a table.
	if (0 == lo)
		return false;
	int lt = 0;
	lt = lua_type(L, index);
	ensure_is_table(lt)
	lua_pushnil(L);//nil as an initial key in this table
	while (lua_next(L, index)) {
		//data will be push in the top of the stack.
		//-1 is value, -2 is key
		const char* key = lua_tostring(L, -2);
		if (0 == strcmp(key, "lobbyname"))
		{
			const char* val = lua_tostring(L, -1);
			mem_assign(char, lo->lobby_name_, val, lo->lobby_name_len_, strlen(val), 1);
		}
		else if (0 == strcmp(key, "stage"))
		{
			int stage = (int)lua_tonumber(L, -1);
			lo->stage_ = stage;
		}
		else if (0 == strcmp(key, "averability"))
		{
			int averability = (int)lua_tonumber(L, -1);
			float* a = (float*)&averability;
			lo->averability_ = *a;
		}
		else if (0 == strcmp(key, "members"))
		{
			//a table
			lt = lua_type(L, -1);
			ensure_is_table(lt)
			size_t member_count = lua_rawlen(L, -1);
			if (member_count > 0)
			{
				lo->members_ = new sproto::lobbymember[member_count];
				lo->member_num_ = member_count;
				int top1 = lua_gettop(L);
				lua_pushnil(L);
				while (lua_next(L, top1))
				{
					int mi = lua_tonumber(L, -2);
					--mi;
					lt = lua_type(L, -1);
					ensure_is_table(lt)
					sproto::lobbymember& m = lo->members_[mi];
					int top2 = lua_gettop(L);
					lua_pushnil(L);
					while (lua_next(L, top2))
					{
						const char* mk = lua_tostring(L, -2);
						if (0 == strcmp(mk, "user_id"))
						{
							const char* mv = lua_tostring(L, -1);
							mem_assign(char, m.user_id_, mv, m.user_id_len_, strlen(mv), 1);
						}
						else if (0 == strcmp(mk, "is_host"))
						{
							bool mv = 1 == lua_toboolean(L, -1);
							m.is_host_ = mv;
						}
						else if (0 == strcmp(mk, "ability"))
						{
							int ability = (int)lua_tonumber(L, -1);
							float* a = (float*)&ability;
							m.ability_ = *a;
						}
						else if (0 == strcmp(mk, "stage"))
						{
							const char* mv = lua_tostring(L, -1);
							mem_assign(char, m.user_stage_, mv, m.user_stage_len_, strlen(mv), 1);
						}
						else if (0 == strcmp(mk, "exp_gain"))
						{
							m.exp_gain_ = (int)lua_tonumber(L, -1);
						}
						else if (0 == strcmp(mk, "time_spend"))
						{
							m.time_spend_ = (int)lua_tonumber(L, -1);
						}
						else if (0 == strcmp(mk, "actions"))
						{
							//action is still a table.
							lt = lua_type(L, -1);
							ensure_is_table(lt)
							size_t action_count = lua_rawlen(L, -1);
							if (action_count > 0)
							{
								m.task_actions_ = new sproto::lobbytaskaction[action_count];
								m.task_action_num_ = action_count;
								int top3 = lua_gettop(L);
								lua_pushnil(L);
								while (lua_next(L, top3))
								{
									int ai = (int)lua_tonumber(L, -2);
									--ai;
									sproto::lobbytaskaction& act = m.task_actions_[ai];
									lt = lua_type(L, -1);
									ensure_is_table(lt)
									int top4 = lua_gettop(L);
									lua_pushnil(L);
									while (lua_next(L, top4))
									{
										const char* ak = lua_tostring(L, -2);
										if (0 == strcmp(ak, "task_id"))
										{
											int ti = (int)lua_tonumber(L, -1);
											act.task_id_ = ti;
										}
										else if (0 == strcmp(ak, "item_id"))
										{
											int ii = (int)lua_tonumber(L, -1);
											act.item_id_ = ii;
										}
										else if (0 == strcmp(ak, "action_id"))
										{
											int _ai = (int)lua_tonumber(L, -1);
											act.action_id_ = _ai;
										}
										else if (0 == strcmp(ak, "action_status"))
										{
											int as = (int)lua_tonumber(L, -1);
											act.action_status_ = as;
										}
										lua_pop(L, 1);
									}
									lua_pop(L, 1);
								}
							}
						}
						lua_pop(L, 1);
					}
					lua_pop(L, 1);
				}
			}
		}
		lua_pop(L, 1);//pop value,keep key,next key will be filled.
	}
	return true;
}

int lsproto_fill_lobbychanged_recieve(lua_State *L)
{
	//recieve the event of the lobby changing.
	//sproto_fill_lobbychanged_recieve(lobby)

	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_lobby_changed>(new sproto::sproto_lobby_changed(
		new sproto::lobby()
		));
	if (_sproto_recieve_lobby_from_lua(L, 1, static_cast<sproto::sproto_lobby_changed*>(sd.Get())->lobby_))
	{
		FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
		return 1;
	}
	return 0;
}

int lsproto_fill_hostchanged_recieve(lua_State *L)
{
	//recive the event of the host changing.
	//sproto_fill_hostchanged_recieve(sethost)
	bool sethost = 1 == lua_toboolean(L,1);
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_host_changed>(new sproto::sproto_host_changed(
		sethost
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_queryonlineuser_response(lua_State *L)
{
	//recieve the response of querying onlineuser.
	//sproto_fill_queryonlineuser_response(userids) --userids is a table.
	int index = 1;
	int lt = 0;
	lt = lua_type(L, index);
	ensure_is_table(lt)
	size_t num = lua_rawlen(L, index);
	if (num <= 0)
		return 0;
	
	sproto::sproto_queryonlineuser* ou = new sproto::sproto_queryonlineuser();
	ou->user_ids_ = new char*[num];
	ou->user_id_num_ = num;

	lua_pushnil(L);//nil as an initial key in this table
	while (lua_next(L, index)) {
		int ui = (int)lua_tonumber(L, -2);
		--ui;
		const char* id = lua_tostring(L, -1);
		char*& uid = ou->user_ids_[ui];
		int nouse = 0;
		mem_assign(char, uid, id, nouse, strlen(uid), 1)
		lua_pop(L, 1);
	}
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_queryonlineuser>(ou);
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_querylobby_response(lua_State *L)
{
	//recive the response of all current lobbies.
	//sproto_fill_querylobby_response(lobbies)
	int index = 1;
	int lt = 0;
	lt = lua_type(L, index);
	ensure_is_table(lt)
	size_t num = lua_rawlen(L, index);
	if (num <= 0)
		return 0;

	sproto::sproto_querylobby* ql = new sproto::sproto_querylobby();
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_querylobby>(ql);
	ql->lobbies_ = new sproto::lobby[num];
	ql->lobby_num_ = num;

	lua_pushnil(L);//nil as an initial key in this table
	while (lua_next(L, index)) {
		int qi = (int)lua_tonumber(L, -2);
		--qi;
		sproto::lobby* lb = &ql->lobbies_[qi];
		if (!_sproto_recieve_lobby_from_lua(L, lua_gettop(L), lb))
		{
			lua_pop(L, 1);
			return 0;
		}
		lua_pop(L, 1);
	}
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_createlobby_response(lua_State *L)
{
	//recive the response of the result of lobby-created.
	//sproto_fill_createlobby_response(succeed)
	bool succeed = 1 == lua_toboolean(L, 1);
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_createlobby>(new sproto::sproto_createlobby(
		succeed
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_updatelobby_response(lua_State *L)
{
	//recive the response of the result of lobby-updated.
	//sproto_fill_updatelobby_response(succeed)
	bool succeed = 1 == lua_toboolean(L, 1);
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_updatelobby>(new sproto::sproto_updatelobby(
		succeed
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_joinlobby_response(lua_State *L)
{
	//recive the response of the result of lobby-joined.
	//sproto_fill_joinlobby_response(succeed,lobby)
	bool succeed = 1 == lua_toboolean(L, 1);
	int index = 2;
	int lt = 0;
	lt = lua_type(L, index);
	ensure_is_table(lt)
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_joinlobby>(new sproto::sproto_joinlobby(
			succeed,new sproto::lobby()
		));
	sproto::lobby*& lb = static_cast<sproto::sproto_joinlobby*>(sd.Get())->lobby_;
	if (_sproto_recieve_lobby_from_lua(L, index, lb))
	{
		FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	}
	return 1;
}

int lsproto_fill_assignmemberaction_response(lua_State *L)
{
	//recive the response of the result of assignmemberaction.
	//sproto_fill_assignmemberaction_response(succeed)
	bool succeed = 1 == lua_toboolean(L, 1);
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_assignmemberaction>(new sproto::sproto_assignmemberaction(
		succeed
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_updatememberaction_response(lua_State *L)
{
	//recive the response of the result of updatememberaction.
	//sproto_fill_updatememberaction_response(succeed)
	bool succeed = 1 == lua_toboolean(L, 1);
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_updatememberaction>(new sproto::sproto_updatememberaction(
		succeed
		));
	FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
	return 1;
}

int lsproto_fill_game_ready_recieve(lua_State* L)
{
	//recive the response of the notification of game should be ready.
	//sproto_fill_game_ready_recieve(lobby)
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_game_ready>(new sproto::sproto_game_ready(
		new sproto::lobby()
		));
	if (_sproto_recieve_lobby_from_lua(L, 1, static_cast<sproto::sproto_game_ready*>(sd.Get())->lobby_))
	{
		FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
		return 1;
	}
	return 0;
}

int lsproto_fill_game_launch_recieve(lua_State* L)
{
	//recive the response of the notification of game should be launched on.
	//sproto_fill_game_launch_recieve(lobby)
	FSprotoPacker::FSprotoBufferPtr sd = MakeShareable<sproto::sproto_game_launch>(new sproto::sproto_game_launch(
		new sproto::lobby()
		));
	if (_sproto_recieve_lobby_from_lua(L, 1, static_cast<sproto::sproto_game_launch*>(sd.Get())->lobby_))
	{
		FSprotoPacker::SprotoPackerIns->RecieverPutData(sd);
		return 1;
	}
	return 0;
}

static const struct luaL_Reg lua_reg[] = {
	{ "sproto_fill_heartbeat", lsproto_fill_heartbeat },
	{ "sproto_fill_message_recieve",lsproto_fill_message_recieve },
	{ "sproto_fill_movement_recieve", lsproto_fill_movement_recieve },
	{ "sproto_fill_lobbychanged_recieve",lsproto_fill_lobbychanged_recieve },
	{ "sproto_fill_hostchanged_recieve",lsproto_fill_hostchanged_recieve },
	{ "sproto_fill_game_ready_recieve",lsproto_fill_game_ready_recieve },
	{ "sproto_fill_game_launch_recieve",lsproto_fill_game_launch_recieve },
	{ "sproto_fill_handshake", lsproto_fill_handshake },
	{ "sproto_fill_get_response", lsproto_fill_get_response },
	{ "sproto_fill_queryuid_response",lsproto_fill_queryuid_response },
	{ "sproto_fill_queryonlineuser_response",lsproto_fill_queryonlineuser_response },
	{ "sproto_fill_querylobby_response",lsproto_fill_querylobby_response },
	{ "sproto_fill_createlobby_response",lsproto_fill_createlobby_response },
	{ "sproto_fill_updatelobby_response",lsproto_fill_updatelobby_response },
	{ "sproto_fill_joinlobby_response",lsproto_fill_joinlobby_response },
	{ "sproto_fill_assignmemberaction_response",lsproto_fill_assignmemberaction_response },
	{ "sproto_fill_updatememberaction_response",lsproto_fill_updatememberaction_response },
	{ NULL, NULL },
};

FSprotoPacker::FSprotoPacker()
{
	SprotoPackerIns = this;
}

FSprotoPacker::~FSprotoPacker()
{
}

void FSprotoPacker::Initialize()
{
	LuaMachine->Initialize();
	NetSprotoSetup(LuaMachine.Get());
	LuaMachine->RegisterLibrary(lua_reg,"ue4lua");
}

TArray<FSprotoPacker::FSprotoBufferPtr> FSprotoPacker::GetRecieverData()
{
	FScopeLock Lock(&RecieverBuffCritical);
	TArray<FSprotoBufferPtr> buffers;
	for (int i = 0; i < 10; ++i)
	{
		if (RecieverData[i].Get())
		{
			FSprotoBufferPtr d = RecieverData[i];
			buffers.Add(d);
			RecieverData[i].Reset();
		}
	}
	return buffers;
}

void FSprotoPacker::RecieverPutData(const FSprotoBufferPtr & d)
{
	FScopeLock Lock(&RecieverBuffCritical);
	for (int i = 0; i < 10; ++i)
	{
		if (nullptr == RecieverData[i].Get())
		{
			RecieverData[i] = d;
			return;
		}
	}
}

const static FString LuaSproto(TEXT("luaSproto"));
const static FString net_sproto_packaging(TEXT("netSprotoPackaging"));
const static FString net_sproto_packlen(TEXT("netSprotoPacklen"));
const static FString net_sproto_unpackaging(TEXT("netSprotoUnPackaging"));

bool FSprotoPacker::NetSprotoSetup(FLuaMachine* lvm, const FString lib)
{
	if (lib.IsEmpty() || nullptr == lvm )
		return false;
	return lvm->LoadLuaString(FString::Printf(TEXT("%s=require '%s'"), *LuaSproto, *lib));
}

FSprotoPacker::FSenderBufferPtr FSprotoPacker::NetSprotoPack(const FString & proto_request, const FString & str_to_pack)
{
	static int pack_session = 0;
	LuaMachine->LoadLuaString(FString::Printf(TEXT("local r_=%s.request('%s',%s,%d) %s,%s=%s.pack_package(r_)"),
		*LuaSproto, *proto_request, *str_to_pack, ++pack_session,
		*net_sproto_packaging, *net_sproto_packlen, *LuaSproto));
	return LuaMachine->GetBinaryData(net_sproto_packaging, net_sproto_packlen);
}

void FSprotoPacker::NetSprotoUnPack(const uint8* pack, const int32 len)
{
	if (LuaMachine->SetBinaryData(net_sproto_unpackaging, (const char*)pack, len))
	{
		LuaMachine->LoadLuaString(FString::Printf(TEXT("if nil ~= %s and nil ~= %s then %s.dispatch_package(%s) end"),
			*LuaSproto, *net_sproto_unpackaging, *LuaSproto, *net_sproto_unpackaging));
		LuaMachine->LoadLuaString(FString::Printf(TEXT("%s=nil"), *net_sproto_unpackaging));
	}
}
