/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_CAMERA_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_CAMERA_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/camera.h>

namespace orcaprobefactory
{

class CameraProbe : public orcaprobe::InterfaceProbe
{

public:

    CameraProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperation( const int index );
    
private:

    int loadGetData();
    int loadGetDescription();
    int loadSubscribe();
    int loadUnsubscribe();
    
//     orca::RangeScanner2dConsumer consumerPrx_;

};

} // namespace

#endif
