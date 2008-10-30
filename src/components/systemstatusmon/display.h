/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef DISPLAY_H
#define DISPLAY_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaifaceimpl/printingconsumers.h>
#include <orcaifaceimpl/bufferedconsumers.h>
#include <orcaice/context.h>

namespace systemstatusmon
{
    
class Display
{
    public:
        virtual ~Display() {};
        virtual void refresh()=0;
};
    
class SimpleDisplay : public Display
{
    public:
        SimpleDisplay( const orcaice::Context &context, gbxiceutilacfr::SafeThread *thread)
        {
            consumer_ = new orcaifaceimpl::PrintingSystemStatusConsumerImpl( context );
            consumer_->subscribeWithTag( "SystemStatus", thread );
        }
        void refresh() {};
        
    private:
        orcaifaceimpl::PrintingSystemStatusConsumerImplPtr consumer_;
};    

class ColourTextDisplay : public Display
{
    public:
        ColourTextDisplay( const orcaice::Context &context, gbxiceutilacfr::SafeThread *thread);
        void refresh();
        
    private:
        orcaifaceimpl::BufferedSystemStatusConsumerImplPtr consumer_;
        
        void display( const orca::SystemStatusData &data );
};

}

#endif
