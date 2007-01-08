/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_VELOCITYCONTROL2D_I_H
#define ORCA2_SEGWAY_RMP_VELOCITYCONTROL2D_I_H

#include <orca/velocitycontrol2d.h>
#include <orcaice/notify.h>

namespace segwayrmp
{

// serves latest data on demand and accepts commands
class VelocityControl2dI : public orca::VelocityControl2d
{
public:
    VelocityControl2dI( const orca::VelocityControl2dDescription& descr, 
                 orcaice::Notify<orca::VelocityControl2dData>& commandsPipe );

    virtual ::orca::VelocityControl2dDescription getDescription(const ::Ice::Current& ) const;

    virtual void setCommand(const ::orca::VelocityControl2dData&, const ::Ice::Current& );

private:
    orca::VelocityControl2dDescription descr_;

    orcaice::Notify<orca::VelocityControl2dData> &commandPipe_;
};

} // namespace

#endif
