Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run "cmd /c TASKKILL /F /IM apachenamepipeserver.exe /T",vbhide
Set ws = Nothing
Wscript.quit