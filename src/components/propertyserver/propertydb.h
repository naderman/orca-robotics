#ifndef PROPSERVER_PROPERTYDB_H
#define PROPSERVER_PROPERTYDB_H

#include <string>
#include <map>

namespace propserver {

//
// @brief Property Database
//
// @author Alex Brooks
//
class PropertyDb
{

public: 

    PropertyDb();
    ~PropertyDb();

    // Overwrites any existing properties of the same name.
    void addProperties( const std::map<std::string,std::string> &newProperties );
    const std::map<std::string,std::string> &properties() const { return properties_; }

    void writePropertiesToFile( const std::string &filename );
    // Overwrites any existing properties of the same name.
    void readPropertiesFromFile( const std::string &filename );

private: 

    std::map<std::string,std::string> properties_;

};

std::string toString( const std::map<std::string,std::string> &properties );
void writeToFile( const std::string &filename, const std::map<std::string,std::string> &properties );
std::map<std::string,std::string> readFromFile( const std::string &filename );

}

#endif
