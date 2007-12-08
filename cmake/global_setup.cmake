#
# Give feedback on custom entries
#
MESSAGE( STATUS "Setting project name to ${PROJECT_NAME}" )
MESSAGE( STATUS "Setting project version to ${PROJECT_VERSION}" )
MESSAGE( STATUS "Setting project interface lib name to ${PROJECT_INTERFACE_LIB}" )

#
# Official dependency number 1: Hydro
# Find Hydro installation, we need it early to use its cmake scripts
#
IF ( DEFINED HYDRO_HOME )
    # Hydro home is specified with a command line option or is already in cache
    MESSAGE( STATUS "Hydro location was specified or using cached value: ${HYDRO_HOME}")
ELSE ( DEFINED HYDRO_HOME )
    # Find Hydro installation
    INCLUDE ( ${ORCA_CMAKE_DIR}/FindHydro.cmake )
    ASSERT( HYDRO_FOUND 
            "Looking for Hydro - not found. Please install Hydro, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Hydro - found in ${HYDRO_HOME}" 
            1 )
ENDIF ( DEFINED HYDRO_HOME )
#
# Load Hydro manifest
#
INCLUDE( ${HYDRO_HOME}/hydro_manifest.cmake )
MESSAGE( STATUS "Loaded Hydro manifest")

#
# process version number
#
INCLUDE( ${HYDRO_HOME}/cmake/version.cmake )

#
# Project directories
#
INCLUDE( ${HYDRO_HOME}/cmake/dirs.cmake )

#
# Determine OS, and make os-specefic choices
#
INCLUDE( ${HYDRO_HOME}/cmake/os.cmake )

#
# Set the build type (affects debugging symbols and optimization)
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/buildtype.cmake )

#
# check compiler type and version
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/compiler.cmake )

#
# 
# Official dependency number 2: ZeroC's Ice
# Find Ice installation
#
IF ( DEFINED ICE_HOME )
    # Ice home is specified with a command line option or it's already in cache
    MESSAGE( STATUS "Ice location was specified or using cached value: ${ICE_HOME}")
ELSE ( DEFINED ICE_HOME )
    # Find Ice installation
    INCLUDE ( ${ORCA_CMAKE_DIR}/FindIce.cmake )
    ASSERT( ICE_FOUND 
            "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Ice - found in ${ICE_HOME}" 
            1 )
ENDIF ( DEFINED ICE_HOME )

# Test Ice installation
INCLUDE ( ${ORCA_CMAKE_DIR}/TestIce.cmake )
ASSERT ( ICE_WORKS
         "Testing Ice - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
         "Testing Ice - ok."
         1 )

# Check which parts of Ice are actually installed (produce "manifest")
# INCLUDE ( ${ORCA_CMAKE_DIR}/ManifestIce.cmake )

#
# Include external and local macro definitions
#
INCLUDE( ${HYDRO_HOME}/cmake/GlobalAdd.cmake )
INCLUDE( ${HYDRO_HOME}/cmake/Require.cmake )
INCLUDE( ${HYDRO_HOME}/cmake/messages.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/FindComponentSources.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/GenerateConfigFile.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

#
# Defaults for big source code switches 
# (these are defaults. after the user modifies these in GUI they stay in cache)
#
OPTION( BUILD_SERVICES "Enables compilation of all IceBox services" OFF )
OPTION( BUILD_JAVA     "Enables compilation of all Java interfaces and components" OFF  )
OPTION( BUILD_PYTHON   "Enables compilation of all Python interfaces and components" OFF  )
OPTION( BUILD_TESTS    "Enables compilation of all tests" ON  )
OPTION( BUILD_EXAMPLES "Enables compilation of all examples" ON  )
OPTION( BUILD_SANDBOX  "Enables compilation of everything in the sandbox" OFF )
OPTION( GENERATE_XML   "Enables generation of XML file for IceGrid" ON )

#                                                         
# Look for low-level C headers, write defines to config.h 
#                                                         
INCLUDE( ${ORCA_CMAKE_DIR}/write_config_h.cmake )

#                                                         
# Look for dependencies required by individual components 
# alexm: everything or most of what's currently in this file
# will move into individual comps and libs, only the global dependencies
# will be checked globally.
#                                                         
INCLUDE( ${ORCA_CMAKE_DIR}/check_depend.cmake )

#
# Project-specific global setup
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/local/project_setup.cmake )

# Store the location of the command in cache
# (after the project-specific Find functions are defined ...)
FIND_DEFTOOLS( DEFTOOLS_HOME )
SET( ORCA_DEF2CFG_COMMAND ${DEFTOOLS_HOME}/def2cfg${EXE_EXTENSION} 
        CACHE PATH "Path to def2cfg executable." FORCE )
SET( ORCA_DEF2XML_COMMAND ${DEFTOOLS_HOME}/def2xml${EXE_EXTENSION} 
        CACHE PATH "Path to def2xml executable." FORCE )
# SET( ORCA_DEF2XMLTEMPLATE_COMMAND ${DEFTOOLS_HOME}/def2xmltemplate${EXE_EXTENSION} 
#         CACHE PATH "Path to def2xmltemplate executable." FORCE )
MESSAGE( STATUS "Using ${ORCA_DEF2CFG_COMMAND}" )

#
# Installation preferences
#
# CMake default is FALSE
# SET( CMAKE_SKIP_BUILD_RPATH TRUE )
# CMake default is FALSE
# SET( CMAKE_BUILD_WITH_INSTALL_RPATH TRUE )
SET( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib ${HYDRO_HOME}/lib ${ICE_HOME}/lib )

IF ( NOT ORCA_MOTHERSHIP )
    # For satellite projects only, link to Orca
    SET( CMAKE_INSTALL_RPATH  ${CMAKE_INSTALL_RPATH} ${ORCA_HOME}/lib )
ENDIF ( NOT ORCA_MOTHERSHIP )

IF ( ORCA_MOTHERSHIP )
    # For orca project only, install CMake scripts
    ADD_SUBDIRECTORY ( cmake )
ENDIF ( ORCA_MOTHERSHIP )

#
# Testing with CTest
#
# Enable testing by including the Dart module
# (must be done *before* entering source directories )
INCLUDE (${CMAKE_ROOT}/Modules/Dart.cmake)
ENABLE_TESTING()

#                                                         
# Enter the source tree                                   
#                                                         
ADD_SUBDIRECTORY ( src )

#
# Some scripts need to be installed
#
ADD_SUBDIRECTORY ( scripts )


#
# Write installation manifest in CMake format
#
WRITE_MANIFEST()   

#                                                         
# Print results of CMake activity                         
#                                                  
GLOBAL_CONFIG_REPORT( "Ice version       ${ICE_VERSION}" )
GLOBAL_LIST_RESET()
