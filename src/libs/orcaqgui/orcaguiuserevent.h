/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAGUI_USEREVENT_H
#define ORCAGUI_USEREVENT_H

#include <string> 
 
namespace orcaqgui {
 
class OrcaGuiUserEvent
{
    
public:
    OrcaGuiUserEvent()
    {};
    
    virtual ~OrcaGuiUserEvent() {};  
    virtual std::string type() = 0;
     

private:    
    std::string platform_;
    
};

}

#endif
