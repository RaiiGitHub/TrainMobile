local sprotolib = {}

local proto = require "proto"
local sproto = require "sproto"
--local ue4lua = qianqian

sprotolib.host = sproto.new(proto.s2c):host "package"
sprotolib.request = sprotolib.host:attach(sproto.new(proto.c2s))
sprotolib.last = ""

local CALLREQUEST = {}
local CALLRESPONSE = {}

function CALLREQUEST:heartbeat()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_heartbeat()
	end
end

function CALLREQUEST:recieve()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_message_recieve(self.what,#self.what,self.uid,#self.uid,self.from)
	end
end

function CALLREQUEST:recievemovement()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_movement_recieve(self.role,#self.data,self.data)
	end
end

function CALLREQUEST:recievelobbychanged()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_lobbychanged_recieve(self.lobbyinstance)
	end
end

function CALLREQUEST:recievehostchanged()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_hostchanged_recieve(self.sethost)
	end
end

function CALLREQUEST:recievegameready()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_game_ready_recieve(self.lobbyinstance)
	end
end

function CALLREQUEST:recievegamelaunch()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_game_launch_recieve(self.lobbyinstance)
	end
end

function CALLRESPONSE:get()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_get_response(self.result,#self.result)
	end
end

function CALLRESPONSE:handshake()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_handshake(self.msg,#self.msg,self.agent_index,self.inlobby)
	end
end

function CALLRESPONSE:queryuserid()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_queryuid_response(self.userid,#self.userid,self.userfound)
	end
end

function CALLRESPONSE:queryonlineuser()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_queryonlineuser_response(self.userids)--is a table.
	end
end

function CALLRESPONSE:querylobby()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_querylobby_response(self.lobbyinstance)--is a table(with all lobbies.)
	end
end

function CALLRESPONSE:createlobby()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_createlobby_response(self.succeed)--indicate that whether succeed or not.
	end
end

function CALLRESPONSE:updatelobby()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_updatelobby_response(self.succeed)--indicate that whether succeed or not.
	end
end

function CALLRESPONSE:joinlobby()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_joinlobby_response(self.succeed,self.lobbyinstance)--as well as return the joined-lobby's current-full-information.
	end
end

function CALLRESPONSE:assignmemberaction()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_assignmemberaction_response(self.succeed)--indicate that whether succeed or not.
	end
end

function CALLRESPONSE:updatememberaction()
	if nil ~= ue4lua then
		ue4lua.sproto_fill_updatememberaction_response(self.succeed)--indicate that whether succeed or not.
	end
end


local function callrequest(name, args)
	local f = assert(CALLREQUEST[name])
	local r = f(args)
end

local function callresponse(session, args)
	local f = assert(CALLRESPONSE[args.type])
	local r = f(args)
end

function sprotolib.pack_package(pack)
	local package = string.pack(">s2", pack)
	return package,#package
end

function sprotolib.unpack_package(text)
	local size = #text
	if size < 2 then
		return nil, text
	end
	--<<8+low as size.
	local s = text:byte(1) * 256 + text:byte(2)
	if size < s+2 then
		return nil, text
	end
	return text:sub(3,2+s), text:sub(3+s)
end

function sprotolib.recv_package(r)
	local process = sprotolib.last .. r
	while true do
		local result
		result, sprotolib.last = sprotolib.unpack_package(process)
		if result then
			sprotolib.call_package(sprotolib.host:dispatch(result))
			process = sprotolib.last
		else
			return
		end
	end
end

function sprotolib.call_package(t, ...)
	if t == "REQUEST" then
		return pcall(callrequest, ...)
	else
		assert(t == "RESPONSE")
		return pcall(callresponse, ...)
	end
end

function sprotolib.dispatch_package(r)
	if (not r) or r == "" then
		return
	end
	sprotolib.recv_package(r)
end

return sprotolib

--pack_package("handshake")
--pack_package("set", { what = "hello", value = "world" })


