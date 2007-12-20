#ifndef PROPSERVER_PROPERTYDB_H
#define PROPSERVER_PROPERTYDB_H

#include <string>
#include <map>

namespace propserver {

//!
//! @brief Property Database
//!
//! @author Alex Brooks
//!
class PropertyDb
{

public: 

    PropertyDb();
    ~PropertyDb();

    // Overwrites any properties that might already be set.
    void addProperties( const std::map<std::string,std::string> &newProperties );
    const std::map<std::string,std::string> &properties() const { return properties_; }

private: 

    std::map<std::string,std::string> properties_;

};

std::string toString( const std::map<std::string,std::string> &properties );

}

#endif
