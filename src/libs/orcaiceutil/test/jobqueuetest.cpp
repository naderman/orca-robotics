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
#include <orcaiceutil/jobqueue.h>
#include <orcaiceutil/simpletracer.h>

using namespace std;

class TestJob : public orcaiceutil::Job 
{
public:
    TestJob( orcaiceutil::JobQueueUser* user ) :
        orcaiceutil::Job( user ) {};
    
    virtual orcaiceutil::JobStatusPtr process()
    {
        return new orcaiceutil::OkJobStatus(jobQueueUser());
    };

    virtual std::string toString() const
    {
        return "TestJob";
    };
};

int main(int argc, char * argv[])
{
    cout<<"starting job queue with thread pool 1 ... ";
    {
        orcaiceutil::SimpleTracer tracer;
        orcaiceutil::JobQueue jobQueue( &tracer, 1, 1 );
        jobQueue.start();
        orcaiceutil::JobQueueUser* fakeUser = 0;
        orcaiceutil::JobPtr j = new TestJob( fakeUser );
        jobQueue.addJob( j );
    }
    cout<<"ok"<<endl;
    
    cout<<"starting job queue with thread pool 2 ... ";
    {
        orcaiceutil::SimpleTracer tracer;
        orcaiceutil::JobQueue jobQueue( &tracer, 2, 1 );
        jobQueue.start();
        orcaiceutil::JobQueueUser* fakeUser = 0;
        orcaiceutil::JobPtr j = new TestJob( fakeUser );
        jobQueue.addJob( j );
    }
    cout<<"ok"<<endl;
    
    return EXIT_SUCCESS;
}
