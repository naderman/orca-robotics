/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
// **********************************************************************
//
// Copyright (c) 2003-2006 ZeroC, Inc. All rights reserved.
//
// This copy of Ice is licensed to you under the terms described in the
// ICE_LICENSE file included in this distribution.
//
// **********************************************************************

//
// this file is a modified copy of ZeroC's Application.h v.3.1.0
//

#ifndef ORCAICE_ICEAPPLICATION_H
#define ORCAICE_ICEAPPLICATION_H

#include <Ice/Ice.h>

namespace orcaice
{

//
// This is Ice::Application, cut-and-pasted from the Ice source v.3.0.0.
// The only difference is that there's an extra flag passed to the constructor
// that enables/disables setting up a Ctrl-C handler.
//
// orcaice::Application derives from this, and adds stuff specific to libOrcaIce
//
class IceApplication : private IceUtil::noncopyable
{
public:

    IceApplication( bool installCtrlCHandler );
    virtual ~IceApplication();

    //
    // This main() must be called by the global main(). main()
    // initializes the Communicator, calls run() as a template method,
    // and destroys the Communicator upon return from run(). It
    // thereby handles all exceptions properly, i.e., error messages
    // are printed if exceptions propagate to main(), and the
    // Communicator is always destroyed, regardless of exceptions.
    //
// orca start
    virtual int main(int, char*[]);
// orca end
    int main(int, char*[], const char*);
    int main(int, char*[], const Ice::InitializationData&);
    ICE_DEPRECATED_API int main(int, char*[], const char*, const Ice::LoggerPtr&);

    virtual int run(int, char*[]) = 0;

    //
    // Return the application name, i.e., argv[0].
    //
    static const char* appName();

    //
    // One limitation of this class is that there can only be one
    // Application instance, with one global Communicator, accessible
    // with this communicator() operation. This limitation is due to
    // how the signal handling functions below operate. If you require
    // multiple Communicators, then you cannot use this Application
    // framework class.
    //
    static Ice::CommunicatorPtr communicator();

    //
    // These methods can be used to set a Ctrl+C Handler callback.
    //
    static void destroyOnInterrupt();
    static void shutdownOnInterrupt();
    static void ignoreInterrupt();

    //
    // These methods can be used to temporarily block a signal and
    // arrange for delivery of a pending signal later. Any signal that
    // is received after holdInterrupt() was called is remembered and
    // delivered when releaseInterupt() is called. That signal is then
    // handled according to the signal disposition established with
    // destroyOnInterrupt(), shutdownOnInterrupt() or
    // ignoreInterrupt().
    //
    static void holdInterrupt();
    static void releaseInterrupt();

    //
    // This method returns true if a signal handler was triggered,
    // false otherwise. This can be used once
    // Communicator::waitForShutdown() returns to test whether the
    // shutdown was due to an interrupt (interrupted() returns true in
    // that case) or because Communicator::shutdown() was called
    // (interrupted() returns false in that case).
    //
    static bool interrupted();


#if defined(__SUNPRO_CC)
//
// Sun C++ 5.x does not like classes with no data members 
//
private:
    char _dummy;
#endif

};
}

#endif
