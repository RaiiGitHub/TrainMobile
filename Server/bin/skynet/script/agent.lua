local skynet = require "skynet"
local netpack = require "netpack"
local socket = require "socket"
local sproto = require "sproto"
local sprotoloader = require "sprotoloader"

local WATCHDOG
local host
local send_request

local CMD = {}
local REQUEST = {}
local client_fd
local user_id = "defaultuser"

function print_table(t)
	for k,v in pairs(t) do
		if type(v) == "table" then
			print("sub table met.",k,#v)
			print_table(v)
		else
			print(k,v)
		end
	end
end	

function REQUEST:get()
	print("get", self.what)
	local r = skynet.call("FORWARDSERVICE", "lua", "getmsg", self.what)
	return { type="get",result = r }
end

function REQUEST:set()
	print("set", self.what, self.value)
	local r = skynet.call("FORWARDSERVICE", "lua", "setmsg", self.what, self.value)
end

function REQUEST:handshake()
    user_id = self.userid
	print("agent.handshake",user_id)
	--may at the condiftion of client reconnecting
	local _inlobby = skynet.call("LOBBYSERVICE","lua","updateclientagentidwhenreconnecting",user_id)
	return { type="handshake",msg = "Welcome to skynet.您好，这里是天网。こんにちは、ここは天網です。",agent_index=client_fd,inlobby=_inlobby }
end

function REQUEST:quit()
	skynet.call(WATCHDOG, "lua", "close", client_fd)
end

function REQUEST:forward()
    local towhom = ""
	for k,v in pairs(self.who) do
		towhom = v..","..towhom
	end
	print("agent.forward",self.what,towhom)
	skynet.call("FORWARDSERVICE","lua","forward",{content={self.what,self.who},from=client_fd,uid=user_id})
	return {type="forward",result='forwarded.'}
end

function REQUEST:forwardbyuserid()
    local towhom = ""
	local who = {}
	for k,v in pairs(self.who) do
		towhom = v..","..towhom
		local a = skynet.call(WATCHDOG,"lua","findagent",v)
		if nil ~= a then
			local i = skynet.call(a,"lua","getagentindex")
			if nil ~= i then
				table.insert(who,i)
				towhom = towhom.."("..i..")"..","
			end
		end
	end
	print("agent.forwardbyuserid",self.what,towhom)
	skynet.call("FORWARDSERVICE","lua","forward",{content={self.what,who},from=client_fd,uid=user_id})
	return {type="forward",result='forwarded.'}
end

function REQUEST:forwardmovement()
    local towhom = ""
	for k,v in pairs(self.who) do
		towhom = v..","..towhom
	end
	--print("agent.forwardmovement~>",self.role,towhom)
	skynet.call("FORWARDSERVICE","lua","forwardmovement",{content={{role=self.role,data=self.data},self.who},from=client_fd})
end

function REQUEST:forwardmovementbyuserid()
	local towhom = ""
  	local who = {}
	for k,v in pairs(self.who) do
		towhom = v..","..towhom
		local a = skynet.call(WATCHDOG,"lua","findagent",v)
		if nil ~= a then
			local i = skynet.call(a,"lua","getagentindex")
			if nil ~= i then
				table.insert(who,i)
				towhom = towhom.."("..i..")"..","
			end
		end
	end
	--print("agent.forwardmovementbyuserid~>",self.role,towhom)
	skynet.call("FORWARDSERVICE","lua","forwardmovement",{content={{role=self.role,data=self.data},who},from=client_fd})
end

function REQUEST:queryuserid()
    local agents = skynet.call(WATCHDOG,"lua","agents")
	local query_agent = agents[self.agent_index]
	print("queryuserid~>agent_index",self.agent_index)
	if nil ~= query_agent then 
		local uid = skynet.call(query_agent,"lua","getuserid")
		return { type="queryuserid",userid = uid,userfound = true }
	end
	return { type="queryuserid",userid = "",userfound = false }
end

function REQUEST:queryonlineuser()
	--return all uses' id except self
	local agents = skynet.call(WATCHDOG,"lua","agents")
	local users = {}
	for k,v in pairs(agents) do
		if k ~= client_fd then  
			local uid = skynet.call(v,"lua","getuserid")
			table.insert(users,uid)
		end
	end
	return {type="queryonlineuser",userids=users}
end

function REQUEST:querylobby()
	local lobbies = skynet.call("LOBBYSERVICE","lua","getlobbies")
	local instances = {}
	for k,v in pairs(lobbies) do
		local attr = skynet.call(v,"lua","getattribute")
		if nil ~= attr then
			table.insert(instances,attr)
		end
	end
	return {type="querylobby",lobbyinstance=instances}
end 

function REQUEST:createlobby()
	local result = skynet.call("LOBBYSERVICE","lua","createlobby",self.attr)
	return result
end

--note:
--if there's reponse in the request,you'd better gaurantee that you will not send other message to the requesting client anymore,otherwise the client may probably not recieve it.
--note that you shouldn't respond more than 2 to a same client in a request.
function REQUEST:updatelobby()
	local result = skynet.call("LOBBYSERVICE","lua","updatelobby",{
		lobbyname = self.lobbyname,
		stage = self.stage,
		averability = self.averability
	})
	if true == result.succeed then
		if 10 == self.stage then
			--lobby is locked manually.server should dispatch this message to other members in this lobby.
			skynet.call("LOBBYSERVICE","lua","notifyclientstobeready",{lobbyname=self.lobbyname,uid = user_id})
		end
	end
	return result
end

function REQUEST:joinlobby()
	local result = skynet.call("LOBBYSERVICE","lua","joinlobby",{
		lobbyname = self.lobbyname,
		member = self.who
	})
	--print("REQUEST:joinlobby->",self.lobbyname,self.who.user_id,result.succeed)
	if true == result.succeed then
		--members count limit,current is up to 5.
		--forward.
		skynet.call("LOBBYSERVICE","lua","forwardlobbyattr",{
			lobbyname = result.lobbyinstance.lobbyname,
			user_id = self.who.user_id
		})
		local memnum = #result.lobbyinstance.members
		if 5 == memnum then
			--lock this lobby and send a response to the clients.
			skynet.call("LOBBYSERVICE","lua","locklobby",result.lobbyinstance.lobbyname)
			skynet.call("LOBBYSERVICE","lua","notifyclientstobeready",{lobbyname=result.lobbyinstance.lobbyname,uid=user_id})
		end
	end
	return result
end

function REQUEST:assignmemberaction()
	local result = skynet.call("LOBBYSERVICE","lua","assignmemberaction",{
		lobbyname = self.lobbyname,
		user_id = self.user_id,
		actions = self.actions
	})
	if true == result.succeed then
		--forward.
		skynet.call("LOBBYSERVICE","lua","forwardlobbyattr",{
			lobbyname = self.lobbyname,
			user_id = self.user_id
		})
	end
	return result 
end

function REQUEST:updatememberaction()
	local result = skynet.call("LOBBYSERVICE","lua","updatememberaction",{
		lobbyname = self.lobbyname,
		user_id = self.user_id,
		action = self.action
	})
	if true == result.succeed then
		--forward.
		--print("updatememberaction")
		skynet.call("LOBBYSERVICE","lua","forwardlobbyattr",{
			lobbyname = self.lobbyname,
			user_id = self.user_id
		})
	end
	--return result 
	--don't need to respond.
end

function REQUEST:updatememberattribute()
	local result = skynet.call("LOBBYSERVICE","lua","updatememberattribute",{
		lobbyname = self.lobbyname,
		user_id = self.user_id,
		attribute = self.attribute
	})
	if true == result.succeed then
		--forward.
		skynet.call("LOBBYSERVICE","lua","forwardlobbyattr",{
			lobbyname = self.lobbyname,
			user_id = self.user_id
		})
	end
end

function REQUEST:leavelobby()
	local result = skynet.call("LOBBYSERVICE","lua","leavelobby",{
		lobbyname = self.lobbyname,
		user_id = self.user_id
	})
	if 1 == result then
		skynet.call("LOBBYSERVICE","lua","hosttransmit",self.lobbyname)--will auto transmit to the user with the highest ability.
	end
	skynet.call("LOBBYSERVICE","lua","forwardlobbyattr",{
			lobbyname = self.lobbyname,
			user_id = self.user_id
		})
end

function REQUEST:updatememberstage()
	skynet.call("LOBBYSERVICE","lua","updatememberstage",{
		lobbyname = self.lobbyname,
		user_id = self.user_id,
		stage = self.stage
	})
	if self.stage == "r" then
		--check whether all members are ready
		if true == skynet.call("LOBBYSERVICE","lua","checkallmembersready",self.lobbyname)  then
			skynet.call("LOBBYSERVICE","lua","notifyclientstolaunch",self.lobbyname)
			return
		end
	end
end

local function request(name, args, response)
	local f = assert(REQUEST[name])
	local r = f(args)
	if response then
		return response(r)
	end
end

local function send_package(pack)
	local package = string.pack(">s2", pack)
	socket.write(client_fd, package)
end


skynet.register_protocol {
	name = "client",
	id = skynet.PTYPE_CLIENT,
	unpack = function (msg, sz)
		return host:dispatch(msg, sz)
	end,
	dispatch = function (_, _, type, ...)
		if type == "REQUEST" then
			local ok, result  = pcall(request, ...)
			if ok then
				if result then
					send_package(result)
				end
			else
				skynet.error(result)
			end
		else
			assert(type == "RESPONSE")
			error "here doesn't support respond client"
		end
	end
}

function CMD.start(conf)
	local fd = conf.client
	local gate = conf.gate
	WATCHDOG = conf.watchdog
	-- slot 1,2 set at main.lua
	local sp1 = sprotoloader.load(1)
	local sp2 = sprotoloader.load(2)
	host = sp1:host "package"
	send_request = host:attach(sp2)

	skynet.fork(function()
		while true do
			send_package(send_request "heartbeat")
			skynet.sleep(500)
		end
	end)

	client_fd = fd
	skynet.call(gate, "lua", "forward", fd)
end

function CMD.disconnect()
	-- todo: do something before exit
	skynet.exit()
end

function CMD.forward(args)
	send_package(send_request("recieve",args))
end

function CMD.forwardmovement(args)
	send_package(send_request("recievemovement",args))
end

function CMD.forwardlobbychanged(args)
	send_package(send_request("recievelobbychanged",args))
end

function CMD.forwardhostchanged(args)
	send_package(send_request("recievehostchanged",args))
end

function CMD.notifygameready(args)
	send_package(send_request("recievegameready",args))
end

function CMD.notifygamelaunch(args)
	send_package(send_request("recievegamelaunch",args))
end

function CMD.getuserid()
	return user_id
end

function CMD.getagentindex()
	return client_fd
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
