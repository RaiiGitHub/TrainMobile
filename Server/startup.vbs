Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.popup "Train mobile server has been startuped succeessfully",2,"startup"
ws.run "cmd /c TASKKILL /F /IM apachenamepipeserver.exe /T",vbhide,true
ws.run "cmd /c TASKKILL /F /IM skynet.exe /T",vbhide,true
ws.run "cmd /c TASKKILL /F /IM trainmobileserver.exe /T",vbhide,true
ws.run """D:\Program Files (x86)\HTTPD\bin\trainmobileserver.lnk""",vbhide
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\autogenip.lnk""",vbhide
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\skynet\autorestartskynet.lnk""",vbhide
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\namepipe\autorestartnps.lnk""",vbhide
Set ws = Nothing
Wscript.quit
