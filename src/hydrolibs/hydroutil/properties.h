/*
Orca Project: Components for robotics 
             http://orca-robotics.sf.net/
Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
This copy of Orca is licensed to you under the terms described in the
ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_PROPERTIES_H
#define HYDROUTIL_PROPERTIES_H

#include <string>
#include <map>
#include <vector>

namespace hydroutil
{

/*!
@brief Container for key-value pairs, useful for storing configuration properties.

Not thread-safe.
 */
class Properties
{
public:

    //! Configuration structure for Properties
    struct Config
    {
        //! Constructor
        Config( bool warnDefaultProp=false ) :
            warnDefaultProperty(warnDefaultProp) {};

        //! If set to TRUE, prints a warning to cout when default value of a property is used.
        //! Default value for this property is FALSE.
        bool warnDefaultProperty;
    };

    //! Constructor
    Properties( const Config& config=Config() );
    
    //! Copy constructor.
    //! If @c removePrefix is not empty, it is assumed that all keys in @c props start
    //! with the same prefix and it is removed. This is a common situation when @c props
    //! is the output of Ice::Properties::getPropertiesForPrefix() function.
    //! If a particular key does not have the specified prefix, it is quietly added
    //! unmodified.
    Properties( const std::map<std::string,std::string>& props, 
                const std::string& removePrefix="",
                const Config& config=Config() );

    //! Returns TRUE if the property with @c key is defined (even if it's empty) and 
    //! FALSE if it is not defined.
    bool isDefined( const ::std::string& key );
    
    //! Checks if the property @c key is defined. Throws gbxutilacfr::Exception if the 
    //! property is not defined.
    void ensureDefined( const std::string& key );

    //! Returns: 0 = property found (and set in value), non-zero = property not found.
    int getProperty( const ::std::string& key, std::string &value ) const;
    
    //! Returns the default value if key is not found.
    std::string getPropertyWithDefault( const ::std::string& key, const std::string &defaultValue ) const;
    
    //! Returns: 0 = property found (and set in value), non-zero = property not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    int getPropertyAsDouble( const ::std::string& key, double &value ) const;
    
    //! Returns the default value if key is not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    double getPropertyAsDoubleWithDefault( const ::std::string& key, const double defaultValue ) const;

    //! Parses the value into a vector of doubles assuming empty space delimeters. Elements will continue
    //! to be added to the end of the vector until the end of the line is reached.
    //! Returns: 0 = property found (and set in value), non-zero = property not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    int getPropertyAsDoubleVector( const ::std::string& key, std::vector<double> &value ) const;
    
    //! Returns the default value if key is not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    std::vector<double> getPropertyAsDoubleVectorWithDefault( const ::std::string& key, const std::vector<double> &defaultValue ) const;

    //! Returns: 0 = property found (and set in value), non-zero = property not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    int getPropertyAsInt( const ::std::string& key, int &value ) const;
    
    //! Returns the default value if key is not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    int getPropertyAsIntWithDefault( const ::std::string& key, const int defaultValue ) const;

    //! Parses the value into a vector of ints assuming empty space delimeters. Elements will continue
    //! to be added to the end of the vector until the end of the line is reached.
    //! Returns: 0 = property found (and set in value), non-zero = property not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    int getPropertyAsIntVector( const ::std::string& key, std::vector<int> &value ) const;

    //! Returns the default value if key is not found.
    //! Throws an gbxutilacfr::Exception if the key is found but the value cannot be converted to the desired type.
    std::vector<int> getPropertyAsIntVectorWithDefault( const ::std::string& key, const std::vector<int> &defaultValue ) const;
    
    //! Sets property
    void setProperty( const ::std::string& key, const ::std::string& value );
    
    //! Returns const reference to the property map
    const std::map<std::string,std::string>& data() const { return props_; };

    //! Returns number of properties in the set
    size_t size() const { return props_.size(); }; 

    //! Returns all properties in format "key=value" per line.
    std::string toString() const;

    //! Returns standardized text which warns that a configuration property is not set.
    //! If @c fatal is TRUE, the message adds that "operation cannot continue".
    static std::string missingPropertyWarning( const std::string& key, bool fatal=false );

    const Config &config() const { return config_; }

private:
    
    std::map<std::string,std::string> props_;

    Config config_;
};

} // end namespace

#endif
