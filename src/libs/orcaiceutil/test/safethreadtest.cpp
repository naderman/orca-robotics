/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include <IceUtil/Time.h>

#include <orcaiceutil/safethread.h>

using namespace std;

class TestThread : public orcaiceutil::SafeThread
{
public:    
    virtual void walk()
    {
        while ( !isStopping() ) {
            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(50));
        }
    };
};

class TestThreadWithThrow : public orcaiceutil::SafeThread
{
public:
    virtual void walk()
    {
        throw "throwing from walk";
    };
};

int main(int argc, char * argv[])
{
    cout<<"testing start() and stop()... ";
    {
        orcaiceutil::Thread* t=0;
        try
        {
            t = new TestThread;
            t->start();
        }
        catch (...)
        {
            cout<<"failed"<<endl<<"should be able to create thread"<<endl;
            exit(EXIT_FAILURE);
        }
        IceUtil::ThreadControl tc = t->getThreadControl();
        t->stop();
        tc.join();
        // do not delete t! it's already self-destructed.
    }
    cout<<"ok"<<endl;

    cout<<"testing SafeThread() with exceptions... ";
    {
        orcaiceutil::Thread* t=0;
        try
        {
            t = new TestThreadWithThrow;
            t->start();
        }
        catch (...)
        {
            cout<<"failed"<<endl<<"all exception should've been caught."<<endl;
            // ok
        }
        IceUtil::ThreadControl tc = t->getThreadControl();
        t->stop();
        tc.join();
        // do not delete t! it's already self-destructed.
    }
    cout<<"ok"<<endl;

    return EXIT_SUCCESS;
}
