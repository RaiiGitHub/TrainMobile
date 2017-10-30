Dim ws
Set ws = CreateObject("Wscript.Shell")
ws.run "cmd /c skynetexit.vbs",vbhide,true
'Wscript.sleep 500
ws.run "cmd /c skynetrun.vbs",vbhide,true
Set ws = Nothing
Wscript.quit