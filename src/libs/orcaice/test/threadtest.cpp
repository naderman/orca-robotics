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

#include <IceUtil/Time.h>

#include <orcaice/thread.h>

using namespace std;

class TestThread : public orcaice::Thread
{
public:
    TestThread( bool shouldIThrow )
    {
        if ( shouldIThrow ) {
            throw "trowing from constructor";
        }
    };
    
    virtual void run()
    {
        while ( isActive() ) {
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(50));
        }
    };
};

int main(int argc, char * argv[])
{
    cout<<"testing start() and stop()... ";
    TestThread* t1=0;
    try
    {
        t1 = new TestThread( false );
        t1->start();
    }
    catch (...)
    {
        t1 = 0;
    }
    if ( t1 == 0 ) {
        cout<<"failed"<<endl<<"should be able to create thread"<<endl;
        exit(EXIT_FAILURE);
    }
    IceUtil::ThreadControl tc1 = t1->getThreadControl();
    t1->stop();
    tc1.join();
    // do not delete t1! it's already self-destructed.
    cout<<"ok"<<endl;

    cout<<"testing start() and stop() with exceptions... ";
    TestThread* t2=0;
    try
    {
        t2 = new TestThread( true );
        t2->start();
    }
    catch (...)
    {
        t2 = 0;
    }
    if ( t2 != 0 ) {
        cout<<"failed"<<endl<<"should not be able to create thread"<<endl;
        exit(EXIT_FAILURE);
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
