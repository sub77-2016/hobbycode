global D_TRUE = 1
global D_FALSE = 0
global D_BOX_ID = #(16,0)
global D_SPHERE_ID = #(17,0)
--global D_CYLINDER_ID = #(18,0)
global D_CAPSULE_ID = #(1832744876, 2043230633)

-- Path for output files.
global savePath

-- Global handle for the output file.
global outFile

-- Checks if the scene is in the proper format for exporting.
function checkScene =
(
	-- is something selected?
	if selection.count == 0 then
	(
		displayTempPrompt "Opal Error: nothing is selected!" 5000
		return D_FALSE
	)
		
	-- if saving is canceled return false
	savePath = getSaveFileName caption:"Export Solid To..." filename:"untitled" types:"Opal Blueprint(*.xml)|*.xml|All|*.*"
	if savePath == undefined then return D_FALSE

	return D_TRUE
)

-- Prints an offset element for Solid.
function printSolidOffset trans angle axis = 
(
	format "\t\t<Offset>\n" to:outFile

	format "\t\t\t<Transform type=\"translate\" x=\"%\" y=\"%\" z=\"%\"/>\n" trans.x trans.y trans.z to:outFile

	-- Only save the rotation if it's significant.
	if abs(angle) > 0.001 then
	(
		format "\t\t\t<Transform type=\"rotate\" angle=\"%\" x=\"%\" y=\"%\" z=\"%\"/>\n" angle axis.x axis.y axis.z to:outFile
	)
	
	format "\t\t</Offset>\n" to:outFile
)

-- Prints an offset element for Shapes.
function printShapeOffset trans angle axis = 
(
	format "\t\t\t<Offset>\n" to:outFile

	format "\t\t\t\t<Transform type=\"translate\" x=\"%\" y=\"%\" z=\"%\"/>\n" trans.x trans.y trans.z to:outFile

	-- Only save the rotation if it's significant.
	if abs(angle) > 0.001 then
	(
		format "\t\t\t\t<Transform type=\"rotate\" angle=\"%\" x=\"%\" y=\"%\" z=\"%\"/>\n" angle axis.x axis.y axis.z to:outFile
	)
	
	format "\t\t\t</Offset>\n" to:outFile
)

-- Prints a Shape element for the given object.
function printShape object printOffset = 
(
	if object.classID[1] == D_SPHERE_ID[1] then
	(
		format "\t\t<Shape type=\"sphere\">\n" to:outFile
		format "\t\t\t<Dimensions radius=\"%\"/>\n" (abs(object.scale[1] * object.radius)) to:outFile

	)
	else if object.classID[1] == D_CAPSULE_ID[1] then
	(
		format "\t\t<Shape type=\"capsule\">\n" to:outFile
		format "\t\t\t<Dimensions radius=\"%\" length=\"%\"/>\n" (abs(object.scale[1] * object.radius)) (abs(object.scale[1] * object.height)) to:outFile
	)
	-- Default to boxes for everything else.
	else
	(
		-- We need to find the extents of the object's locally-aligned bounding box.  
		-- The MAXScript bounding box extents are wrong for the local coordsys, 
		-- so we'll just move the object to the global origin, gets its extents, 
		-- and then move it back.
		
		-- First, store the object's original transform.
		originalTransform = object.transform
		
		-- Set its transform to the identity.
		object.transform = Matrix3(1)

		-- Calculate the bounding box's extents.
		dimensions = object.max - object.min

		-- Restore the object's original transform.
		object.transform = originalTransform

		format "\t\t<Shape type=\"box\">\n" to:outFile
		format "\t\t\t<Dimensions x=\"%\" y=\"%\" z=\"%\"/>\n" dimensions.x dimensions.y dimensions.z to:outFile
	)

	-- If requested, print the Shape's offset from its parent.
	if (true == printOffset) then
	(
		in coordsys parent offsetTranslation = object.center
		in coordsys parent offsetRotation = object.rotation
		printShapeOffset offsetTranslation offsetRotation.angle offsetRotation.axis
	)

	format "\t\t</Shape>\n" to:outFile
)

-- Check if the scene is valid.
readyToGo = checkScene()

if(readyToGo == D_TRUE) then
(
	-- Create the output file.
	outFile = createfile savePath

	-- Add the XML version and initial Blueprint tags.
	format "<?xml version=\"1.0\"?>\n" to:outFile
	format "<OpalBlueprint>\n" to:outFile

	-- Handle Solids.
	--i = 0
	for object in selection do
	(
		-- Only print a Solid element for group heads and non-group members.
		if isGroupHead object or false == isGroupMember object then
		(	
			format "\t<Solid>\n" to:outFile
	
			-- Use the object name for the Solid name.
			format "\t\t<Name value=\"%\"/>\n" object.name to:outFile

			-- Handle group objects.
			if isGroupHead object then
			(
				-- Create a Shape for each group member.

				-- Loop over the group members.
				for member in object do
				(
					-- The group head "is" the Solid; use its transform as the Solid's transform.
					if true == isGroupHead member then
					(
						in coordsys world worldTranslation = member.center
						in coordsys world worldRotation = member.rotation
						printSolidOffset worldTranslation worldRotation.angle worldRotation.axis
					)
					-- Create a Shape for each group member.  Their transforms represent offsets from 
					-- the group head.
					else
					(
						printShape member true
					)
				)
			)
			-- Handle "normal" (non-group) objects.  Just create a single Shape.
			else if false == isGroupMember object then
			(
				-- Print the Solid's transform.
				in coordsys world worldTranslation = object.center
				in coordsys world worldRotation = object.rotation
				printSolidOffset worldTranslation worldRotation.angle worldRotation.axis
				
				-- Print the Shape with no offset from its Solid.
				printShape object false
			)
			
			format "\t</Solid>\n" to:outFile
		)
	)
	
	-- Add the closing Blueprint tag.
	format "</OpalBlueprint>\n" to:outFile

	-- Close the output file.
	close outFile	

	displayTempPrompt "Finished exporting OPAL Blueprint" 10000
)