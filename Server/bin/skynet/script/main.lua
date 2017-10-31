local skynet = require "skynet"
local sprotoloader = require "sprotoloader"
local max_client = 64

skynet.start(function()
	skynet.error("Server start")
	skynet.uniqueservice("protoloader")
	if not skynet.getenv "daemon" then
		local console = skynet.newservice("console")
	end
	--skynet.newservice("debug_console",8181)
	local watchdog = skynet.newservice("watchdog")
	skynet.call(watchdog, "lua", "start", {
		port = 5060,
		maxclient = max_client,
		nodelay = true,
	})
	local fs = skynet.newservice("forwardservice")
	skynet.call(fs,"lua","start",{
		WATCHDOG = watchdog
	})
	local ls = skynet.newservice("lobbyservice")--an entry point of lobbyservice.
	skynet.call(ls,"lua","start",{
		WATCHDOG = watchdog
	})
	skynet.error("Watchdog listen on", 5060)
	skynet.exit()
end)
