/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_PROPERTY_FILE_UTILITIES_H
#define ORCAICE_PROPERTY_FILE_UTILITIES_H

#include <string>
#include <Ice/Ice.h>

namespace orcaice
{
namespace detail
{

/*
 *  Platform-independent function to find Orca global configuration file.
 *
 *  1. Tries to load the file specified in Orca.GlobalConfig configuration parameter.
 *  2. If it is not defined (or empty), tries to load the file specified by the 
 *     environment variable @c ORCAICE_GLOBAL_CONFIG
 *  3. If it is not defined (or empty), tries to load the defaultt file from a 
 *     system-dependent location:
 *          > Linux: $HOME/.orcarc
 *          > Win32: C:\orca.ini
 *  Throws gbxutilacfr::Exception if none of the three options work out.
 */
std::string getGlobalConfigFilename( const Ice::PropertiesPtr &props );

/*
 *  Platform-independent function to find Orca component configuration file.
 *
 *  1. Returns the file name specified in Orca.Config configuration parameter.
 *          $ mycomponent --Orca.Config=local.cfg
 *              returns "local.cfg"
 *  2. Returns the file name specified in Ice.Config configuration parameter.
 *          $ mycomponent --Ice.Config=local.cfg
 *              returns "local.cfg"
 *  3. If Orca.Config is not defined (or empty), and there is only one command line argument
 *     it is assumed to be the path to the config file.
 *          $ mycomponent local.cfg
 *              returns "local.cfg"
 *  4. File ./mycomponent.cfg when the component mycomponent is executed with 
 *     no command line arguments.
 *          $ mycomponent
 *          > mycomponent.exe
 *              return "mycomponent.cfg"
 *  
 *  Returns an empty string if everything else fails.
 */
std::string getApplicationConfigFilename( const Ice::StringSeq & args );

/*
 *  Platform-independent function to find Orca component configuration file.
 *
 *  1. Returns the file name specified in Orca.Config configuration parameter. E.g.
 *          IceBox.Service.MyService=MyService:create --Orca.Config=local.cfg
 *              returns "local.cfg"
 *
 *  2. Returns the file name specified in Ice.Config configuration parameter. E.g.
 *          IceBox.Service.MyService=MyService:create --Ice.Config=local.cfg
 *              returns "local.cfg"
 *
 *  Returns an empty string if the argument list is empty or
 *  the argument contains no suitable properties.
 */
std::string getServiceConfigFilename( const Ice::StringSeq & args );

} // namespace
} // namespace

#endif
