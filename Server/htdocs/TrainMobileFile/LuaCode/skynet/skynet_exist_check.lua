require "paflua"
paf.pafcore.System.LoadDLL("LuaNamepipeServer.dll")
local nps = paf.Namepipe.Server()
local nps_skynet = nps:CheckServerProcessExist("skynet.exe")._
--print(tostring(nps_mpn))
return nps_skynet