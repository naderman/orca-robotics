/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <iostream>

#include "ogfusionI.h"

using namespace std;
using namespace ognode;

OgFusionI::OgFusionI( orca::OgFusionConfig& config,
                      gbxiceutilacfr::Buffer<orca::OgFusionData>& OgFusionDataBuffer )
    : config_(config),
      ogFusionDataBuffer_(OgFusionDataBuffer)
{
}

void
OgFusionI::setData(const orca::OgFusionData& data, const Ice::Current& context)
{
    //cout << "got an object with " << data->observation.size() << " cells\n";
    ogFusionDataBuffer_.push(data);
}

orca::OgFusionConfig
OgFusionI::getConfig(const ::Ice::Current&)
{
    return config_;
}
