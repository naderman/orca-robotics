/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <orcaice/component.h>

namespace orcamon
{

class Component : public orcaice::Component
{
public:
    Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();
    
private:
    hydroiceutil::ThreadPtr thread_;
};

} // namespace