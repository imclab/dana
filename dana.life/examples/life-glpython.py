#!/usr/bin/env python
#------------------------------------------------------------------------------
# Copyright (c) 2006-2007 Nicolas Rougier.
# All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
# 
# $Id$
#------------------------------------------------------------------------------
#!/usr/bin/env python

import random
import dana.core as core
import dana.projection as proj
import dana.life as life
from glpython import window
from dana.visualization.glpython import Figure
import dana.gui.gtk as gui

# Create a new model
model = core.Model()

# Create a new network
net = core.Network ()
model.append(net)
size = 100

# Create the map
Map = core.Map ( (size,size), (0,0) )
Map.append(core.Layer())
Map[0].fill(life.Unit)
net.append(Map)

# Create input to focus connections
p = proj.projection()
p.self = False
p.distance = proj.distance.euclidean (False)
p.density = proj.density.full(1)
p.profile = proj.profile.constant(1.0)
p.shape = proj.shape.box(1.0/size, 1.0/(size))
p.src = Map[0]
p.dst = Map[0]
p.connect()

for u in Map[0]:
    u.potential = random.randint (0, 1)

fig = Figure()
win,fig = window (backend='gtk', figure=fig)
fig.network (net, style='flat', show_colorbar=False)
fig.text (size=.1, position = (.5, -.05), text="Asynchronouys Game Of Life demo")
fig.text (size=.05, position = (.5, -.08), text="Nicolas Rougier, 07/2007")
panel = gui.ControlPanel (model)
win.show()

