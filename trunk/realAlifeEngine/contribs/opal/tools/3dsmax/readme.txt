OPAL 3ds max Exporter
---------------------

This exporter for 3ds max generates an OPAL XML file.  It is currently limited to a subset of the possible OPAL XML elements.  The current feature list includes:

* Single-Shape Solids
	- Ungrouped objects in 3dsmax
* Multi-Shape Solids
	- Grouped objects are automatically added to a single Solid
* Shapes
	- Box (3dsmax "box" primitive)
	- Sphere (3dsmax "sphere" primitive)
	- Capsule (3dsmax "capsule" primitive)
	- All other object types are exported as boxes, using the objects' locally-aligned bounding boxes

Note: All properties not supported by the exporter will use OPAL's default settings upon instantiation.

Note: All objects' local transforms refer to the center of their bounding boxes.  For Solids with a single Shape, this refers to the Shape's bounding box center.  For grouped objects (multi-Shape Solids), this refers to the group's bounding box center.


Planned Features
----------------

* Joints
	- Joint type
	- anchor point
	- axes