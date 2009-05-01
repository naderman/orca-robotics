/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

/*
 *  Convenience header: an easy way to include all of orcaice header files
 */

#ifndef ORCAICE_ORCAICE_H
#define ORCAICE_ORCAICE_H

// orcaice headers
#include <orcaice/exceptions.h>
#include <orcaice/subsystemthread.h>

#include <orcaice/configutils.h>
#include <orcaice/proputils.h>
#include <orcaice/convertutils.h>
#include <orcaice/stringutils.h>
#include <orcaice/timeutils.h>
#include <orcaice/catchutils.h>

#include <orcaice/iceutils.h>
#include <orcaice/multiiceutils.h>
#include <orcaice/connectutils.h>
#include <orcaice/multiconnectutils.h>
#include <orcaice/icestormutils.h>
#include <orcaice/multiicestormutils.h>

// this header contains specialized tools, not everyone needs them.
// #include <orcaice/icegridutils.h>

// mostly used internally during initialization
// #include <orcaice/printutils.h>

// these are rarely used or should be removed entirely
// #include <orcaice/ptrbuffer.h>
// #include <orcaice/ptrstore.h>
// #include <orcaice/ptrnotify.h>

// rarely used
// #include <orcaice/activator.h>

#endif
