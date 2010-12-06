#!/usr/bin/env python
# -----------------------------------------------------------------------------
# DANA - Distributed (Asynchronous) Numerical Adaptive computing framework
# Copyright (C) 2009-2010  Nicolas P. Rougier
#
# Distributed under the terms of the BSD License. The full license is in
# the file COPYING, distributed as part of this software.
# -----------------------------------------------------------------------------
'''
This is a set of plot functions dedicated to DANA. They allow to show
connections when user click on a unit and to display unit activity when mouse is
over a unit.

**Examples**::

  >>> from dana import *
  >>> from display import plot
  >>> Z = Group((40,40), 'V;U')
  >>> fig = plt.figure(figsize=(10,10), facecolor='white')
  >>> plot(plt.subplot(1,2,1), Z('V'), 'Zv')
  >>> plot(plt.subplot(1,2,2), Z('U'), 'Zu')
  >>> plt.show()
'''
from dana import *
from functools import partial

def format_coord(axis, x, y):
    Z = axis.get_array()
    if x is None or y is None or Z is None:
        return ''
    x,y = int(x), int(y)
    if 0 <= x < Z.shape[1] and 0 <= y < Z.shape[0]:
        return '[%d,%d]: %s' % (x,y, Z[y,x])
    return ''

def update(G=None, x=-1, y=-1):
    mgr = plt.get_current_fig_manager()
    if G is None:
        for axis,group,data,subplot in mgr.subplots:
            if hasattr(group, 'mask'):
                D = np.where(group.mask,data,np.NaN)
            else:
                D = data
            axis.set_data(D)
    else:
        for axis,group,data,subplot in mgr.subplots:
            axis.set_data(np.empty_like(data)*np.NaN)
        if hasattr(G, '_connections'):
            for C in G._connections:
                for axis,group,data,subplot in mgr.subplots:
                    if C._actual_source is data:
                        try:
                            axis.set_data(C[y,x])
                        except NotImplementedError:
                            pass

def button_press_event(event):
    G,x,y = None, -1, -1
    if event.inaxes and event.button == 1:
        G = event.inaxes.group
        x,y = int(event.xdata), int(event.ydata)
    update(G, x, y)
    plt.draw()

def plot(subplot, data, title='', *args, **kwargs):
     mgr = plt.get_current_fig_manager()
     a,b = 0.75, 1.0
     chessboard = np.array(([a,b]*16 + [b,a]*16)*16)
     chessboard.shape = 32,32
     if isinstance(data, Group):
         group = data.base
         data = data[data.keys[0]]
     else:
         group = data
         data = data

     plt.imshow(chessboard, cmap=plt.cm.gray, interpolation='nearest',
                extent=[0,group.shape[1],0,group.shape[0]],
                vmin=0, vmax=1)
     plt.hold(True)

     if hasattr(group, 'mask'):
         D = np.where(group.mask,data,np.NaN)
     else:
         D = data
     axis = plt.imshow(D, interpolation='nearest',
                       #cmap=plt.cm.bone, cmap= plt.cm.PuOr_r,
                       #origin='lower', vmin=0, vmax=1,
                       extent=[0,group.shape[1],0,group.shape[0]],
                       *args, **kwargs)
     subplot.format_coord = partial(format_coord, axis)
     subplot.group = group
     plt.xticks([]), plt.yticks([])

     x,y,w,h = axis.get_axes().bbox.bounds
     dw = 0*float(group.shape[1]-1)/w
     dh = 0*float(group.shape[0]-1)/h
     plt.axis([-dw,group.shape[1]+dw,-dh,group.shape[0]+dh])
     if not hasattr(mgr, 'subplots'):
         mgr.subplots = []
     mgr.subplots.append((axis,group,data,subplot))

     if title:
         plt.title(title,fontsize=16)


if __name__ == '__main__':
    Z = Group((40,40), 'V')
    Z.mask = np.ones((40,40))
    Z.mask[:20,:20] = 0
    fig = plt.figure(figsize=(8,8), facecolor='white')
    plot(plt.subplot(1,1,1), Z('V'), 'A group with dead units')
    plt.show()