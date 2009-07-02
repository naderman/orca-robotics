/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCASYSTEMSTATUSUTIL_STRING_UTILS_H
#define ORCASYSTEMSTATUSUTIL_STRING_UTILS_H

#include <orca/systemstatus.h>

namespace orcasystemstatusutil {

//! Returns icons corresponding to component states.
std::string toIcon( const orca::ComponentState& );

//! Returns strings corresponding to component states.
std::string toString( const orca::ComponentState& );

//! Returns more readable output than the auto-generated functions. 
std::string toString( const orca::ComponentHealth& );

//! Meshes (reported) health and (observed) stall status into a single string.
std::string toString( const orca::ComponentHealth& health, bool isStalled );

//! Returns return-separated listing of brief information on all subsystems currently
//! in Initialising state.
std::string toStringIntialisingOnly( const orca::SubsystemStatusDict& subsystems, int indent=0 );

//! Returns stringified warnings, errors, stalls, and information messages.
std::string toStringImportantOnly( const orca::SubsystemStatusDict& s, int indent=0 );
//! Returns stringified warnings, errors, stalls, and information messages.
std::string toStringImportantOnly( const orca::ComponentStatus& s, int indent=0 );
//! Returns stringified stales, warnings, errors, stalls, and information messages.
std::string toStringImportantOnly( const orca::EstimatedComponentStatus& s, int indent=0 );

}

#endif
