#pragma once
//implemented according to proto.lua,using for c/c++ calling.
//not pure c style.

#define mem_assign(t,d,s,dl,sl,a) \
{\
dl = sl; \
d = 0; \
size_t u = sizeof(t); \
if (sl > 0)	\
{\
  d = new t[sl+a];\
  memcpy(d,s,u*sl);\
  if( a > 0 ) d[sl] = '\0';\
}\
}

//m is an array.
#define mem_free(m) \
{if(m) delete []m;}

#define mem_free_single(m) \
{if(m) delete m;}

namespace sproto {
	enum sproto_msg_type {
		REQUEST = 1,
		RESPONSE = 2
	};
	enum sproto_type {
		HANDSHAKE = 1,
		HEARTBEAT = 2,
		GET = 3,
		MESSAGE = 4,
		MOVEMENT = 5,
		QUERY_UID = 6,
		QUERY_ONLINE_USERS = 7,
		QUERY_LOBBIES = 8,
		CREATE_LOBBY = 9,
		UPDATE_LOBBY = 10,
		JOIN_LOBBY = 11,
		ASSIGN_MEMBER_ACTION = 12,
		UPDATE_MEMBER_ACTION = 13,
		LOBBY_CHANGED = 14,
		HOST_CHANGED = 15,
		GAME_READY = 16,
		GAME_LAUNCH = 17
	};

	class sproto_base {
		typedef sproto_base super;
	public:
		sproto_type type_;
		sproto_msg_type st_;
	};

	class sproto_handshake :public sproto_base {
	public:
		sproto_handshake() :msg_(0), len_(0), agent_index_(0), in_lobby_(false){
			type_ = HANDSHAKE;
			st_ = RESPONSE;
		}
		sproto_handshake(const char* msg, const unsigned int len,const int agent_index,const bool in_lobby) {
			type_ = HANDSHAKE;
			st_ = RESPONSE;
			agent_index_ = agent_index;
			in_lobby_ = in_lobby;
			mem_assign(char, msg_, msg, len_, len, 1)
		}
		~sproto_handshake() {
			mem_free(msg_)
		}
		char* msg_;
		unsigned int len_;
		int agent_index_;
		bool in_lobby_;
	};

	class sproto_get :public sproto_base {
	public:
		sproto_get() :value_(0), len_(0) {
			type_ = GET;
			st_ = RESPONSE;
		}
		sproto_get(const char* value, const unsigned int len) {
			type_ = GET;
			st_ = RESPONSE;
			mem_assign(char, value_, value, len_, len, 1)
		}
		~sproto_get() {
			mem_free(value_)
		}
		char* value_;
		unsigned int len_;
	};

	struct movement {
	public:
		movement() :px_(0.f), py_(0.f), pz_(0.f), sx_(0.f), sy_(0.f), sz_(0.f), er_(0.f), ep_(0.f),ey_(0.f), vx_(0.f), vy_(0.f), vz_(0.f), n_(0), n_len_(0) {}
		movement(const float px, const float py, const float pz, const float sx, const float sy, const float sz, const float er, const float ep, const float ey, const float vx, const float vy, const float vz, const char* n, const unsigned int n_len) {
			px_ = px;
			py_ = py;
			pz_ = pz;
			sx_ = sx;
			sy_ = sy;
			sz_ = sz;
			er_ = er;
			ep_ = ep;
			ey_ = ey;
			vx_ = vx;
			vy_ = vy;
			vz_ = vz;
			mem_assign(char, n_, n, n_len_, n_len, 1)
		}
		~movement() {
			mem_free(n_)
		}
		float px_;//position.x
		float py_;//position.y
		float pz_;//position.z
		float sx_;//scale.x
		float sy_;//scale.y
		float sz_;//scale.z
		float er_;//eular.roll
		float ep_;//eular.pitch
		float ey_;//eular.yaw
		float vx_;//velocity.x
		float vy_;//velocity.y
		float vz_;//velocity.z
		char* n_;//animation name
		unsigned int n_len_;
	};

	class sproto_heartbeat :public sproto_base {
	public:
		sproto_heartbeat() {
			type_ = HEARTBEAT;
			st_ = REQUEST;
		}
		~sproto_heartbeat() {
		}
	};

