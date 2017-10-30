local sprotoparser = require "sprotoparser"

local proto = {}

proto.c2s = sprotoparser.parse [[
.package {
	type 0 : integer
	session 1 : integer
}

handshake 1 {
	request {
		userid 0: string #specific the login id of this user.
	}
	response {
		type 0 : string
		msg 1  : string
		agent_index 2 : integer
		inlobby 3 : boolean
	}
}

get 2 {
	request {
		what 0 : string
	}
	response {
		type 0 : string
		result 1 : string
	}
}

set 3 {
	request {
		what 0 : string
		value 1 : string
	}
}

quit 4 {}

forward 5 {
	request {
		what 0 : string
		who 1 : *integer #empty is forwarded to all except self, otherwise specific the agent indexes.
	}
	response {
		type 0: string
		result 1 : string
	}
}

.movement {
		location 0 : *integer #position(float,3*4bytes,transimit as int)
		rotation 1 : *integer #rotation(float,3*4bytes,transimit as int)
		scale 2 : *integer #scale(float,3*4bytes,transimit as int)
		velocity 3 : *integer #velocity(float,3*4bytes,transimit as int)
		animation 4 : string #animation name
}

forwardmovement 6 {
	request {
		role 0 : string
		data 1 : *movement #data array.
		who 2 : *integer #empty is forwarded to all except self, otherwise specific the agent indexes.
	}
}

queryuserid 7 {
	request {
		agent_index 0: integer #specific the agent index of the querying user.
	}
	response {
		type 0 : string
		userid 1 : string #echo the user id to the request client.
		userfound 2 : boolean # true if found
	}
}

queryonlineuser 8 {
	request {
	}
	response {
		type 0 : string
		userids 1 : *string #other users except the request self.
	}
}

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
	stage 4 : string #specific the stage of the member. --r:ready,nr:not ready,gc:game connected,gd:game disconnected.
	exp_gain 5 : integer #current gain experience
	time_spend 6 : integer #current time spend
}

.lobby {
		lobbyname 0 : string
		stage 1 : integer #may be updated.
		averability 2 : integer #for matching,clients should join in the most-close-averablility lobby in the auto matching mode.
		members 3 : *lobbymember #may be updated at different time
}

querylobby 9 {
	request {
	}
	response {
		type 0 : string
		lobbyinstance 1 : *lobby #all created lobbies.
	}
}

createlobby 10 {
	request {
		attr 0 : lobby #lobby should be in different names.
	}
	response {
		type 0 : string
		succeed 1 : boolean #true to indicate ok.
	}
}

updatelobby 11 {
	request {
		lobbyname 0 : string #specific the lobby to update.
		stage 1 : integer #stage to update. update by the host.
		averability 2 : integer #averability need to be updated.
	}
	response {
		type 0 : string
		succeed 1 : boolean #true to indicate upate ok.
	}

}

joinlobby 12 {
	request {
		lobbyname 0 : string #specific the lobby to join in,if the name is 'matchup',then will find a matched lobby in the server.
		who 1 : lobbymember #client to join in,actions will be empty at this stage.
	}
	response {
		type 0 : string
		succeed 1 : boolean #true to indicate join ok.
		lobbyinstance 2 : lobby #response the joined lobby.
	}
}

assignmemberaction 13 {
	request {
		lobbyname 0 : string #specific the lobby holder.
		user_id 1 : string #owner.
		actions 2 : *lobbytaskaction
	}
	response {
		type 0 : string
		succeed 1 : boolean #true to indicate assign ok.
	}
}

updatememberaction 14 {
	request {
		lobbyname 0 : string #specific the lobby holder
		user_id 1 : string #specific the user id of this lobby
		action 2 : lobbytaskaction # specific the updating action,should be forwarded to other members in this lobby.once per time.
	}
	#response {
	#	type 0 : string
	#	succeed 1 : boolean #true to indicate update ok.
	#}#don' t need to respond.
}

leavelobby 15 {
	request {
		lobbyname 0 : string #specific the lobby to leave.if the host leaved,the host will automatically transimit to the next member in this lobby,if there's no member in it,lobby will be destroyed from the server.can be empty,if it is emtpy,will leave any lobbies that matched to the specific user_id.
		user_id 1 : string #specific the user to leave.
	}
}

updatememberstage 16 {
	request {
		lobbyname 0 : string #specific the lobby to be ready.
		user_id 1 : string #specific the user member
		stage 2 : string 
	}
}

forwardbyuserid 17 {
	request {
		what 0 : string
		who 1 : *string #empty is forwarded to all except self, otherwise specific the string user-ids.
	}
	response {
		type 0: string
		result 1 : string
	}
}


forwardmovementbyuserid 18 {
	request {
		role 0 : string
		data 1 : *movement #data array.
		who 2 : *string #empty is forwarded to all except self, otherwise specific the user-ids.
	}
}

.memberattr{
	attr_name 0 : string
	attr_value 1 : string
}

updatememberattribute 19 {
	request {
		lobbyname 0 : string #specific the lobby to be ready.
		user_id 1 : string #specific the user member
		attribute 2 : *memberattr #attributes may be a lot.
	}
}
]]

proto.s2c = sprotoparser.parse [[
.package {
	type 0 : integer
	session 1 : integer
}

heartbeat 1 {}

recieve 2 {
	request {
		what 0 : string
		from 1 : integer
		uid 2 : string #as well as echo the user id of this client.
	}
}

.movement {
		location 0 : *integer #position(float,3*4bytes,transimit as int)
		rotation 1 : *integer #rotation(float,3*4bytes,transimit as int)
		scale 2 : *integer #scale(float,3*4bytes,transimit as int)
		velocity 3 : *integer #velocity(float,4bytes,transimit as int)
		animation 4 : string #animation name
}

recievemovement 3 {
	request {
		role 0 : string
		data 1 : *movement
	}
}

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
	stage 4 : string #specific the stage of the member. --r:ready,nr:not ready,gc:game connected,gd:game disconnected.
	exp_gain 5 : integer #current gain experience
	time_spend 6 : integer #current time spend
}

.lobby {
		lobbyname 0 : string
		stage 1 : integer #may be updated.
		averability 2 : integer #for matching,clients should join in the most-close-averablility lobby in the auto matching mode.
		members 3 : *lobbymember #may be updated at different time
}

recievelobbychanged 4 {
	request {
		lobbyinstance 0 : lobby #echo to the request client.
	}
}

recievehostchanged 5 {
	request {
		sethost 0 : boolean #specific the new host client.
	}
}

recievegameready 6 {
	request {#ask the clients whether it is ready or not.
		lobbyinstance 0 : lobby #response the current lobby.
	}
}

recievegamelaunch 7 {
	request {#notify the clients that it should launch on the game.
		lobbyinstance 0 : lobby #response the current lobby.
	}
}

]]

return proto
