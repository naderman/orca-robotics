/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_APPLICATION_H
#define ORCAICE_APPLICATION_H

#include <Ice/Ice.h>

#include <orcaice/iceapplication.h>

namespace orcaice
{

class Component;

/*!
 *   @brief Similar to Ice::Application, plus some Orca-specifc stuff.
 *   
 *   Typical usage:
@verbatim
int main(int argc, char * argv[])
{
    MyComponent component;
    orcaice::Application app( component );
    return app.main(argc, argv);
}
@endverbatim
 *
 *   @author Alex Brooks
 *   @sa Service, Component
 */
class Application : public orcaice::IceApplication
{
public:

    /*!
     *  Will start the @e component when @c main is called.
     * 
     *  The flag @e installCtrlCHandler determines whether the Ctl-C handler is installed. By default it is and
     *  the handler destroys the communicator. This triggers component shutdown, if and only if
     *  the application is blocked in waitForCommunicatorShutdown(), i.e. if Communicator::start() returns
     *  control to application. Most console components should do that.
     *
     *  GUI components implemented in Qt are a notable exception. A Qt app::exec() called in
     *  Communicator::start() does not return. For such components set installCtrlCHandler=false and
     *  remember to call Communicator::destroy() or Communicator::shutdown() before returning
     *  from Component::start().
     *
     */
    Application( orcaice::Component &component, bool installCtrlCHandler=true );
    ~Application();

    //!
    //! Reimplements one of the main functions from Ice::Application
    //!
    virtual int main(int, char*[]);

    //
    // Implements the run function from Ice::Application
    // Not part of OrcaIce public API.
    //
    virtual int run(int, char*[]);

private:

    // By convention there is exactly one adapter per component and, therefore, per application
    // Keep the pointer to it here, so it does not get destroyed too soon.
    Ice::ObjectAdapterPtr   adapter_;

    // An application contains only one Component.
    orcaice::Component &component_;
};

} // namespace

#endif
