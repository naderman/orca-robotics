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

#include <gbxsickacfr/gbxiceutilacfr/timer.h>
#include <orcaice/subsystemthread.h>
#include <orcaifaceimpl/printingconsumers.h>
#include <orcaifaceimpl/bufferedconsumers.h>
#include <orcaice/context.h>

namespace systemstatusmon
{
    
//
// Abstract Display class
// 
// Author: Tobias Kaupp
// 
class Display
{
    public:
        virtual ~Display() {};
        virtual void refresh() = 0;
};
    
//
// A simple implementation of a Display: plain print-out of SystemStatusData on console
// 
class SimpleDisplay : public Display
{
    public:
        SimpleDisplay( const orcaice::Context   &context, 
                       orcaice::SubsystemThread *thread )
        {
            consumer_ = new orcaifaceimpl::PrintingSystemStatusConsumerImpl( context );
            consumer_->subscribeWithTag( "SystemStatus", thread );
        }
        void refresh() {};
        
    private:
        orcaifaceimpl::PrintingSystemStatusConsumerImplPtr consumer_;
};    

//
// An implementation of Display presenting SystemStatusData in a coloured matrix on the console
//
class ColourTextDisplay : public Display
{
    public:
        ColourTextDisplay( const orcaice::Context   &context, 
                           orcaice::SubsystemThread *thread );
        void refresh();
        
    private:
        
        orcaice::Context context_;
        gbxiceutilacfr::Timer timeSinceHeardTimer_;
        orcaifaceimpl::BufferedSystemStatusConsumerImplPtr consumer_;
        double publishIntervalSec_;
                
        void display( const orca::SystemStatusData &data );
};

}

#endif
