/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef QORCACM_CALLS_H
#define QORCACM_CALLS_H

//#include <orca/home.h>

#include "callqueue.h"

namespace qorcacm
{
/*
class GetHomeCall : public Call
{
public:
    GetHomeCall( const orca::HomePrx & home ) :
        home_(home) {}

    virtual void execute() {};

private:
    const orca::HomePrx home_;
};
*/

class GetComponentsCall : public Call
{
public:
    GetComponentsCall( const orcaice::Context & context, const std::string & locatorString,
                       orcaice::Notify<orcacm::ComponentData>* homePipe );

    virtual void execute();

private:
    orcaice::Context context_;
    std::string locatorString_;
    orcaice::Notify<orcacm::ComponentData>* homePipe_;
};


} // namespace

#endif

