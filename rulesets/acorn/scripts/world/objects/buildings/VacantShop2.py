#This file is distributed under the terms of the GNU General Public license.
#Copyright (C) 2001 Al Riddoch (See the file COPYING for details).

from cyphesis.Thing import Thing
from atlas import *
from Vector3D import Vector3D

# bbox = 5,4,2.5
# bmedian = 4.5,4.5,2.5
# offset = SW corner = -0.5,0.5,0

class Shop_vacant_deco_2_se(Thing):
    def setup_operation(self, op):
        ret = Oplist()
        # South wall with door
	loc = Location(self, Vector3D(-0.5, 0, -0.5))
        loc.bbox = Vector3D(6, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
	loc = Location(self, Vector3D(7.5, 0, -0.5))
        loc.bbox = Vector3D(2, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # West wall
	loc = Location(self, Vector3D(-0.5, 0, -0.5))
        loc.bbox = Vector3D(0.5, 5, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # North wall
	loc = Location(self, Vector3D(-0.5, 0, -8))
        loc.bbox = Vector3D(10, 5, -0.5)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        # East wall
	loc = Location(self, Vector3D(9, 0, -0.5))
        loc.bbox = Vector3D(0.5, 5, -8)
        ret.append(Operation("create",Entity(name='wall',parent='wall',location=loc),to=self))
        return ret
