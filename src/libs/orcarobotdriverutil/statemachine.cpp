#include "statemachine.h"
#include <iostream>

using namespace std;

namespace orcarobotdriverutil {

StateMachine::StateMachine()
{
    // Assume something's wrong till we know otherwise
    setFault( "Initialising" );
}

void
StateMachine::setWarning( const std::string &reason )
{
    stateStore_.set( State( StateTypeWarning, reason ) );
}

void
StateMachine::setFault( const std::string &reason )
{
    stateStore_.set( State( StateTypeFault, reason ) );
}

void
StateMachine::setOK()
{
    stateStore_.set( State( StateTypeOK ) );
}

bool
StateMachine::isFault()
{
    State s;
    stateStore_.get( s );
    return ( s.stateType == StateTypeFault );
}

bool
StateMachine::isFault( std::string &reason )
{
    State s;
    stateStore_.get( s );
    reason = s.reason;
    return ( s.stateType == StateTypeFault );
}

bool
StateMachine::isWarning()
{
    State s;
    stateStore_.get( s );
    return ( s.stateType == StateTypeWarning );
}

bool
StateMachine::isWarning( std::string &reason )
{
    State s;
    stateStore_.get( s );
    reason = s.reason;
    return ( s.stateType == StateTypeWarning );
}


}

