/*
  DANA - Distributed Asynchronous Numerical Adaptive computing library
  Copyright (c) 2006,2007,2008 Nicolas P. Rougier

  This file is part of DANA.

  DANA is free software: you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DANA is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License
  along with DANA. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __DANA_CORE_OBSERVABLE_H__
#define __DANA_CORE_OBSERVABLE_H__

#include <vector>
#include "object.h"
#include "event.h"


namespace dana { namespace core {

typedef boost::shared_ptr<class Observable> ObservablePtr;
typedef boost::shared_ptr<class Observer> ObserverPtr;

// _____________________________________________________________class Observable
class Observable {

    // _______________________________________________________________attributes
public:
    std::vector<ObserverPtr>  observers;
        
public:
    // _____________________________________________________________________life
    Observable (void);
    virtual ~Observable (void);

    // _____________________________________________________________________main
    void attach (ObserverPtr observer, EventPtr event);
    void detach (ObserverPtr observer);
    void notify (EventPtr event);
        
    // ___________________________________________________________________export
    static void python_export (void);
};

}}

#endif
