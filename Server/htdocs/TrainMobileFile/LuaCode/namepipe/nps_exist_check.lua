require "paflua"
paf.pafcore.System.LoadDLL("LuaNamepipeServer.dll")
local nps = paf.Namepipe.Server()
local nps_mpn = nps:CheckServerProcessExist("apachenamepipeserver.exe")._
--print(tostring(nps_mpn))
return nps_mpn