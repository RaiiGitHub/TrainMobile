macroScript TS_UE4_ObjectExportTool category:"TS_Tools" tooltip:"Exports Objects to FBX for UE4" buttonText:"UE4 FBX Export"
(
	/* 
	Tom Shannon
	www.TomShannon3D.com
	
	Exports models to FBX one at a time. 
	Each FBX file will be named after the Max Scene name
	The models can be oriented to 0,0,0 on export (Rather than having them all stacked in MAX)
	You can export the position of the models to the clipboard for easy placing in UE4 by pasting the clipboard contents then replacing the placeholder objects in UE4 with the correct objects.
	
	Version 1.0 -	2014-07-22 : 	First version!
	Version 1.1 -	2014-09-07 : 	Collision features and some useability fixes, renamed
									NEW: Allows users to explicitly set the export directory. This stays saved in the max file permanently and will be reloaded
									NEW: Users can reset the saved export path to use the default export path
									NEW: Exports UCX collision geometry! Just ensure your naming is correct, and the script will detect the apropriate UCX primitives for export. Note: they do NOT need to be selected or visible!
	
									Fixed: Export Window now identifies errors and offers some helpful tootips
									Fixed: Export path textfield identifies when no export path is available
									Fixed: Interface updates when the max file is loaded, saved or reset, showing the correct paths, etc.
									Fixed: Disabled Geometry-specific export options when the object position is being exported (To help notify the user as to what mode they are in)
									Fixed: Checks to see if export path is valid and asks user if they want to create the export path if it doesn't exist, rather than just making one
	Version 1.1.1	2014-09-16 :	Fixed: Exporting objects now names objects correctly (Thanks, Maico G.!)
	*/
	
	try (destroyDialog UE4_Export_FBX_Rollout) catch ()
	
	local ExportType = "Geometry" --legacy variable
	local ExportObjects = #()
	local OriginalSelection = #()
	
	local defaultMeshPath = @"/Engine/EditorMeshes/EditorCube.EditorCube"
	
	--Strings for building things
	t3d_Header = 
"Begin Map
   Begin Level
"
t3d_StaticMesh_Entry = 
"      Begin Actor Class=StaticMeshActor Name=% Archetype=StaticMeshActor'/Script/Engine.Default__StaticMeshActor'
         Begin Object Class=StaticMeshComponent Name=StaticMeshComponent0 ObjName=StaticMeshComponent0 Archetype=StaticMeshComponent'/Script/Engine.Default__StaticMeshActor:StaticMeshComponent0'
         End Object
         Begin Object Name=StaticMeshComponent0
            StaticMesh=StaticMesh'/Game/%/%.%'
            RelativeLocation=(X=%,Y=%,Z=%)
            RelativeScale3D=(X=%,Y=%,Z=%)
            RelativeRotation=(Pitch=%,Yaw=%,Roll=%)
            CustomProperties 
         End Object
         StaticMeshComponent=StaticMeshComponent0
         Components(0)=StaticMeshComponent0
         RootComponent=StaticMeshComponent0
         ActorLabel=\"%\"
      End Actor
"

	  
t3d_Footer =
"   End Level
Begin Surface
End Surface
End Map
"

	global UE4_Export_FBX_Rollout = rollout UE4_Export_FBX_Rollout "UE4 FBX Exporter Version 1.1.1"
	(

		group  "Export Mode"
		(
			radioButtons UE4_ObjectType labels:#("Export Selected Objects to FBX Files","Copy Selected Object Transforms and Names to Clipboard") align:#left
		)
		
		group "Export Options"
		(
			checkbox UE4_UseGrid "Use user grid? (Grid must be named \"UE4_Grid\" to work.)" enabled:false checked:true
			checkbox UE4_MoveToOrigin "Move each object to 0,0,0?" enabled:true checked:true
			checkbox UE4_IncludeCollision "Include UCX Collision Shells?" across:2 checked:true
			hyperlink hl2 "For More Information Click Here" address:"https://docs.unrealengine.com/latest/INT/Engine/Content/FBX/StaticMeshes/index.html#collision" color:(color 0 0 255) visitedColor:(color 0 0 255)

				
			checkbox UE4_Dialog "Show FBX Export Dialog? (Will only be shown once per export)" checked:true 
			label label1 "Export Path:" align:#left
			edittext UE4_ExportPath_Text readonly:true  width:360 align:#left across:3 
			button UE4_SetExportPathButton "..."  height:17 width:17 offset:[174,0] tooltip:"Explicitly set the export path"
			button UE4_ClearExportPathButton "X"  height:17 width:17 offset:[60,0] tooltip:"Reset the export patht to be relative to this MAX scene file"
			button OpenExportPathButton "Open Export Path in Explorer"
			edittext UE4_folder_Text readonly:false  width:100 align:#left tooltip:"Explicitly set the game folder."
		)
		
		listbox UE4_Listbox "Objects to be Exported:" readonly:true height:20
		button doit "Export Selected Objects" width:220 height:50	
		
		hyperlink hl1 "Help and Info" address:"http://www.tomshannon3d.com/2014/07/tstools-first-release-ue4-mass-fbx.html" align:#center color:(color 0 0 255) visitedColor:(color 0 0 255)
		
		local doGridTransform = false
		local exportObjects = #()
		local selnames = #()
		local ErrorArray = #()
		local exportPath = undefined
		
		local layerNames = #()
		local layerObjects = #()
		
		local packageName = "" --do i still need this?
		
		fn updateExportPath =
		(
			-- If exportPath AND maxFilePath aren't set
			if maxfilepath == "" and ExportPath == undefined then 
				UE4_ExportPath_Text.text = "Please save the scene or specify an export path explicitly."
			
			-- If exportPath has NOT been defined and the scene file HAS been saved, assume the export path
			if exportPath == undefined and maxfilepath != "" do 
			(
				exportPath = maxfilepath+@"export\"
				UE4_ExportPath_Text.text = exportPath
			)
				
			--If there's an export path defined in the scene, use that instead
			SavedExportPath = (getAppData rootNode 77900)
			if (SavedExportPath != undefined) then 
			(
				exportPath = SavedExportPath
				UE4_ExportPath_Text.text = exportPath
			) 
		)		
		
		--Ensure there's a valid export path before exporting. If it's not there, ask the user if they want to create the path or cancel
		--Returns true if the directory exists or is created
		--Returns false if the director doesn't exist and the user says NO
		fn checkExportPath exportPathName =
		(
			if doesfileexist (exportPathName as String) == false then 
			(
				makeTheDir = queryBox (exportPathName as String +"\nDoes not exist.\nCreate this directory now?") title:"Export Directory Does Not Exist" beep:true
				if makeTheDir == true then
				(
					makeDir exportPathName
					return true
				)
				else return false
			)
			else return true
		)
		
		--Callback controls
		fn addCallbacks =
		(
			callbacks.addScript #selectionSetChanged "UE4_Export_FBX_Rollout.updateList()" id:#UE4_Export_FBX_Rollout_Update
			callbacks.addScript #filePostOpen "UE4_Export_FBX_Rollout.updateList resetExportPath:true" id:#UE4_Export_FBX_Rollout_Update
			callbacks.addScript #systemPostNew "UE4_Export_FBX_Rollout.updateList resetExportPath:true" id:#UE4_Export_FBX_Rollout_Update
			callbacks.addScript #systemPostReset "UE4_Export_FBX_Rollout.updateList resetExportPath:true" id:#UE4_Export_FBX_Rollout_Update
			callbacks.addScript #filePostSave "UE4_Export_FBX_Rollout.updateList()" id:#UE4_Export_FBX_Rollout_Update
		)
		
		fn removeCallbacks =
		(
			callbacks.RemoveScripts id:#UE4_Export_FBX_Rollout_Update
		)
		
		--return all collision shells matching the baseMesh's name
		fn findMatchingUCX baseMesh =
		(
			local matchingUCX = (execute ("$'UCX_"+baseMesh.name as string+"'*")) as array
			if matchingUCX != undefined then
				return matchingUCX
			else return false
		)
		
			
		
		--global updateList
		global updateList = fn updateList resetExportPath:false =
		(
			if resetExportPath==true do
				exportpath = undefined
			
			--Actual objects to be exported!
			exportObjects = #()
			
			--Names, etc to populate the selection list. This veruable name is bad now because it handles some error strings as well!
			local selnames = #()
			
			--run the export path function to ensure the path is as up to date as possible
			updateExportPath()
			
			--Check to see if there's a UE4_Grid object in the scene, if so set the options
			if $UE4_Grid == undefined then
			(
				UE4_UseGrid.checked = false
				UE4_UseGrid.enabled = false
			)
			else UE4_UseGrid.enabled = true
			
			--Make sure there's something in the selection
			theSelection = selection as array
			OriginalSelection = selection as Array
			ErrorArray = #()
			
			--Do some basic error checking:
			--Ensure something is selected
			if theSelection.count == 0 do
			(
				append ErrorArray "    --Nothing Selected! "
				append ErrorArray "        Please select some meshes to export."
			)
			--Ensure there's something selected that can be exported, also build the exportObjects array that's used for the actual export
			for obj in theSelection do
			(
				if (superclassof obj == GeometryClass and classof obj != Targetobject and matchpattern obj.name pattern:"UCX_*" == false) do 
				(
					objName = obj.name
					append exportObjects obj
				)
			)
			if exportObjects.count == 0 and theSelection.count != 0 do 
			(
				append ErrorArray "    --No valid Geometry selected! "
				append ErrorArray "        Please select some non-UCX meshes to export."
			)
			--Check the valididty of the export path
			if exportPath == undefined do 
			(
				append ErrorArray "    --No Export Path Defined! Please define an export path above."
				append ErrorArray "        Please define an export path above."
			)
			
			
			if errorArray.count == 0 then
			(
				if UE4_ObjectType.state == 1 then append selnames (exportPath) --display the export path in the window
				else append selnames ("To be copied to Clipboard:")
				
				if exportObjects.count == 0 then
				(
					UE4_Listbox.items = #("No Valid Objects Selected! Only non-UCX geometry can be exported.")
					return true
				)
				
				-- build the .fbx  list (THIS IS PURELY for display!)
				for obj in exportObjects do
				(
					objName = obj.name
					if UE4_ObjectType.state == 1 then
					(
						if UE4_UseGrid.checked == false then tempString = "  "+objName+".fbx @ "+obj.pos as string
							else tempString = "  "+objName+".fbx @ "+(obj.pos-$UE4_Grid.pos) as string+" relative to UE4_Grid.pos"
						if UE4_MoveToOrigin.checked == true do tempString = "  "+objName+".fbx @ 0,0,0"
							
					)
					if UE4_ObjectType.state == 2 then
					(
						if UE4_UseGrid.checked == false then tempString = "  "+objName+" @ "+obj.pos as string
							else tempString = "  "+objName+" @ "+(obj.pos-$UE4_Grid.pos) as string+" relative to UE4_Grid.pos"
					)
					
					append selnames tempString
					
					if UE4_IncludeCollision.checked == true and UE4_ObjectType.state == 1 do
					(
						for UCX in (findMatchingUCX obj) do append selnames ("    "+UCX.name as string)
					)
				)

				if (selnames.count > 0 )then
					UE4_Listbox.items = selnames
					
			)-- END if selection.count != 0 then
			else
			(
				append selnames ("There are errors; can't build the object list:" as string)
				join selnames ErrorArray
				UE4_Listbox.items = selnames
			)
			
		)-- end function updateList
		

		on UE4_ObjectType changed newState do
		(
			if Newstate == 1 then
			(
				UE4_UseGrid.enabled = true 
				UE4_MoveToOrigin.enabled = true
				UE4_IncludeCollision.enabled = true 
				UE4_Dialog.enabled = true 
			)
			else
			(
				UE4_UseGrid.enabled = false 
				UE4_MoveToOrigin.enabled = false
				UE4_IncludeCollision.enabled = false 
				UE4_Dialog.enabled = false 
			)
			updatelist()
		)
		
		
		on UE4_Export_FBX_Rollout open do
		(
			updateList()
			addCallbacks()
		)
		
		on UE4_Export_FBX_Rollout close do
		(
			removeCallbacks()
			UE4_Export_FBX_Rollout = null
		)
		
		on UE4_SetExportPathButton pressed do
		(
			--Open to a resonable location
			if (doesfileexist (exportPath as String)) then thePath = getSavepath initialDir:(exportPath)
			else thePath = getSavepath initialDir:(maxfilepath)
			print thePath
			
			--Once set, save the path to the scene's appdata
			if thePath != undefined then 
			(
				exportPath = thePath
				SetAppData rootNode 77900 exportPath
			)
				
			updateList()
		)
		
		on UE4_ClearExportPathButton pressed do
		(
			exportPath = undefined
			deleteAppData rootNode 77900
			updateList()
		)
		
		
		--Refresh the List/UI when otions are changed, etc.
		on UE4_UseGrid changed newState do updateList()
		on UE4_Dialog changed newState do updateList()
		on UE4_MoveToOrigin changed newState do updateList()
		on UE4_IncludeCollision changed newState do updateList()
		
		
		on OpenExportPathButton pressed do
		(
			if exportPath != undefined then
				if (checkExportPath exportPath == true) then shellLaunch "explorer.exe" exportPath
			else 
				messageBox "No export path is defined."
		)
		
		--
		--
		--//* THIS IS THE BIG BUTTON PRESS FUCTIONALITY!!
		--
		--
		on doit pressed do
		(
			updateList()
			
			if ErrorArray.count != 0 do
			(
				ErrorMessage = ""
				for e in errorArray do append ErrorMessage ("\n"+e as string)
				messagebox ("Cannot export at this time:\n"+ ErrorMessage)
				return false
			)
			
			--Getting the options here for easy reference
			doGridTransform = UE4_UseGrid.checked
			doOrigin = UE4_MoveToOrigin.checked 
			dot3d = UE4_ObjectType.state == 2
			doGeom = UE4_ObjectType.state == 1
			doUCX = UE4_IncludeCollision.checked
			
			doDialog = UE4_Dialog.checked
			
			--Using the exportObjects generated by updateList to determine what to export
			--Probably don't need this anymore
			theSel = exportObjects
			
			--we need to 'pause' the callbacks in the script while we do this
			removeCallbacks()
			
			-- for speed
			max create mode
			
			--Here's where we create our selection arrays
			selectionArrays = #()
			
			-- Begin the t3d stringstream if we're doing that
			if (dot3d == true) do 
			(
				local t3dFile = stringstream ""
				format t3d_Header to:t3dFile
			)
			
			--ensure there's a directory to export to
			if (doGeom == true) do makeDir exportPath all:true
			
			--This is so we only show the FBX window the first time
			firstObject = true
			for obj in exportObjects do
			(
				objName = obj.name
				--select obj
				--Geometry/fbx
				if (doGeom == true) then
				(
					if doUCX then select (append (findMatchingUCX obj) obj)
					else select obj
					--print ("SLKAJSLDKJALSKDJALKSJDLAKSJDLAKSJDLKAJSDLKJASLDKJASLDJK")
					local objectShift = [0,0,0] --to be able to shift the collision shells if needed
					local exportFileName = exportPath+@"\"+objName+@".fbx"
					
					local objTransform = copy obj.transform
					
					--Get the shift based on user options
					if (doGridTransform == true and $UE4_Grid != undefined) then
					(
						objectShift = $UE4_Grid.pos
						
						if doOrigin == true do 
							objectShift = obj.pos - $UE4_Grid.pos
						
					)
					else if doOrigin == true then 
						objectShift = obj.pos
					
					--move everything
					for o in selection do 
						o.pos = o.pos - objectShift
					
					--Export selected!
					--Show the FBX dialog
					if firstObject == true and doDialog == true then 
						exportFile exportFileName selectedOnly:true
					
					else
						exportFile exportFileName #noPrompt selectedOnly:true
					
					firstObject = false
					
					--Move everything back
					for o in selection do 
						o.pos = o.pos + objectShift
					
				)		
				
				

				--.t3d --Now it just adds the text to the clipboard
				if (dot3d == true) then
				(
					
					--Default transformations
					local OBJRot = (matrix3 1).rotationpart as eulerangles 
					local OBJPos = [0,0,0]
					
					if (doGridTransform and $UE4_Grid != undefined) then
					(
						OBJRot = obj.transform.rotationpart as eulerangles 
						OBJPos = obj.pos - $UE4_Grid.pos
						
					)
					else
					(
						OBJRot = obj.transform.rotationpart as eulerangles 
						OBJPos = obj.pos
					)
--Name=EditorCube7
--RelativeLocation=(X=-2304.000000,Y=2047.999878,Z=298.000000)
--RelativeScale3D=(X=4.000000,Y=2.000000,Z=0.250000)
--RelativeRotation=(Pitch=0.000005,Yaw=180.000366,Roll=354.375000)
--ActorLabel="EditorCube114"
					if superclassof obj == GeometryClass do
						format t3d_StaticMesh_Entry objName UE4_folder_Text.text objName objName OBJPos.x (OBJPos.y*-1) OBJPos.z obj.scale.x obj.scale.y obj.scale.z (OBJRot.y*-1.0) (OBJRot.z*-1.0) OBJRot.x  objName to:t3dFile
				)
			)
			
			--Close off the t3d file if it's being made
			if (dot3d == true) then 
			(
				
				format t3d_Footer to:t3dFile
				setClipBoardText  t3dFile
				free t3dfile
			)
			
			select exportObjects
			updateList()
			addCallbacks()
		) --end on doit pressed do
		
	) -- end rollout
	createDialog UE4_Export_FBX_Rollout width:420	
)