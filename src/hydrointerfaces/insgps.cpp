/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <hydrobros1/hydrobros1.h>

#include "insgps.h"

using namespace std;

using namespace hydrointerfaces;

InsGps::Config::Config()
{
}

bool
InsGps::Config::isValid() const
{
    // nothing to validate yet   
    return true;
}
std::string
InsGps::Config::toString() const
{
    std::stringstream ss;
    ss << "InsGps driver config: \n";
    ss << "Empty for now \n";
    return ss.str();
}

/*
bool 
InsGps::Config::operator==( const InsGps::Config & other )
{
    return true;
}

bool 
InsGps::Config::operator!=( const InsGps::Config & other )
{
    return false;
}
*/
#if 0
// details of the generic transport and asociated conversion magic
typedef std::auto_ptr<InsGps::GenericData> GenericPtr;

class InsWrapper : public InsGps::GenericData
{
public:
    InsWrapper() : InsGps::GenericData( InsGps::Ins ) {};
    std::auto_ptr<InsGps::InsData> dataPtr;
};
typedef std::auto_ptr<InsWrapper> InsWrapperPtr;

class GpsWrapper : public InsGps::GenericData
{
public:
    GpsWrapper() : InsGps::GenericData( InsGps::Gps ) {};
    std::auto_ptr<InsGps::GpsData> dataPtr;
};
typedef std::auto_ptr<GpsWrapper> GpsWrapperPtr;

class ImuWrapper : public InsGps::GenericData
{
public:
    ImuWrapper() : InsGps::GenericData( InsGps::Imu ) {};
    std::auto_ptr<InsGps::ImuData> dataPtr;
};
typedef std::auto_ptr<ImuWrapper> ImuWrapperPtr;

// conversion functions, to put actual data into generic transport capsules
// for use inside the drivers
std::auto_ptr<InsGps::GenericData>
insgpsutil::generic(std::auto_ptr<InsGps::InsData> data){
    InsWrapperPtr wrapperPtr(new InsWrapper);
    wrapperPtr->dataPtr = data;
    return GenericPtr((InsGps::GenericData *)wrapperPtr.release());
}
std::auto_ptr<InsGps::GenericData>
insgpsutil::generic(std::auto_ptr<InsGps::GpsData> data){
    GpsWrapperPtr wrapperPtr(new GpsWrapper);
    wrapperPtr->dataPtr = data;
    return GenericPtr((InsGps::GenericData *)wrapperPtr.release());
}
std::auto_ptr<InsGps::GenericData>
insgpsutil::generic(std::auto_ptr<InsGps::ImuData> data){
    ImuWrapperPtr wrapperPtr(new ImuWrapper);
    wrapperPtr->dataPtr = data;
    return GenericPtr((InsGps::GenericData *)wrapperPtr.release());
}

// conversion functions, to recover the encapsulated data from the generic transport mechanism;
// generic is invalid (0) after this
std::auto_ptr<InsGps::InsData>
insgpsutil::generic2Ins(std::auto_ptr<InsGps::GenericData> generic){
    InsWrapper *tmp = dynamic_cast<InsWrapper*>(generic.get());
    return tmp->dataPtr;
}
std::auto_ptr<InsGps::GpsData>
insgpsutil::generic2Gps(std::auto_ptr<InsGps::GenericData> generic){
    GpsWrapper *tmp = dynamic_cast<GpsWrapper*>(generic.get());
    return tmp->dataPtr;
}
std::auto_ptr<InsGps::ImuData>
insgpsutil::generic2Imu(std::auto_ptr<InsGps::GenericData> generic){
    ImuWrapper *tmp = dynamic_cast<ImuWrapper*>(generic.get());
    return tmp->dataPtr;
}
#endif

