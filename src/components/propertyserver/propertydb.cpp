#include "propertydb.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace propserver {

PropertyDb::PropertyDb()
{
}

PropertyDb::~PropertyDb()
{
}

void 
PropertyDb::addProperties( const std::map<std::string,std::string> &newProperties )
{
    // Copy them into our properties
    for ( std::map<string,string>::const_iterator it=newProperties.begin(); 
          it!=newProperties.end();
          ++it ) 
    {
        properties_[it->first] = it->second;
    }
}

std::string
toString( const std::map<std::string,std::string> &properties )
{
    stringstream ss;
    ss << endl;

    for ( map<string,string>::const_iterator it = properties.begin();
          it != properties.end();
          it++ )
    {
        ss << "  " << it->first << "=" << it->second << endl;
    }
    return ss.str();
}

}

