local skynet = require "skynet"
require "skynet.manager"	-- import skynet.register

local lobbies = {}
local WATCHDOG
local command = {}

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

function command.START(conf)
	WATCHDOG = conf.WATCHDOG
	print("lobbyservice ready...")
end

function command.GETLOBBIES()
	return lobbies
end

function command.MATCHLOBBY(ability)
    --lobby should not be locked. 
	local index = 0
	local ability_near = 0
	local ability = 0
	--print("lobbyservice.MATCHLOBBY.lobby count",#lobbies)
	for k,v in pairs(lobbies) do
		local stage = skynet.call(v,"lua","getlobbystage")
		if 10 ~= stage and 9 ~= stage then
			local aver_a = skynet.call(v,"lua","getaverability")
			--print("lobbyservice.MATCHLOBBY.aver_a",aver_a)
			if ability == 0  then
				ability = aver_a
			end
			local _near = math.abs(ability - aver_a)
			if ability_near <= _near then
				index = k
				ability_near = _near
			end
		end
	end
	return 0 == index and nil or lobbies[index]--may be nil
end

function command.GETLOBBY(lobbyname)
	for k,v in pairs(lobbies) do
		local _attr = skynet.call(v,"lua","getattribute")
		if nil ~= _attr and _attr.lobbyname == lobbyname then
			return v
		end
	end
	return nil
end

function command.GETLOBBYBYUID(uid)
	for k,v in pairs(lobbies) do
		local m = skynet.call(v,"lua","getmember",uid)
		if nil ~= m then
			return v
		end
	end
	return nil
end

function command.CREATELOBBY(attr)
	if nil == attr then 
		return
	end
	local result = {type="createlobby",succeed=false}
	if nil ~= command.GETLOBBY(attr.lobbyname) then
		return result --already exist.
	end
	local author = attr.members[1]
	if nil == author then
		print("lobbyservice.createlobby,should specific the author.")
		return result
	end
	result.succeed = true
	local newlb = skynet.newservice("lobby")
	table.insert(lobbies,newlb)
	skynet.call(newlb,"lua","acceptattribute",attr)
	skynet.call(newlb,"lua","updatememberagentid",{user_id=author.user_id,wd=WATCHDOG})
	print("lobby",attr.lobbyname," created.")
	print_table(attr)
	return result
end


function command.UPDATELOBBY(attr)
	local result = {type="updatelobby",succeed = false}
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		print(attr.lobbyname,"not found.[lobbyservice.UPDATELOBBY]")
		return result
	end
	skynet.call(l,"lua","setlobbystage",attr.stage)
	skynet.call(l,"lua","setlobbyaverability",attr.averability)
	result.succeed = true
	return result
end

function command.JOINLOBBY(attr)
	local result = {type="joinlobby",succeed = false,lobbyinstance={}}
	local l = nil
	if attr.lobbyname == "matchup" then
		--match current lobby.
		l = command.MATCHLOBBY(attr.member.ability)
	else
		l = command.GETLOBBY(attr.lobbyname)
	end

	if nil == l then
		return result
	end

	local _member_add_ok = skynet.call(l,"lua","addmember",attr.member)
	if false == _member_add_ok then
		return result
	end
	skynet.call(l,"lua","updatememberagentid",{user_id=attr.member.user_id,wd=WATCHDOG})
	result.succeed = true
	result.lobbyinstance = skynet.call(l,"lua","getattribute")
	return result --this result need to be forwarded.
end

function command.ASSIGNMEMBERACTION(attr)
	--print("lobbyservice.assignmemberaction",attr.lobbyname)
	local result = {type="assignmemberaction",succeed = false}
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		print("lobby not found.")
		return result
	end
	if true == skynet.call(l,"lua","acceptmemberactions",{
		user_id = attr.user_id,
		actions = attr.actions
	}) then
		result.succeed = true
	end
	return result--this result need to be forwarded.
end

function command.UPDATEMEMBERACTION(attr)
	local result = {type="updatememberaction",succeed = false}
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		return result
	end
	if true == skynet.call(l,"lua","updatememberaction",{
		user_id = attr.user_id,
		action = attr.action
	}) then
		result.succeed = true
	end
	return result--this result need to be forwarded.
end

function command.UPDATEMEMBERATTRIBUTE(attr)
	local result = {type="updatememberattribute",succeed = false}
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		return result
	end
	if true == skynet.call(l,"lua","updatememberattribute",{
		user_id = attr.user_id,
		attribute = attr.attribute
	}) then
		result.succeed = true
	end
	return result--this result need to be forwarded.
end


function command.UPDATEMEMBERSTAGE(attr)
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		return
	end
	skynet.call(l,"lua","updatememberstage",{
		user_id = attr.user_id,
		stage = attr.stage
	}) 
	print("lobbyserver.updatememerstage->",attr.lobbyname,attr.user_id,attr.stage)
end

function command.UPDATELOBBYSTAGE(attr)
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		return
	end
	skynet.call(l,"lua","setlobbystage", attr.stage) 
	print("lobbyserver.updatelobbystage->",attr.lobbyname,attr.stage)
end

function command.CHECKALLMEMBERSREADY(lbname)
	local l = command.GETLOBBY(lbname)
	if nil == l then
		return false
	end
	return skynet.call(l,"lua","checkallmembersready") 
end

function command.LEAVELOBBY(attr)
	--return 0:don't need to forward.1:need to (forward host transmiting.)
	local l = command.GETLOBBY(attr.lobbyname)
	if nil == l then
		l = command.GETLOBBYBYUID(attr.user_id)
		if nil == l then
			return 0
		end
	end
	local ishost = skynet.call(l,"lua","ishost",attr.user_id)
	if true == skynet.call(l,"lua","removemember",attr.user_id) then
		if false == skynet.call(l,"lua","hasmember") then
			for k,v in pairs(lobbies) do
				if v == l then
					table.remove(lobbies,k)--remove!
					local lbname = skynet.call(v,"lua","getlobbyname")
					print("lobby shutdown.",lbname)
					return 0
				end
			end
		else
			--auto transmit if the removed one is the host.
			return true == ishost and 1 or 0
		end
	end
	return 0
end

function command.HANDLEMEMBERCONNECTION(agend_id)
	--if the stage of this user is 'nr', remove him from the lobby.others just change the stage of this member.
	--print("lobbyservice::HANDLEMEMBERCONNECTION",agend_id)
	for k,v in pairs(lobbies) do
		local m = skynet.call(v,"lua","getmemberbyagentid",agend_id)
		local ln = skynet.call(v,"lua","getlobbyname")
		if nil ~= m then
			if m.stage == 'nr' then
				local result = command.LEAVELOBBY({
						lobbyname = ln,
						user_id = m.user_id
						})
				if 1 == result then
					command.HOSTTRANSMIT(ln)
				end
			else
				m.stage = 'gd'--game disconnected.
			end
			command.FORWARDLOBBYATTR({
				lobbyname = ln,
				user_id = m.user_id
			})--forward
			return
		end
	end
end

function command.UPDATECLIENTAGENTIDWHENRECONNECTING(uid)
	--for client reconnecting
	for k,v in pairs(lobbies) do
		if true == skynet.call(v,"lua","updatememberagentid",{user_id=uid,wd=WATCHDOG}) then
			return true
		end
	end
	return false
end

function command.FORWARDLOBBYATTR(attr)
	--to all members except self.
	if nil == WATCHDOG or nil == attr then 
		return
	end
	
	local agents = skynet.call(WATCHDOG,"lua","agents")
	local lbname = attr.lobbyname
	local fromuid = attr.user_id

	local lb = command.GETLOBBY(lbname)
	if nil == lb then
		return
	end

	local _attr = skynet.call(lb,"lua","getattribute")
	if nil == _attr then 
		return 
	end

	local members = _attr.members
	if nil == members then 
		return
	end

	--print("command.FORWARDLOBBYATTR.averability",_attr.averability)
	--print_table(_attr)
	for k,v in pairs(members) do
		if v.user_id ~= fromuid then
			local a = skynet.call(WATCHDOG,"lua","findagent",v.user_id)
			if nil ~= a then
				skynet.call(a,"lua","forwardlobbychanged",{lobbyinstance=_attr})
			end
		end
	end
end

function command.HOSTTRANSMIT(lbname)
	if nil == lbname then
		return
	end
	print("lobbyservice.HOSTTRANSMIT~>",lbname)
	local lb = command.GETLOBBY(lbname)
	if nil == lb then
		return
	end

	local attr = skynet.call(lb,"lua","getattribute")
	local max_ability_index = 0
	local max_ability = 0
	for k,v in pairs(attr.members) do
		if v.ability > max_ability then
			max_ability = v.ability
			max_ability_index = k
		end
	end
	
	if 0 ~= max_ability_index then
		local uid = attr.members[max_ability_index].user_id
		if true == skynet.call(lb,"lua","setashost",uid) then
			local a = skynet.call(WATCHDOG,"lua","findagent",uid)
			if nil ~= a then
				skynet.call(a,"lua","forwardhostchanged",{sethost = true})
			end
		end
	end
end

function command.LOCKLOBBY(lbname)
	if nil == lbname then
		return
	end
	local lb = command.GETLOBBY(lbname)
	if nil == lb then
		return
	end
	skynet.call(lb,"lua","shouldlock",true)
end

function command.NOTIFYCLIENTSTOBEREADY(param)
	if nil == param then
		return
	end
	local lbname = param.lobbyname
	local uid = param.uid
	local lb = command.GETLOBBY(lbname)
	if nil == lb then
		return
	end

	local attr = skynet.call(lb,"lua","getattribute")
	if nil == attr or nil == attr.members then 
		return
	end

	for k,v in pairs(attr.members) do
		if uid ~= v.user_id then
			local a = skynet.call(WATCHDOG,"lua","findagent",v.user_id)
			if nil ~= a then
				skynet.call(a,"lua","notifygameready",{lobbyinstance=attr})
			end
		end
	end
	print(lbname,"is ready,dispatch to the memebrs.")
end

function command.NOTIFYCLIENTSTOLAUNCH(lbname)
	if nil == lbname then
		return
	end

	local lb = command.GETLOBBY(lbname)
	if nil == lb then
		return
	end

	local attr = skynet.call(lb,"lua","getattribute")
	if nil == attr then 
		return
	end

	for k,v in pairs(attr.members) do
		local a = skynet.call(WATCHDOG,"lua","findagent",v.user_id)
		if nil ~= a then
			skynet.call(a,"lua","notifygamelaunch",{lobbyinstance=attr})
		end
	end

	print(lbname,"can be launched on,dispatch to the memebrs.")
end


skynet.start(function()
	skynet.dispatch("lua", function(session, address, cmd, ...)
		local f = command[string.upper(cmd)]
		if f then
			skynet.ret(skynet.pack(f(...)))
		else
			error(string.format("Unknown command %s", tostring(cmd)))
		end
	end)
	skynet.register "LOBBYSERVICE"
end)
