//
// Copyright (C) 2006 Nicolas Rougier
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.

#ifndef __DANA_LEARN_MUNIT_H__
#define __DANA_LEARN_MUNIT_H__

#include <boost/python.hpp>
#include "unit.h"
#include "core/layer.h"
#include "core/object.h"

using namespace boost::python;

namespace dana { namespace learn {

    class MUnit : public Unit {

        public:
            //  life management
            // =================================================================
        MUnit(void);
        virtual ~MUnit(void);
        float compute_dp(void);
        

        public:
            // python export
            // =================================================================
            static void boost (void);
    };
}} // namespace dana::learn

#endif