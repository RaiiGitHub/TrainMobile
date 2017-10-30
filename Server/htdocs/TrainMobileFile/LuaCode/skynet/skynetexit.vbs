Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run "cmd /c TASKKILL /F /IM skynet.exe /T",vbhide
Set ws = Nothing
Wscript.quit