	class sproto_message :public sproto_base {
	public:
		sproto_message() :what_(0), what_len_(0), user_id_(0), user_id_len_(0), from_agent_index_(0) {
			type_ = MESSAGE;
			st_ = REQUEST;
		}
		sproto_message(const char* what, unsigned int what_len, const char* user_id, unsigned int user_id_len, unsigned int from_agent_index) {
			type_ = MESSAGE;
			st_ = REQUEST;
			from_agent_index_ = from_agent_index;
			mem_assign(char, what_, what, what_len_, what_len, 1)
			mem_assign(char, user_id_, user_id, user_id_len, user_id_len, 1)
		}
		~sproto_message() {
			mem_free(what_)
		}
		char* what_;
		unsigned int what_len_;
		char* user_id_;
		unsigned int user_id_len_;
		unsigned int from_agent_index_;
	};

	class sproto_movement :public sproto_base {
	public:
		sproto_movement() : sm_(0), sm_len_(0), role_(0), role_len_(0) {
			type_ = MOVEMENT;
			st_ = REQUEST;
		}
		sproto_movement(const char* role, unsigned int role_len, movement* sm, unsigned int sm_len) {
			type_ = MOVEMENT;
			st_ = REQUEST;
			sm_ = sm;
			sm_len_ = sm_len;
			mem_assign(char, role_, role, role_len_, role_len, 1)
		}
		~sproto_movement() {
			mem_free(role_)
			mem_free(sm_)//sm_ should be assigned outside.
		}
		movement* sm_;
		unsigned int sm_len_;
		char* role_;
		unsigned int role_len_;
	};

	class sproto_queryuid :public sproto_base {
	public:
		sproto_queryuid() : uid_(0), uid_len_(0), uid_found_(false) {
			type_ = QUERY_UID;
			st_ = RESPONSE;
		}
		sproto_queryuid(const char* uid, unsigned int uid_len, bool uid_found) {
			type_ = QUERY_UID;
			st_ = RESPONSE;
			uid_found_ = uid_found;
			mem_assign(char, uid_, uid, uid_len_, uid_len, 1)
		}
		~sproto_queryuid() {
			mem_free(uid_)
		}
		char* uid_;
		unsigned int uid_len_;
		bool uid_found_;
	};

	class sproto_queryonlineuser :public sproto_base {
	public:
		sproto_queryonlineuser() :user_ids_(0), user_id_num_(0) {
			type_ = QUERY_ONLINE_USERS;
			st_ = RESPONSE;
		}
		sproto_queryonlineuser(char** user_ids, unsigned int user_id_num) :user_ids_(user_ids), user_id_num_(user_id_num) {
			type_ = QUERY_ONLINE_USERS;
			st_ = RESPONSE;
		}
		~sproto_queryonlineuser() {
			for (unsigned int i = 0; i < user_id_num_; ++i)
			{
				mem_free(user_ids_[i])
			}
			mem_free(user_ids_)
		}
		char** user_ids_;//each user is a char*.SHOULD be assiged outside.
		unsigned int user_id_num_;
	};

	struct lobbytaskaction {
		lobbytaskaction() : task_id_(0), item_id_(0), action_id_(0), action_status_(0) {}
		unsigned int task_id_;
		unsigned int item_id_;
		unsigned int action_id_;
		unsigned int action_status_;
	};

	struct lobbymember {
		lobbymember() :user_id_(0), user_id_len_(0), user_stage_(0), user_stage_len_(0),is_host_(false), ability_(0.f), task_actions_(0), task_action_num_(0), exp_gain_(0), time_spend_(0){}
		~lobbymember()
		{
			mem_free(user_id_)
			mem_free(task_actions_)
		}
		char* user_id_;
		unsigned int user_id_len_;
		char* user_stage_;
		unsigned int user_stage_len_;
		bool is_host_;
		float ability_;
		int exp_gain_;
		int time_spend_;
		lobbytaskaction* task_actions_;
		unsigned int task_action_num_;
	};

	struct lobby {
		lobby() :lobby_name_(0), lobby_name_len_(0), stage_(0), averability_(0.f), members_(0), member_num_(0) {}
		~lobby()
		{
			mem_free(lobby_name_)
			mem_free(members_)
		}
		char* lobby_name_;
		unsigned int lobby_name_len_;
		unsigned int stage_;
		float averability_;
		lobbymember* members_;
		unsigned int member_num_;
	};

