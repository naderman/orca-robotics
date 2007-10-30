#ifndef ORCAROBOTDRIVERUTIL_STATEMACHINE_H
#define ORCAROBOTDRIVERUTIL_STATEMACHINE_H

#include <string>
#include <hydroutil/store.h>

namespace orcarobotdriverutil {

//!
//! @brief A thread-safe state machine useful when writing robot hardware drivers
//!
//! @author Alex Brooks
//!
class StateMachine
{

public: 

    //! Default state is 'fault: "Initialising"'
    StateMachine();

    void setWarning( const std::string &reason );
    void setFault( const std::string &reason );
    void setOK();
    bool isFault();
    bool isFault( std::string &reason );
    bool isWarning();
    bool isWarning( std::string &reason );
    bool isWarnOrFault();

private: 

    ////////////////////////////////////////
    enum StateType {
        StateTypeOK,
        StateTypeWarning,
        StateTypeFault
    };
    class State {
    public:
        State() {}
        State( StateType s )
            {
                stateType = s;
                reason = "";
            }
        State( StateType s, const std::string &r )
            {
                stateType = s;
                reason = r;
            }

        StateType   stateType;
        std::string reason;
    };
    ////////////////////////////////////////

    hydroutil::Store<State> stateStore_;

};

}

#endif
