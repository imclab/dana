//
// Copyright (C) 2006 Nicolas Rougier
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.

#ifndef __DANA_CORE_ENVIRONMENT_H__
#define __DANA_CORE_ENVIRONMENT_H__

#include <boost/python.hpp>
#include <vector>
#include "object.h"
#include "map.h"

using namespace boost::python;


namespace dana { namespace core {

    class Environment : public Object
    {
        public:
            class Network *         network;
            std::vector<MapPtr>     maps;
            
        public:
            // life management
            Environment (void);
            virtual ~Environment (void);
            
            // content management
            void attach (MapPtr map);
            
            // activity management
            virtual void evaluate  (void);

            // python export
            static void boost (void);
    };

}} // namespace dana::core

#endif