/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include "componentmonitor.h"

namespace statusmon
{
    
class AbstractDisplay;
    
class MainThread : public gbxiceutilacfr::SafeThread
{
public:

    MainThread( const orcaice::Context & context );
    virtual ~MainThread();

    // from SafeThread
    virtual void walk();
    

private:
    
    hydroiceutil::JobQueuePtr jobQueue_;
    orcaice::Context context_;
    std::vector<ComponentMonitor> monitors_;
    
    AbstractDisplay *display_;
    void loadDisplay();
    
    // using a map assumes that there's only one status interface per component
    std::map<std::string,std::string> getComponentPlatformPairs();
    void createMonitors();
    
    


};

} // namespace

#endif
