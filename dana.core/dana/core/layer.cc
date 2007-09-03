//
// Copyright (C) 2006 Nicolas Rougier
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// $Id: layer.cc 257 2007-07-29 11:38:44Z rougier $

#include <algorithm>
#include "map.h"
#include "layer.h"
#include "unit.h"
#include <numpy/arrayobject.h>

using namespace dana::core;


// ============================================================================
//  constructor
// ============================================================================
Layer::Layer (void) : Object()
{
    units.clear();
    map = 0;
}

// ============================================================================
//  destructor
// ============================================================================
Layer::~Layer (void)
{}


// ============================================================================
//  append new unit if some place left
// ============================================================================
void
Layer::append (UnitPtr unit)
{
    std::vector<UnitPtr>::iterator result;
    result = find (units.begin(), units.end(), unit);
    if (result != units.end())
        return;

    if ((map) && (map->width)) {
        if ((size() < map->width*map->height)) {
            unit->set_x (units.size() % map->width);
            unit->set_y (units.size() / map->width);
            units.push_back (UnitPtr(unit));
            unit->set_layer (this);
        } else {
            PyErr_SetString(PyExc_MemoryError, "no space left within layer");
            py::throw_error_already_set();
        }
    } else {
        units.push_back (UnitPtr(unit));
        unit->set_layer (this);
    }
}

// ============================================================================
//  get unit at index
// ============================================================================
UnitPtr
Layer::get (const int index) const
{
    int i = index;
    if (i < 0)
        i += units.size();

    try {
        return units.at(i);
    } catch (...) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
    }
    return UnitPtr();
}

// ============================================================================
//  get unit at x,y
// ============================================================================
UnitPtr
Layer::get (const int x, const int y) const
{
    int i = x;
    int j = y;
    if ( (!map) || (map->width == 0)) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return UnitPtr(new Unit());
    }
    
    if (i < 0)
        i += map->width;
    if (j < 0)
        j += map->height;
        
    if ((i >= map->width) || (j >= map->height)) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        py::throw_error_already_set();
        return UnitPtr(new Unit());
    }
    int index = j*map->width + i;
    return get (index);
}

// ============================================================================
//  get units size
// ============================================================================
int
Layer::size (void) const
{
    return units.size();
}

// ============================================================================
//  fill layer with objects of given type
// ============================================================================
int
Layer::fill (py::object type)
{
    py::extract <UnitPtr> unit (type());
    if (!unit.check()) {
        PyErr_SetString(PyExc_TypeError,"type is not derived from unit class");
        py::throw_error_already_set();
        return 0;
    }

    if ((map) && (map->width)) {
        units.clear();
        for (int i=0; i< map->width*map->height; i++) {
            UnitPtr unit = py::extract <UnitPtr> (type());
            append (UnitPtr(unit));
        }
     } else {
        PyErr_SetString(PyExc_AssertionError, "layer has no shape");
        py::throw_error_already_set();
        return 0;
     }
    return units.size();
}

// ============================================================================
//  Remove all units
// ============================================================================
void
Layer::clear (void)
{
    for (unsigned int i = 0; i< units.size(); i++)
        units[i]->potential = 0.0f;
}

// ============================================================================
//   evaluates all units potential and returns difference
// ============================================================================
float
Layer::compute_dp (void)
{
    // Speed problem with random_shuffle and threads
    //random_shuffle (permuted.begin(), permuted.end());
    
    float d = 0.0;
    int index = 0;
    for (unsigned int i = 0; i< units.size(); i++) {
        index = map->shuffles[map->shuffle_index][i];
        d += units[index]->compute_dp();
    }
    return d;
}

// ============================================================================
//  computes all units weights and returns difference
// ============================================================================
float
Layer::compute_dw (void)
{
    // Speed problem with random_shuffle and threads
    //random_shuffle (permuted.begin(), permuted.end());
    
    float d = 0.0;
    int index = 0;
    for (unsigned int i = 0; i< units.size(); i++) {
        index = map->shuffles[map->shuffle_index][i];
        d += units[index]->compute_dw();
    }
    return d;
}



// ________________________________________________________________________write
int
Layer::write (xmlTextWriterPtr writer)
{
    // <Layer>
    xmlTextWriterStartElement (writer, BAD_CAST "Layer");
    
    for (unsigned int i=0; i< units.size(); i++)
        units[i]->write(writer);

    // </Layer>
    xmlTextWriterEndElement (writer);

    return 0;
}

