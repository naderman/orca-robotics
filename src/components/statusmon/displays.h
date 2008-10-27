/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ABSTRACT_DISPLAY_H
#define ABSTRACT_DISPLAY_H

#include "componentmonitor.h"
#include <orcaifaceimpl/systemstatusImpl.h>

namespace statusmon
{
    
class AbstractDisplay
{
    public:
        
        AbstractDisplay( const orcaice::Context &context )
            : context_(context)
        {};
        
        virtual ~AbstractDisplay() {};
        
        virtual void setSystemStatus( const std::vector<StatusDetails> &systemStatusDetails ) = 0;
        
    protected:
        
        orcaice::Context context_;
};

class TextDisplay : public AbstractDisplay
{
    public:
    
        TextDisplay( const orcaice::Context &context );
        
        virtual void setSystemStatus( const std::vector<StatusDetails> &systemStatusDetails );
          
};

class InterfaceDisplay : public AbstractDisplay
{
    public:
        InterfaceDisplay( const orcaice::Context &context,
                          gbxiceutilacfr::Thread *thread );
            
        virtual void setSystemStatus( const std::vector<StatusDetails> &systemStatusDetails );
        
    private:
        orcaifaceimpl::SystemStatusImplPtr iface_;
};


}

#endif

