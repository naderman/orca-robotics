#
# Give feedback on custom entries
#
MESSAGE( STATUS "Setting project name to ${PROJECT_NAME}" )
MESSAGE( STATUS "Setting project version to ${GBX_PROJECT_VERSION}" )
MESSAGE( STATUS "Setting project interface lib name to ${PROJECT_INTERFACE_LIB}" )

#
# Official dependency number 1: Hydro
# (we need to find Hydro early to use its cmake scripts)
#
IF( DEFINED HYDRO_HOME AND HYDRO_HOME )
    # the variable is specified with a command line option or is already in cache
    MESSAGE( STATUS "Hydro location was specified or using cached value: ${HYDRO_HOME}" )
ELSE ( DEFINED HYDRO_HOME AND HYDRO_HOME )
    # not specified, need to find it
    INCLUDE( ${ORCA_CMAKE_DIR}/FindHydro.cmake )
    ASSERT( HYDRO_FOUND 
            "Looking for Hydro - not found. Please install Hydro, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Hydro - found in ${HYDRO_HOME}" 
            1 )
ENDIF( DEFINED HYDRO_HOME AND HYDRO_HOME )

# when we find it, put it into cache
SET( HYDRO_HOME ${HYDRO_HOME} CACHE PATH "Hydro installed directory" FORCE )

#
# Load Hydro manifest
#
INCLUDE( ${HYDRO_HOME}/hydro_manifest.cmake )
ASSERT( HYDRO_MANIFEST_LOADED 
        "Loading Hydro manifest - failed. Please reinstall Hydro, ** delete CMakeCache.txt **, then re-run CMake." 
        "Loading Hydro manifest - loaded." 
        1 )

# Test Hydro installation
INCLUDE( ${ORCA_CMAKE_DIR}/TestHydro.cmake )
ASSERT ( HYDRO_WORKS
         "Testing Hydro - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
         "Testing Hydro - ok."
         1 )

# Special Hydro directories
SET( HYDRO_BIN_DIR ${HYDRO_HOME}/bin )
SET( HYDRO_LIB_DIR ${HYDRO_HOME}/lib/hydro )
SET( HYDRO_INCLUDE_DIR ${HYDRO_HOME}/include/hydro )
SET( HYDRO_SHARE_DIR ${HYDRO_HOME}/share/hydro )
SET( HYDRO_CMAKE_DIR ${HYDRO_HOME}/share/hydro/cmake )

#
# Official dependency number 2: Gearbox
#
IF( DEFINED GEARBOX_HOME AND GEARBOX_HOME )
    # the variable is specified with a command line option or is already in cache
    MESSAGE( STATUS "Gearbox location was specified or using cached value: ${GEARBOX_HOME}" )
    # this is a hack, we'll require gearbox soon anyway
    SET( GEARBOX_FOUND 1 )
ELSE ( DEFINED GEARBOX_HOME AND GEARBOX_HOME )
    # not specified, need to find it
    INCLUDE( ${HYDRO_CMAKE_DIR}/FindGearbox.cmake )
    ASSERT( GEARBOX_FOUND 
            "Looking for Gearbox - not found. Please install Gearbox, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Gearbox - found in ${GEARBOX_HOME}" 
            1 )
ENDIF( DEFINED GEARBOX_HOME AND GEARBOX_HOME )

# when we find it, put it into cache
SET( GEARBOX_HOME ${GEARBOX_HOME} CACHE PATH "Gearbox installed directory" FORCE )

#
# Load Gearbox manifest
#
INCLUDE( ${GEARBOX_HOME}/gearbox_manifest.cmake )
ASSERT( GEARBOX_MANIFEST_LOADED 
        "Loading Gearbox manifest - failed. Please reinstall Gearbox, ** delete CMakeCache.txt **, then re-run CMake." 
        "Loading Gearbox manifest - loaded." 
        1 )

# Test Gearbox installation 
# (we don't have any specific requirements yet)
# INCLUDE( ${ORCA_CMAKE_DIR}/TestGearbox.cmake )
# ASSERT ( GEARBOX_WORKS
#          "Testing Gearbox - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
#          "Testing Gearbox - ok."
#          1 )

# Special Gearbox directories
SET( GEARBOX_BIN_DIR ${GEARBOX_HOME}/bin )
SET( GEARBOX_LIB_DIR ${GEARBOX_HOME}/lib/gearbox )
SET( GEARBOX_INCLUDE_DIR ${GEARBOX_HOME}/include/gearbox )
SET( GEARBOX_SHARE_DIR ${GEARBOX_HOME}/share/gearbox )
SET( GEARBOX_CMAKE_DIR ${GEARBOX_HOME}/share/gearbox/cmake )

#
# Special Hydro directories
#
SET( GEARBOX_CMAKE_DIR ${GEARBOX_HOME}/share/gearbox/cmake )

#
# process version number
#
INCLUDE( ${HYDRO_CMAKE_DIR}/SetupVersion.cmake )

