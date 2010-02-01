#include "propertydb.h"
#include <iostream>
#include <sstream>
#include <errno.h>
#include <string.h>
#include <gbxutilacfr/exceptions.h>
#include <hydroutil/stringutils.h>
#include <fstream>

using namespace std;

namespace hydropropertydb {

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
        string value = it->second;
        if ( value == "property-not-set" )
            value = "";
        properties_[it->first] = value;
    }
}

void
PropertyDb::writePropertiesToFile( const std::string &filename )
{
    writeToFile( filename, properties_ );
}
void
PropertyDb::readPropertiesFromFile( const std::string &filename )
{
    addProperties( readFromFile( filename ) ); 
}

std::string
toString( const std::map<std::string,std::string> &properties )
{
    stringstream ss;
    for ( map<string,string>::const_iterator it = properties.begin();
          it != properties.end();
          it++ )
    {
        ss << endl << it->first << "=" << it->second;
    }
    return ss.str();
}

void
writeToFile( const std::string &filename, const std::map<std::string,std::string> &properties )
{
    FILE *f = fopen( filename.c_str(), "w" );
    if ( !f )
    {
        stringstream ss;
        ss << "Failed to open file for writing: " << strerror(errno);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    fprintf( f, "%s\n", toString(properties).c_str() );

    int ret = fclose( f );
    if ( ret != 0 )
    {
        stringstream ss;
        ss << "Warning: Error closing file: " << strerror(errno);
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

std::map<std::string,std::string>
readFromFile( const std::string &filename )
{
    std::map<std::string,std::string> properties;

    string line;
    ifstream file(filename.c_str());
    if ( file.is_open() )
    {
        while ( !file.eof() )
        {
            getline(file,line);
            if ( line.empty() )
                continue;

            vector<string> keyAndVal = hydroutil::toStringSeq( line, '=' );
            if ( keyAndVal.size() != 2 )
            {
                stringstream ss;
                ss << "Badly formed line: '" << line << "'";
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }
            properties[keyAndVal[0]] = keyAndVal[1];
        }
        file.close();
    }
    else
    {
        stringstream ss;
        ss << "Failed to open file for reading: " << filename;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    return properties;
}

}

