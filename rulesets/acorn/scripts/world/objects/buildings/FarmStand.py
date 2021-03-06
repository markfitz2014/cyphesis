#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 5,8,2.5
# bmedian = 6.5,6.5,2.5
# offset = SW corner = 1.5,-1.5,0

class Shop_farmstand_deco_agril_1_sesw(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall
	loc = Location(self, Vector3D(1.5, 0, 1.5))
        loc.bbox = Vector3D(10, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall with door
	loc = Location(self, Vector3D(1.5, 0, 1.5))
        loc.bbox = Vector3D(0.5, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(1.5, 0, -2.5))
        loc.bbox = Vector3D(0.5, 5, -12)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(1.5, 0, -14))
        loc.bbox = Vector3D(8, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall with door
	loc = Location(self, Vector3D(11, 0, 1.5))
        loc.bbox = Vector3D(0.5, 5, -2)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(11, 0, -2.5))
        loc.bbox = Vector3D(0.5, 5, -4)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(9, 0, -6.5))
        loc.bbox = Vector3D(4, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(9, 0, -6.5))
        loc.bbox = Vector3D(0.5, 5, -16)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
