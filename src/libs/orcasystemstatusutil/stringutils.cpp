/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "stringutils.h"

using namespace std;

namespace orcasystemstatusutil {

std::string toIcon( const orca::ComponentState& state )
{
    switch( state )
    {
        case orca::CompInitialising:
            return "^";
        case orca::CompWorking:
            return ">";
        case orca::CompFinalising:
            return "v";
        default:
            assert( !"Unknown component state." );
    }
}

std::string toString( const orca::ComponentState& state )
{
    switch( state )
    {
        case orca::CompInitialising:
            return "Initialising";
        case orca::CompWorking:
            return "Working";
        case orca::CompFinalising:
            return "Finalising";
        default:
            assert( !"Unknown component state." );
    }
}

std::string toString( const orca::ComponentHealth& health )
{
    switch ( health )
    {
        case orca::CompOk :
            return "OK";
        case orca::CompWarning :
            return "Warning";
        case orca::CompFault :
            return "Fault";
        default :
            assert( !"Unknown component health." );
    }
}

std::string toString( const orca::ComponentHealth& health, bool isStalled )
{
    if ( isStalled )
        return "Stalled";
    else
        return toString( health );
}


std::string toStringIntialisingOnly( const orca::SubsystemStatusDict& s, int indent )
{
    string ind;
    for ( int i=0; i < indent; ++i ) ind += " ";

    stringstream ss;
    bool wroteOneItem = false;
    for ( orca::SubsystemStatusDict::const_iterator it=s.begin(); it!=s.end(); ++it ) {
        if ( it->second.state != orca::SubsystemInitialising && it->second.state != orca::SubsystemIdle )
            continue;

        if ( wroteOneItem ) 
            ss << endl;
        else
            wroteOneItem = true;
    
        ss << ind;

        if ( it->second.state == orca::SubsystemInitialising )
            ss << "INIT ";
        else 
            ss << "IDLE ";

        ss << it->first
           << "\t: " 
           << it->second.message;
    }

    return ss.str();
}

std::string toStringAlertOnly( const orca::SubsystemStatusDict& s, int indent )
{
    string ind;
    for ( int i=0; i < indent; ++i ) ind += " ";

    stringstream ss;
    bool wroteOneItem = false;
    for ( orca::SubsystemStatusDict::const_iterator it=s.begin(); it!=s.end(); ++it ) {
        if ( it->second.health == orca::SubsystemOk && !it->second.isStalled )
            continue;

        if ( wroteOneItem ) ss << endl;
        wroteOneItem = true;
    
        ss << ind;

        if ( it->second.isStalled )
            ss << "STALL ";
        else {
            if ( it->second.health == orca::SubsystemWarning )
                ss << "WARN  ";
            else
                ss << "FAULT ";
        }

        ss << it->first
           << "\t: " 
           << it->second.message;
    }

    return ss.str();
}

std::string toStringImportantOnly( const orca::ComponentStatus& s, int indent )
{    
    if ( s.health != orca::CompOk || s.isStalled )
        return toStringAlertOnly( s.subsystems, indent );
    else
        return string();
}

std::string toStringImportantOnly( const orca::EstimatedComponentStatus& s, int indent )
{
    bool somethingWrong = false;
    if ( s.estimatedState == orca::EstCompStale )
        somethingWrong = true;
    if ( !s.reportedStatus.empty() && 
         s.reportedStatus[0].health != orca::CompOk )
        somethingWrong = true;
    if ( !s.reportedStatus.empty() && 
         s.reportedStatus[0].isStalled )
        somethingWrong = true;

    if ( !somethingWrong )
        return string();

    string ind;
    for ( int i=0; i < indent; ++i ) ind += " ";

    stringstream ss;

    switch( s.estimatedState )
    {
        case orca::EstCompInactive :
        {
            ss << "No status updates: component inactive or network fault.";
            break;
        }
        case orca::EstCompReporting :
        {
            assert( !s.reportedStatus.empty() && "Empty reported status in EstCompReporting state." );
//             ss << ifaceutil::toString( s );
            ss << toStringImportantOnly( s.reportedStatus[0], indent+2 );
            break;
        }
        case orca::EstCompStale :
        {
            assert( !s.reportedStatus.empty() && "Empty reported status in EstCompReporting state." );
            ss << "Component status is STALE. Last received status: "
//                << ifaceutil::toString( s );
               << toStringImportantOnly( s.reportedStatus[0], indent+2 );
            break;
        }
        default :
            assert( !"Unknown estimated component state." );
    }

    return ss.str();
}

}
