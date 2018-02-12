macroScript TS_UE4NameUCXPrimitives category:"TS_Tools" tooltip:"Name objects as UCX collision primitives" buttonText:"UE4 UCX Shell Namer"
(
	/* 
	Tom Shannon
	www.TomShannon3D.com
	
	--Quickly name selected collision shells based on selected mesh's name: UCX_[Selected Mesh's Name]_XX
	--Simply select the primitives you want to name, run the script, then select the object you want to name them after
	--Conveniently hides the selected collision primitives until the selection is done
	
	Version 1.0 -	2014-09-07 : 	First version!
	*/
	
	
	SelArray = selection as array
	
	fn geomFilt o = (superClassOf o == GeometryClass)
	
	hide SelArray
	local i = 1.0
	undo "Name UCX Primitives" on
	(
		baseMesh = pickObject message:"Select the mesh to base the name from." filter:geomFilt forceListenerFocus:false pickFrozen:true
		if baseMesh != undefined do
		(
			for obj in SelArray do
			(
				InstanceNumber = formattedPrint (i as float) format:"02.0g"
				obj.name = "UCX_"+baseMesh.name as string+ "_" + InstanceNumber as String
				i = i+1
				print ("Renamed to " + obj.name as string)
			)
		)
	)
	unhide SelArray
	
)