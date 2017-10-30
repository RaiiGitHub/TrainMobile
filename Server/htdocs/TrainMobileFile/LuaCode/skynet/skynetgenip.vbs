Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run "cmd /c ""D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\autoip.bat""",vbhide
ws.run """D:\Program Files (x86)\HTTPD\bin\apachenamepipeserver.exe"" ""D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\skynet_auto_set_ip.lua""",vbhide
Set ws = Nothing
Wscript.quit