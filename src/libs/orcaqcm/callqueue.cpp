/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <Ice/Ice.h>

#include "callqueue.h"

using namespace std;
using namespace qorcacm;

CallQueue::CallQueue()
    : destroy_(false)
{
}

void
CallQueue::add(const CallPtr& req)
{
    Lock sync(*this);
    if(exception_.get())
    {
        exception_->ice_throw(); // Propagate exceptions to the main thread.
    }
    requests_.push_back(req);
    cout<<"queue length : "<<requests_.size()<<endl;
    notify();
}

void
CallQueue::stop()
{
    Lock sync(*this);
    destroy_ = true;
    notify();
}

bool
CallQueue::isActive()
{
    Lock sync(*this);
    return destroy_;
};

void
CallQueue::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    while(true)
    {
        CallPtr req;
        {
            Lock sync(*this);
            while(!destroy_ && requests_.empty())
            {
                wait();
            }
            if(destroy_)
            {
                // exit from the while loop
                break;
            }
            // got something
            req = requests_.front();
            requests_.pop_front();
        }
        
        try
        {
            cout<<"executing job..."<<endl;
            req->execute();
        }
        catch(const Ice::Exception& e)
        {
            Lock sync(*this);
            exception_ = auto_ptr<Ice::Exception>(e.ice_clone());
            destroy_ = true;
        }
        // these include orcaice::Exception's which derive from std::exception
        // normally will get an exception from Notify when the handler is destroyed on shutdown
        catch ( const std::exception & e )
        {
            //cout<<e.what()<<endl;
            //cout<<"unexpected std exception."<<endl;
            Lock sync(*this);
            destroy_ = true;
        }
    }

    //
    // unexpected exceptions
    //
    } // try
    catch ( ... )
    {
        cout<<"unexpected exception from somewhere."<<endl;
        // just terminate this thread
    }

    cout<<"Call queue terminating..."<<endl;
}
