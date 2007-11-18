#ifndef COMMANDHISTORY_SEGWAYRMP_H
#define COMMANDHISTORY_SEGWAYRMP_H

#include <sys/types.h>
#include <deque>
#include <string>

namespace segwayrmp {

//
// @brief remembers the history of commands sent to the Segway, so
//        we can check that the last command it received was recently sent by us.
//
// @author Alex Brooks
//
class CommandHistory
{

public: 

    CommandHistory( int historyLength );

    void setCommand( int16_t speed, int16_t turnrate );
    void checkReceivedCommandWasRecentlySent( int16_t rxSpeed, int16_t rxTurnrate,
                                              bool &speedRecentlySent, bool &turnrateRecentlySent ) const;

    std::string recentlySentSpeeds() const { return dequeAsString( sentSpeeds_ ); }
    std::string recentlySentTurnrates() const { return dequeAsString( sentTurnrates_ ); }

private: 

    std::string dequeAsString( const std::deque<int16_t> &d ) const;

    const int historyLength_;

    std::deque<int16_t> sentSpeeds_;
    std::deque<int16_t> sentTurnrates_;

};

}

#endif
