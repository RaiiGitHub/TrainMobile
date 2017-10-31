Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run "cmd /c npsexit.vbs",vbhide,true
ws.run "cmd /c npsrun.vbs",vbhide,true
Set ws = Nothing
Wscript.quit