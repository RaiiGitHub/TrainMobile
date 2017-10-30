Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.popup "Train mobile server has been shutdowned succeessfully",2,"shutdown"
ws.run "cmd /c TASKKILL /F /IM apachenamepipeserver.exe /T",vbhide
ws.run "cmd /c TASKKILL /F /IM skynet.exe /T",vbhide
ws.run "cmd /c TASKKILL /F /IM trainmobileserver.exe /T",vbhide
Set ws = Nothing
Wscript.quit