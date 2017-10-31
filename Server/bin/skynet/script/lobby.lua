local skynet = require "skynet"
require "skynet.manager"	-- import skynet.register

--local lobbyaction = {
--	task_id = 0,
--	item_id = 0,
--	action_id = 0,
--	action_status = 0
--}

--local lobbymember = {
--	user_id = 0,
--  agent_id = 0,--server side only
--	is_host = false,
--  stage = "nr" --r:ready,nr:not ready,gc:game connected,gd:game disconnected.
--	ability = 0.0,
--  exp_gain = 0,
--  time_spend = 0,
--	actions = {}
--}

local attribute = {
	lobbyname = "",
	stage = 0,
	averability = 0,
	members = {}
}

local CMD = {}

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

function CMD.getattribute()
	return attribute
end

function CMD.getaverability()
	print("lobby.getaverability",attribute.averability)
	return attribute.averability
end

function CMD.getlobbystage()
	return attribute.stage
end

function CMD.getlobbyname()
	return attribute.lobbyname
end

function CMD.setlobbystage(stage)
	attribute.stage = stage
end

function CMD.setlobbyaverability(averability)
	attribute.averability = averability
end

function CMD.acceptattribute(attr)
	attribute = attr
	--print_table(attr)
end

function CMD.gethostmember()
	for k,v in pairs(attribute.members) do 
		if true == v.is_host then
			return v
		end
	end
	return nil
end

function CMD.ishost(uid)
	for k,v in pairs(attribute.members) do 
		if uid == v.user_id then
			return v.is_host
		end
	end
	return false
end

function CMD.setashost(uid)
	for k,v in pairs(attribute.members) do 
		if uid == v.user_id then
			v.is_host = true
			return true
		end
	end
	return false
end

local prelockstage = 0
function CMD.shouldlock(lock)
	if true == lock then
		prelockstage = attribute.stage
		attribute.stage = 10
	else
		attribute.stage = prelockstage
	end
end

function CMD.hasmember()
	return #attribute.members > 0
end


function CMD.getmember(uid)
	--print("getmember",uid)
	for k,v in pairs(attribute.members) do
		local u = v.user_id
		if nil ~= u and u == uid then
			return v
		end
	end
	return nil
end

function CMD.getmemberbyagentid(agentid)
	for k,v in pairs(attribute.members) do
		local a = v.agent_id
		if nil ~= a and a == agentid then
			return v
		end
	end
	return nil
end

function CMD.getmembers()
	return attribute.members
end

function CMD.addmember(member)
	--print(member.user_id)
	--print(CMD.getmember)
	--most members should be 5.
	if nil ~= CMD.getmember(member.user_id) then 
		return false --already exist
	end
	table.insert(attribute.members,member)
	print("member add~>",member.user_id,#attribute.members)
	--print_table(attribute)
	--print("table.insert done meber add.")
	return true
end

function CMD.acceptmemberactions(member)
	--print_table(attribute.members)
	for k,v in pairs(attribute.members) do
		if v.user_id == member.user_id then 
			v.actions = member.actions
			return true
		end
	end
	return false
end

function CMD.updatememberaction(member)
	--print_table(member)
	for k,v in pairs(attribute.members) do
		if v.user_id == member.user_id then 
			for ak,av in pairs(v.actions) do
				if av.task_id == member.action.task_id and av.item_id == member.action.item_id and av.action_id == member.action.action_id then
					av.action_status = member.action.action_status
				end
			end
			return true
		end
	end
	return false
end

function CMD.updatememberattribute(args)
	if nil == args.user_id or nil == args.attribute then
		return
	end
	for k,v in pairs(attribute.members) do
		if v.user_id == args.user_id then
			for _k,_v in pairs(args.attribute) do
				v[_v.attr_name] = _v.attr_value--copy the attribute.
			end
			return
		end
	end
end

function CMD.updatememberagentid(param)
	if nil == param then 
		return true
	end
	for k,v in pairs(attribute.members) do
		if param.user_id == v.user_id then
			local a = skynet.call(param.wd,"lua","findagent",v.user_id)
			if nil ~= a then
				v.agent_id = a
				print("lobby.updatememberagentid",v.user_id,v.agent_id)
				return true
			end
			return false
		end
	end
	return false
end

function CMD.checkallmembersready()
	for k,v in pairs(attribute.members) do
		if v.stage ~= "r" then
			return false
		end
	end
	return true
end

function CMD.updatememberstage(member)
	for k,v in pairs(attribute.members) do
		if v.user_id == member.user_id then
			v.stage = member.stage
			return true
		end
	end
	return false
end

function CMD.findaction(fk)
	if nil == fk.uid or nil == fk.tid or nil == fk.iid or nil == fk.aid then 
		return nil
	end
	local member = CMD.getmember(fk.uid)
	if nil == member then 
		return nil
	end
	for k,v in pairs(member) do
		if v.task_id == fk.tid and v.item_id == fk.iid and v.action_id == fk.aid then
			return v
		end
	end
	return nil
end

function CMD.removemember(uid)
	--print("CMD.removemember.before")
	--print_table(attribute)
	local find_user = false
	for k,v in pairs(attribute.members) do
		if v.user_id == uid then 
			table.remove(attribute.members,k)
			find_user = true
			--print("member removed~>",uid,attribute.lobbyname)
			--print("CMD.removemember.after")
			--print_table(attribute)
			break
		end
	end
	if true == find_user then--user's status should be reset to 'nr'.
		for k,v in pairs(attribute.members) do
			v.stage = 'nr'
		end
	end
	return find_user
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
