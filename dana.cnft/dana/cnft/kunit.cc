//
// Copyright (C) 2007,2006 Nicolas Rougier
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.

#include "core/map.h"
#include "core/layer.h"
#include "core/link.h"
#include "kunit.h"
#include "spec.h"

using namespace boost::python::numeric;
using namespace dana;
using namespace dana::cnft;

// ============================================================================
//  constructor
// ============================================================================
KUnit::KUnit(void) : core::Unit()
{}

// ============================================================================
//  destructor
// ============================================================================
KUnit::~KUnit(void)
{}

// ============================================================================
//  computes potential and returns dp
// ============================================================================
float
KUnit::compute_dp (void)
{
    core::SpecPtr sp = layer->get_spec();
    Spec *s = dynamic_cast<Spec *>(sp.get());

    if (!s) {
        PyErr_SetString(PyExc_TypeError, "Bad specification");
        throw_error_already_set();
    }
    
    float tau      = s->tau;
    float alpha    = s->alpha;
    float baseline = s->baseline;
    float min_act  = s->min_act;
    float max_act  = s->max_act;
    float wm = 1;
    float wp = 1;
    
	float input = 0;
    unsigned int size = afferents.size();
	for (unsigned int i=0; i<size; i++)
		input += afferents[i]->weight * afferents[i]->source->potential;

	float lateral_p = 0;
	float lateral_m = 0;
    size = laterals.size();

	for (unsigned int i=0; i<size; i++) {
        if ((laterals[i]->source->potential > 0) || (laterals[i]->weight > 0)){
            float u = laterals[i]->weight * laterals[i]->source->potential;
            if (u >= 0)
                lateral_p += u;
            else
                lateral_m += u;
//            lateral += laterals[i]->weight * laterals[i]->source->potential;
        }
    }

    float lateral = wp*lateral_p + wm*lateral_m;
    
    float du = (-potential + baseline + (1.0f/alpha)*(lateral + input)) / tau;
	float value = potential;
	potential += du;

	if (potential < min_act)
        potential = min_act;

	if (potential > max_act)
        potential = max_act;

	return value-potential;
}

// ============================================================================
//  computes weights and returns dw
// ============================================================================
float
KUnit::compute_dw (void)
{
    return 0.0f;
}

// ============================================================================
//    Boost wrapping code
// ============================================================================
void
KUnit::boost (void) {

    using namespace boost::python;
    register_ptr_to_python< boost::shared_ptr<KUnit> >();
    
    class_<KUnit, bases<core::Unit> >("KUnit",
    "======================================================================\n"
    "\n"
    "A unit is a potential that is computed on the basis of some external\n"
    "sources that feed the unit. Those sources can be anything as long as\n"
    "they have some potential.\n"
    "\n"
    "Attributes:\n"
    "-----------\n"
    "   potential : unit potential (float)\n"
    "   position  : unit position within layer (tuple, read only)\n"
    "\n"
    "======================================================================\n",
        init<>(
        "__init__ () -- initialize unit\n")
        )
        ;
}