// _________________________________________________________________________read
int
Layer::read (xmlTextReaderPtr reader)
{
    xmlReaderTypes type   = XML_READER_TYPE_NONE;
    std::string    name   = "";
    int            status = 1;

    unsigned int index = 0;
    do  {
        status = xmlTextReaderRead(reader);
        if (status != 1)
            break;
        name = (char *) xmlTextReaderConstName(reader);
        type = (xmlReaderTypes) xmlTextReaderNodeType(reader);

        if ((type == XML_READER_TYPE_END_ELEMENT) && (name == "Layer"))
            break;

        if ((type == XML_READER_TYPE_ELEMENT) && (name == "Unit")) {
            if (index < units.size())
                units[index]->read (reader);
            index++;
        }        
    } while (status == 1);    
    
    return 0;
}

// ============================================================================
//  get owning layer
// ============================================================================
Map *
Layer::get_map (void) const
{
    return map;
}

// ============================================================================
//  set owning layer
// ============================================================================
void
Layer::set_map (Map *map)
{
    this->map = map;
    npy_intp dims[2] = {map->height, map->width};
    potentials = py::object(py::handle<>(PyArray_SimpleNew (2, dims, PyArray_FLOAT)));
}

// ============================================================================
//  get layer specification
// ============================================================================
SpecPtr
Layer::get_spec (void) const
{
    if ((spec == SpecPtr()) && (map))
        return map->get_spec();
    return SpecPtr(spec);
}

// ============================================================================
//  Set layer specification.
//  If given specification is none, specification from owning map is used.
// ============================================================================
void
Layer::set_spec (const SpecPtr s)
{
    spec = SpecPtr(s);
}

// ============================================================================
//  Get all potentials as a numpy::array
// ============================================================================
py::object
Layer::get_potentials (void)
{
    if ((map == 0) || (map->width == 0)) {
        PyErr_SetString(PyExc_AssertionError, "layer has no shape");
        py::throw_error_already_set();
        return py::object();
    }

    PyArrayObject *array = (PyArrayObject *) potentials.ptr();
    if ((map->height != PyArray_DIM(array, 0)) ||
        (map->width  != PyArray_DIM(array, 1))) {
        npy_intp dims[2] = {map->height, map->width};    
        potentials = py::object(py::handle<>(PyArray_SimpleNew (2, dims, PyArray_FLOAT)));
    }
    PyArray_FILLWBYTE(array, 0);
    float *data = (float *) array->data;
    for (unsigned int i=0; i<units.size(); i++)
        data[i] = units[i]->potential;

    return py::extract<numeric::array>(potentials);
}

// ============================================================================
//    Boost wrapping code
// ============================================================================
void
Layer::python_export (void)
{
    using namespace boost::python;    
    register_ptr_to_python< boost::shared_ptr<Layer> >();

    import_array();
    boost::python::numeric::array::set_module_and_type("numpy", "ndarray");

    // member function pointers for overloading
    UnitPtr    (Layer::*get_index)(int) const = &Layer::get;
    UnitPtr    (Layer::*get_xy)(int, int) const = &Layer::get;
 

    class_<Layer, bases <Object> >("Layer",
    "======================================================================\n"
    "\n"
    "A layer is a shaped set of homogeneous units that are evaluated\n"
    "synchronously but in random order. The shape of the layer is directly\n"
    "inherited from the map it belongs to. As long as a layer is not\n"
    "part of a map, it does not possess any shape (and therefore, cannot.\n"
    "be filled.\n"
    "\n"
    "Attributes:\n"
    "-----------\n"
    "   spec: specification for the layer\n"
    "   map:  owning map (if any)\n"
    "\n"
    "======================================================================\n",
        init<>(
        "__init__() -- initializes layer\n")
        )

        .def_readwrite ("spec", &Layer::spec)

        .add_property("map", 
          make_function (&Layer::get_map,
                            return_value_policy<reference_existing_object>()))

        .def ("compute_dp", &Layer::compute_dp,
        "compute_dp() -> float -- computes potentials and return dp\n")

        .def ("compute_dw", &Layer::compute_dw,
        "compute_dw() -> float -- computes weights and returns dw\n")

        .def ("__len__", &Layer::size,
        "__len__() -> integer -- return number of units\n")
        
//        .def ("append", &Layer::append,
//        "append(unit) -- append unit to end\n")
        
        .def ("__getitem__", get_index,
        "x.__getitem__ (y)  <==> x[y]\n\n"
        "Use of negative indices is supported.\n")
        
        .def ("unit", get_index,
        "unit(index) -> unit -- get unit at index\n\n"
        "Use of negative indices is supported.\n")

        .def ("unit", get_xy,
        "unit(x,y) -> unit -- get unit at (x,y) coordinates\n\n"
        "Use of negative indices is supported.\n")

        .def ("fill", &Layer::fill,
        "fill(type) -> integer -- fill layer with type object")
        
        .def ("clear", &Layer::clear,
        "clear() -- remove all units\n")
        
        .def ("potentials", &Layer::get_potentials,
        "potentials() -> numpy::array -- get units potential as an array")
        ;
}
