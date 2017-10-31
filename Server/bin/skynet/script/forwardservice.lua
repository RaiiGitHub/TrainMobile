local skynet = require "skynet"
require "skynet.manager"	-- import skynet.register

local WATCHDOG

local msgdb = {}

local command = {}

function command.GETMSG(key)
	return msgdb[key]
end

function command.SETMSG(key, value)
	local last = msgdb[key]
	msgdb[key] = value
	return last
end

function command.START(conf)
	WATCHDOG = conf.WATCHDOG
	print("forwardservice ready...")
end

--simple message forwading.
function command.FORWARD(data)
	if nil == WATCHDOG or nil == data then 
		return
	end
	local agents = skynet.call(WATCHDOG,"lua","agents")
	local _from = data.from
	local _uid = data.uid
	local _content = data.content
	if 2 ~= #_content then 
		return
	end
	local towhomcount = #_content[2]

	local revers_whom = {}
	for i=towhomcount,1,-1 do
		revers_whom[_content[2][i]] = true
	end

	if nil ~= _from and nil ~= _content then 
		for n,a in pairs(agents) do
			if n ~= _from then
				--forward!
				if 0 == towhomcount or nil ~= revers_whom[n] then
					--print("[from]:".._from..":need to forward~>"..n..","..a)
					skynet.call(a,"lua","forward",{what = _content[1],from = _from,uid=_uid})
				end
			end
		end
	end
end

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

--movement data forwading
function command.FORWARDMOVEMENT(data)
	if nil == WATCHDOG or nil == data then 
		return
	end

	local agents = skynet.call(WATCHDOG,"lua","agents")
	local from = data.from
	local content = data.content
	if 2 ~= #content then 
		return
	end
	local towhomcount = #content[2]

	local revers_whom = {}
	for i=towhomcount,1,-1 do
		revers_whom[content[2][i]] = true
	end

	--print_table(content[1])

	if nil ~= from and nil ~= content then 
		for n,a in pairs(agents) do
			if n ~= from then
				--forward!
				if 0 == towhomcount or nil ~= revers_whom[n] then
					--print("[from]:"..from..":need to forward movement~>"..n..","..a)
					skynet.call(a,"lua","forwardmovement",content[1])
				end
			end
		end
	end

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
	skynet.register "FORWARDSERVICE"
end)
