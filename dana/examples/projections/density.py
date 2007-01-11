#!/usr/bin/env python

import dana.core as core
import dana.view as view
import dana.projection as projection
import dana.projection.distance as distance
import dana.projection.density as density
import dana.projection.shape as shape
import dana.projection.profile as profile

if __name__ == '__main__':

    size = 40;

    net = core.Network()
    
    m0 = core.Map( (size, size), (0,0) )
    m0.append( core.Layer() )
    m0.layer(0).fill(core.Unit)
    m0.name = 'm0: full'
    net.append(m0)
    
    m1 = core.Map( (size, size), (1,0) )
    m1.append( core.Layer() )
    m1.layer(0).fill(core.Unit)
    m1.name = 'm1: sparse'
    net.append(m1)

    m2 = core.Map( (size, size), (2,0) )
    m2.append( core.Layer() )
    m2.layer(0).fill(core.Unit)
    m2.name = 'm2: sparser'
    net.append(m2)

    proj          = projection.projection()
    proj.self     = True
    proj.distance = distance.euclidean(False)
    proj.density  = density.full(1)
    proj.shape    = shape.box(1,1)
    proj.profile  = profile.constant(1)
    proj.src      = m0[0]
    proj.dst      = m0[0]
    proj.connect()
    
    proj.density  = density.sparse(.25)
    proj.src      = m1[0]
    proj.dst      = m1[0]
    proj.connect()
    
    proj.density  = density.sparser(.5)
    proj.src      = m2[0]
    proj.dst      = m2[0]
    proj.connect()
    
    netview = view.view(net, 'Click on unit to see weights', 12)
    netview.show()
