//
// Copyright (C) 2006 Nicolas Rougier
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef __DANA_CORE_LINK_H__
#define __DANA_CORE_LINK_H__

#include <boost/python.hpp>
#include "object.h"
#include "unit.h"

using namespace boost::python;

namespace dana { namespace core {

    typedef boost::shared_ptr<class Unit> UnitPtr;
    
    class Link /* : public Object */ {
    public:
        UnitPtr	source;
        float	weight;

    public:
        Link (UnitPtr const src, float const w);
        virtual ~Link (void);
    
        UnitPtr	get_source (void) const;
        void	set_source (const UnitPtr src);
        float	get_weight (void) const;
        void	set_weight (const float w);

    public:
        static void	boost (void);
    };

}} // namespace dana::core

#endif
