#ifndef LOGPLAYER_INTERFACETYPECOUNTER_H
#define LOGPLAYER_INTERFACETYPECOUNTER_H

#include <map>
#include <string>

namespace logplayer {

//
// @brief Counts the number of instances of interfaces of a particular type.
//
// @author Alex Brooks
//
class InterfaceTypeCounter
{

public: 

    // Hands out an ID that's unique for the specified interface type
    // (and remembers that it's been handed out)
    int nextAvailableId( std::string interfaceType );

private: 

    std::map< std::string, int > lastHandedOut_;

};

}

#endif
