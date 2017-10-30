package.path = 'D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/?.lua'
require "paflua"
local nps_name = require "nps_name"
paf.pafcore.System.LoadDLL("LuaNamepipeServer.dll")
local nps = paf.Namepipe.Server()
local nps_mpn = nps:InitMainNamepipe("\\\\.\\pipe\\"..nps_name)
os.execute("ping -n -1 127.0.0.1>nul")

--/b
--""D:/Program Files (x86)/HTTPD/htdocs/TrainMobileFile/LuaCode/namepipe/nps.lua""