Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\launch-train_mobile-socket-server.lnk""",vbhide
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\autogenip.lnk""",vbhide
Set ws = Nothing
Wscript.quit