#
# Project directories
#
INCLUDE( ${GEARBOX_CMAKE_DIR}/SetupDirectories.cmake )

#
# Determine OS, and make os-specefic choices
#
INCLUDE( ${GEARBOX_CMAKE_DIR}/SetupOs.cmake )

#
# Set the build type (affects debugging symbols and optimization)
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/internal/buildtype.cmake )

#
# check compiler type and version
#
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/internal/compiler.cmake )

# 
# Official dependency number 3: ZeroC's Ice
# Find Ice installation
#
IF( DEFINED ICE_HOME )
    # Ice home is specified with a command line option or it's already in cache
    MESSAGE( STATUS "Ice location was specified or using cached value: ${ICE_HOME}")
ELSE ( DEFINED ICE_HOME )
    # Find Ice installation
    INCLUDE( ${ORCA_CMAKE_DIR}/FindIce.cmake )
    ASSERT( ICE_FOUND 
            "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake." 
            "Looking for Ice - found in ${ICE_HOME}" 
            1 )
ENDIF( DEFINED ICE_HOME )

# Test Ice installation
INCLUDE( ${ORCA_CMAKE_DIR}/TestIce.cmake )
ASSERT ( ICE_WORKS
         "Testing Ice - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
         "Testing Ice - ok."
         1 )

# TODO: Check which parts of Ice are actually installed (produce "manifest")
# INCLUDE( ${ORCA_CMAKE_DIR}/ManifestIce.cmake )

SET( ICE_BIN_DIR ${ICE_HOME}/bin )
SET( ICE_LIB_DIR ${ICE_HOME}/lib )
SET( ICE_INCLUDE_DIR ${ICE_HOME}/include )

#
# Include external and local macro definitions
#
INCLUDE( ${GEARBOX_CMAKE_DIR}/TargetUtils.cmake )
INCLUDE( ${GEARBOX_CMAKE_DIR}/DependencyUtils.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/GenerateConfigFile.cmake )

# INCLUDE( ${HYDRO_CMAKE_DIR}/messages.cmake )
# INCLUDE( ${ORCA_CMAKE_DIR}/FindComponentSources.cmake )
# Obsolete but still used by Probe
INCLUDE( ${ORCA_CMAKE_DIR}/OptionalSubLibrary.cmake )

#
# Defaults for big source code switches 
# (these are defaults. after the user modifies these in GUI they stay in cache)
#
OPTION( ORCA_BUILD_SERVICES "Enables compilation of all IceBox services" OFF )
OPTION( ORCA_BUILD_JAVA     "Enables compilation of all Java interfaces and components" OFF  )
OPTION( ORCA_BUILD_PYTHON   "Enables compilation of all Python interfaces and components" OFF  )
OPTION( ORCA_BUILD_TESTS    "Enables compilation of all tests" ON  )
OPTION( ORCA_BUILD_EXAMPLES "Enables compilation of all examples" ON  )
OPTION( ORCA_BUILD_SANDBOX  "Enables compilation of everything in the sandbox" OFF )
OPTION( ORCA_BUILD_LICENSE  "Enables writing LICENCE file. For admins only." OFF )
OPTION( ORCA_BUILD_XML      "Enables generation of XML file for IceGrid" ON )

#                                                         
# Look for low-level C headers, write defines to config.h 
#                                                         
INCLUDE( ${GEARBOX_CMAKE_DIR}/WriteConfigH.cmake )

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
INCLUDE( ${PROJECT_SOURCE_DIR}/cmake/internal/project_setup.cmake )

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
# see: \http://www.cmake.org/Wiki/CMake_RPATH_handling
#
# CMake defaults

# use, i.e. don't skip the full RPATH for the build tree
# SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
# SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# the RPATH to be used when installing
SET( CMAKE_INSTALL_RPATH 
        ${GEARBOX_LIB_DIR}
        ${HYDRO_LIB_DIR} 
        ${ICE_INCLUDE_DIR}
        ${ORCA_LIB_DIR}      # this is non-empty only for satellite projects
        ${GBX_LIB_INSTALL_DIR} )

IF( ORCA_MOTHERSHIP )
    # For orca project only, install CMake scripts
    ADD_SUBDIRECTORY( cmake )
ENDIF( ORCA_MOTHERSHIP )

#
# Testing with CTest
#
# Enable testing by including the Dart module
# (must be done *before* entering source directories )
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
ENABLE_TESTING()

#                                                         
# Enter the source tree                                   
#                                                         
ADD_SUBDIRECTORY( src )

#
# Some scripts need to be installed
#
ADD_SUBDIRECTORY( scripts )


#
# Write installation manifest in CMake format
#
GBX_WRITE_MANIFEST()   

#
# Print license information to a file
#
IF( ORCA_BUILD_LICENSE )
    GBX_WRITE_LICENSE()
ENDIF( ORCA_BUILD_LICENSE )

#                                                         
# Print results of CMake activity                         
#                                                  
GBX_CONFIG_REPORT( "Ice version       ${ICE_VERSION}" )
GBX_RESET_ALL_LISTS()
