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
#include "sunit.h"
#include "cnft/spec.h"
#include "core/spec.h"
#include <math.h>

using namespace boost::python::numeric;
using namespace dana;
using namespace dana::learn;

// =============================================================================
//  constructor
// =============================================================================
SUnit::SUnit(void) : learn::Unit()
{}

// =============================================================================
//  destructor
// =============================================================================
SUnit::~SUnit(void)
{}

// ============================================================================
//  computes potential and returns dp
// ============================================================================
float
SUnit::compute_dp (void)
{
    core::SpecPtr sp = layer->get_spec();
    if (sp.get() == NULL)
        return 0.0f;
    cnft::Spec *s = dynamic_cast<cnft::Spec *>(sp.get());
    if (s == 0)
        return 0.0f;
    
    float tau      = s->tau;
    float alpha    = s->alpha;
    float baseline = s->baseline;
    float min_act  = s->min_act;
    float max_act  = s->max_act;

	float input = 0;
    unsigned int size = afferents.size();

	for (unsigned int i=0; i<size; i++)
		input += afferents[i]->weight * afferents[i]->source->potential;

	float lateral = 0;
    size = laterals.size();

	for (unsigned int i=0; i<size; i++)
        if ((laterals[i]->source->potential > 0) || (laterals[i]->weight > 0))
            lateral += laterals[i]->weight * laterals[i]->source->potential;

    float du = (-potential + baseline + (1.0f/alpha)*(lateral + input)) / tau;
	float value = potential;
	potential += du;
    
	//if (potential < min_act)
    //   potential = min_act;

	//if (potential > max_act)
    //    potential = max_act;

    potential = 1.0 / (1.0 + exp(-potential));
    //printf("potential : %f \n",potential);
    
	return value-potential;
}


// ============================================================================
//    Boost wrapping code
// ============================================================================
void
SUnit::boost (void) {

    using namespace boost::python;
    register_ptr_to_python< boost::shared_ptr<SUnit> >();
    
    class_<SUnit, bases<learn::Unit> >("SUnit",
    "======================================================================\n"
    "\n"
    "A unit is a potential that is computed on the basis of some external\n"
    "sources that feed the unit. Those sources can be anything as long as\n"
    "they have some potential."
    "This unit is a learner::Unit with a sigmoidal activation function \n"
    "\n"
    "\n"
    "Attributes (inherited from cnft.Unit):\n"
    "-----------\n"
    "   potential : unit potential (float) \n"
    "   position  : unit position within layer (tuple, read only)\n"
    "\n"
    "Methods\n"
    "----------\n"
    "   set_lrule : defines the learning rule to use\n"
    "   learn     : learn with a given learning rate\n"
    "\n"
    "======================================================================\n",
        init<>(
        "__init__ () -- initialize unit\n")
	);
}
