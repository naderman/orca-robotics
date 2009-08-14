/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_CONTEXT_H
#define HYDROUTIL_CONTEXT_H

#include <string>
#include <hydroutil/properties.h>
#include <gbxutilacfr/tracer.h>
#include <gbxutilacfr/status.h>
#include <hydroportability/sharedlib.h>

namespace hydroutil
{

/*!
 *  @brief Driver's properties, tracer, status etc.
 *
 *  Contains pointers to driver's tracer and status handler. Makes a copy of driver properties. 
 *  With this class it's easy to pass all this information to at the time of driver instantiation.
 *
 *  A note on thread safety. None of access functions are thread-safe. However, all
 *  object which are referenced are themselves thread-safe. 
 *  
 *  This means that once a copy of of Context is created, it is safe to use it from different threads.
 *  Pass context by const reference and store a copy, e.g. a class definition would look like this:
@verbatim
class MyClass
{
public:
    MyClass( const hydroutil::Context & context )
        : context_(context) {};
private:
    hydroutil::Context context_;
}
@endverbatim
 */
class SOEXPORT Context
{

public:
    //! Constructor
    Context( const hydroutil::Properties& props, gbxutilacfr::Tracer& tr, gbxutilacfr::Status& st ) :
        properties_(props),
        tracer_(&tr),
        status_(&st) {};

    //! Const access to driver's properties.
    const hydroutil::Properties&   properties() const { return properties_; };
    //! Access to driver's properties.
    hydroutil::Properties&   properties() { return properties_; };
    //! Access tracing functions.
    gbxutilacfr::Tracer&       tracer() const { return *tracer_; };
    //! Access status functions.
    gbxutilacfr::Status&       status() const { return *status_; };

private:
    // this is our own copy of properties (they are not shared)
    hydroutil::Properties   properties_;
    // Tracer and Status objects are shared
    // we store pointers but return references.
    // it's possible to store references but then a custom copy constructor needs to be defined
    // see http://www.dbforums.com/archive/index.php/t-660589.html
    gbxutilacfr::Tracer*      tracer_;
    gbxutilacfr::Status*      status_;
};

} // end namespace

#endif