	class sproto_querylobby :public sproto_base {
	public:
		sproto_querylobby() :lobbies_(0), lobby_num_(0) {
			type_ = QUERY_LOBBIES;
			st_ = RESPONSE;
		}
		sproto_querylobby(lobby* lobbies, unsigned int lobby_num) :lobbies_(lobbies), lobby_num_(lobby_num) {
			type_ = QUERY_LOBBIES;
			st_ = RESPONSE;
		}
		~sproto_querylobby() {
			mem_free(lobbies_)
		}
		lobby* lobbies_;
		unsigned int lobby_num_;
	};

	class sproto_createlobby :public sproto_base {
	public:
		sproto_createlobby() :succeed_(false) {
			type_ = CREATE_LOBBY;
			st_ = RESPONSE;
		}
		sproto_createlobby(bool succeed) :succeed_(succeed) {
			type_ = CREATE_LOBBY;
			st_ = RESPONSE;
		}
		bool succeed_;
	};

	class sproto_updatelobby :public sproto_base {
	public:
		sproto_updatelobby() :succeed_(false) {
			type_ = UPDATE_LOBBY;
			st_ = RESPONSE;
		}
		sproto_updatelobby(bool succeed) :succeed_(succeed) {
			type_ = UPDATE_LOBBY;
			st_ = RESPONSE;
		}
		bool succeed_;
	};

	class sproto_joinlobby :public sproto_base {
	public:
		sproto_joinlobby() :succeed_(false),lobby_(0) {
			type_ = JOIN_LOBBY;
			st_ = RESPONSE;
		}
		sproto_joinlobby(bool succeed, lobby* lb) :succeed_(succeed), lobby_(lb) {
			type_ = JOIN_LOBBY;
			st_ = RESPONSE;
		}
		~sproto_joinlobby()
		{
			mem_free_single(lobby_)
		}
		bool succeed_;
		lobby* lobby_;//should be assigned outside.
	};

	class sproto_assignmemberaction :public sproto_base {
	public:
		sproto_assignmemberaction() :succeed_(false) {
			type_ = ASSIGN_MEMBER_ACTION;
			st_ = RESPONSE;
		}
		sproto_assignmemberaction(bool succeed) :succeed_(succeed) {
			type_ = ASSIGN_MEMBER_ACTION;
			st_ = RESPONSE;
		}
		bool succeed_;
	};

	class sproto_updatememberaction :public sproto_base {
	public:
		sproto_updatememberaction() :succeed_(false) {
			type_ = UPDATE_MEMBER_ACTION;
			st_ = RESPONSE;
		}
		sproto_updatememberaction(bool succeed) :succeed_(succeed) {
			type_ = UPDATE_MEMBER_ACTION;
			st_ = RESPONSE;
		}
		bool succeed_;
	};

	class sproto_lobby_changed :public sproto_base {
	public:
		sproto_lobby_changed():lobby_(0) {
			type_ = LOBBY_CHANGED;
			st_ = REQUEST;
		}
		sproto_lobby_changed(lobby* lb):lobby_(lb) {
			type_ = LOBBY_CHANGED;
			st_ = REQUEST;
		}
		~sproto_lobby_changed()
		{
			mem_free_single(lobby_)
		}
		lobby* lobby_;//should be assigned outside.
	};

	class sproto_host_changed :public sproto_base {
	public:
		sproto_host_changed() {
			type_ = HOST_CHANGED;
			st_ = REQUEST;
		}
		sproto_host_changed(bool sethost) :sethost_(sethost) {
			type_ = HOST_CHANGED;
			st_ = REQUEST;
		}
		bool sethost_;
	};

	class sproto_game_ready :public sproto_base {
	public:
		sproto_game_ready():lobby_(0){
			type_ = GAME_READY;
			st_ = REQUEST;
		}
		sproto_game_ready(lobby* lb) :lobby_(lb) {
			type_ = GAME_READY;
			st_ = REQUEST;
		}
		~sproto_game_ready()
		{
			mem_free_single(lobby_)
		}
		lobby* lobby_;//should be assigned outside.
	};

	class sproto_game_launch :public sproto_base {
	public:
		sproto_game_launch():lobby_(0){
			type_ = GAME_LAUNCH;
			st_ = REQUEST;
		}
		sproto_game_launch(lobby* lb) :lobby_(lb) {
			type_ = GAME_LAUNCH;
			st_ = REQUEST;
		}
		~sproto_game_launch()
		{
			mem_free_single(lobby_)
		}
		lobby* lobby_;//should be assigned outside.
	};
}










