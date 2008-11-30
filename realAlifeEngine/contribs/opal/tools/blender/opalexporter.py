#!BPY
"""
Name: 'OPAL XML Exporter'
Blender: 235
Group: 'Export'
Tooltip: 'Exported selected meshes to OPAL XML file. \
Each mesh must have a 'type' property.  It and can also have 'static' \
property and and other user-defined properties.'
"""

import math
from math import degrees, sqrt, cos, acos
import Blender
from Blender import Object, Mathutils, Library
from Blender.Mathutils import VecMultMat
import xml.dom.minidom
from xml.dom.minidom import getDOMImplementation

#filename = 'opalScene.xml'
filename = Blender.Get('filename')
filename = Blender.sys.basename(filename)
blueprintName = filename.split('.')[0]
filename = blueprintName + '.xml'
#print dir(Blender.Library)
print
print "=========================================================="
print "Exporting selected objects to OPAL XML file: " + filename
print "=========================================================="
print

def addElement(name, parent):
    new = doc.createElement(name)
    parent.appendChild(new)
    return new

def getLocalExtents(object):
	euler = object.getEuler()
	pos = object.getLocation()
	zeroEuler = Blender.Mathutils.Euler([0,0,0])
	object.setEuler(zeroEuler)
	object.setLocation(0,0,0)
	Blender.Redraw()
	corners = object.getBoundBox()
	object.setLocation(pos[0], pos[1], pos[2])
	object.setEuler(euler)
	
	# now we have 8 points defining the corners of the object
	# calculate the range for each dimension
	x = corners[4][0] - corners[0][0]
	y = corners[2][1] - corners[0][1]
	z = corners[1][2] - corners[0][2]
	return x, y, z

def distance(p1, p2):
	sqrDist = pow(p2[0]-p1[0], 2) + pow(p2[1]-p1[1], 2) + pow(p2[2]-p1[2], 2)
	return (sqrt(sqrDist))
	
def calcABCD(point, normal):
	origin = [0,0,0]
	normal.normalize()
	v = Blender.Mathutils.Vector([0,0,0])
	v[0] = point[0] - origin[0]
	v[1] = point[1] - origin[1]
	v[2] = point[2] - origin[2]	
	angle = 0
	if 0 != v.length:
		v.normalize()
		angle = acos(Blender.Mathutils.DotVecs(normal, v))
	length = distance(point, origin)
	d = length * cos(angle)
	abcd = [normal[0], normal[1], normal[2], d]
	return abcd
	
impl = getDOMImplementation()

doc = impl.createDocument(None, "RootElement", None)
root = doc.documentElement
bp = addElement('OpalBlueprint', root)
bp.setAttribute('name', blueprintName)

objects = Blender.Object.GetSelected()

# for now assume each object represents a Solid and that each Solid
# only has one shape; TODO: use grouping to add multiple shapes to
# a single solid

for o in objects:
	solid = addElement('Solid', bp)
	solid.setAttribute('name', o.getName())

	# export params
	params = addElement('Params', solid)
	
	#staticParam = addElement('Param', params)
	#staticParam.setAttribute('static', 'false') # default value
		
	properties = o.getAllProperties()
	for p in properties:
		newParam = addElement('Param', params)
		newParam.setAttribute(p.getName(), p.getData())
	
#		if p.getName() == 'static':
#			staticParam.setAttribute('static', p.getData())
#		else:
#			if p.getName() == 'mesh':
#				meshParam = addElement('Param', params)
#				meshParam.setAttribute('mesh', p.getData())

	# export offset transforms
	# note that planes don't use offset transforms
	if o.getProperty('type').getData() != 'plane':
		offset = addElement('Offset', solid)
		
#		transform.setAttribute('type', 'matrix')
#		m = o.getMatrix('worldspace')
#		transform.setAttribute('_00', str(m[0][0]))
#		transform.setAttribute('_01', str(m[0][1]))
#		transform.setAttribute('_02', str(m[0][2]))
#		transform.setAttribute('_03', str(m[0][3]))
#		transform.setAttribute('_10', str(m[1][0]))
#		transform.setAttribute('_11', str(m[1][1]))
#		transform.setAttribute('_12', str(m[1][2]))
#		transform.setAttribute('_13', str(m[1][3]))
#		transform.setAttribute('_20', str(m[2][0]))
#		transform.setAttribute('_21', str(m[2][1]))
#		transform.setAttribute('_22', str(m[2][2]))
#		transform.setAttribute('_23', str(m[2][3]))
#		transform.setAttribute('_30', str(m[3][0]))
#		transform.setAttribute('_31', str(m[3][1]))
#		transform.setAttribute('_32', str(m[3][2]))
#		transform.setAttribute('_33', str(m[3][3]))

		transform = addElement('Transform', offset)
		transform.setAttribute('type', 'translate')
		pos = o.getLocation()
		transform.setAttribute('x', str(pos[0]))
		transform.setAttribute('y', str(pos[1]))
		transform.setAttribute('z', str(pos[2]))
			
		# warning: euler angles are bad when y == 90
		transform = addElement('Transform', offset)
		transform.setAttribute('type', 'rotate')
		euler = o.getEuler()	
		transform.setAttribute('x', str(degrees(euler[0])))
		transform.setAttribute('y', str(degrees(euler[1])))
		transform.setAttribute('z', str(degrees(euler[2])))

	shape = addElement('Shape', solid)
	shape.setAttribute('type', o.getProperty('type').getData())
	dimensions = addElement('Dimensions', shape)
	size = getLocalExtents(o)
	
	if o.getProperty('type').getData() == 'box':
		dimensions.setAttribute('x', str(size[0]))
		dimensions.setAttribute('y', str(size[1]))
		dimensions.setAttribute('z',str( size[2]))
	elif o.getProperty('type').getData() == 'sphere':
		dimensions.setAttribute('radius', str(size[0]*sqrt(2)))
	elif o.getProperty('type').getData() == 'capsule':
		dimensions.setAttribute('radius', str(size[0]*sqrt(2)))
		dimensions.setAttribute('length', str(size[2] - 2*size[0]*sqrt(2)))        
	elif o.getProperty('type').getData() == 'plane':
		# planes take a lot of extra work, especially since we don't
		# already have a, b, c, and d for the plane equation
		pos = o.getLocation()
		rotmat = o.getMatrix('worldspace').rotationPart()
		initialNormal = Blender.Mathutils.Vector([0,0,1])
		normal = VecMultMat(initialNormal, rotmat)		
		normal.normalize()
		# this doesn't work because normal doesn't always get updated :(
		#normal = o.getData().faces[0].normal
		
		abcd = calcABCD(pos, normal)
		dimensions.setAttribute('a', str(abcd[0]))
		dimensions.setAttribute('b', str(abcd[1]))
		dimensions.setAttribute('c', str(abcd[2]))
		dimensions.setAttribute('d', str(abcd[3]))

	# note: no offsets for shapes at this time since there is
	# only one shape per solid

f = file(filename, 'w')
f.write(doc.toprettyxml())
f.close()