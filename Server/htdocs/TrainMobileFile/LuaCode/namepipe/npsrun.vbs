Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run """D:\Program Files (x86)\HTTPD\htdocs\TrainMobileFile\LuaCode\namepipe\luanch_nps.lnk""",vbhide
Set ws = Nothing
Wscript.quit