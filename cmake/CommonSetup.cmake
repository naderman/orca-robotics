#
# Give feedback on custom entries
#
message( STATUS "Setting project name to ${PROJECT_NAME}" )
message( STATUS "Setting project version to ${GBX_PROJECT_VERSION}" )

#
# Set CMake policies
# For help on policy CMPxxxx: $ cmake --help-policy CMPxxxx
# 
if(COMMAND cmake_policy)
    cmake_policy( SET CMP0003 OLD )
    cmake_policy( SET CMP0011 NEW )
endif(COMMAND cmake_policy)

#
# Official dependency number 1: Hydro
#
    
if( DEFINED IS_SUPER_PROJECT )
    
    set( h_config_file hydro-config-internal.cmake )
    set( h_search_dir ${CMAKE_SOURCE_DIR}/hydro/cmake/internal )
    find_path( h_found_dir ${h_config_file} PATHS ${h_search_dir} NO_DEFAULT_PATH )

    ORCA_ASSERT( h_found_dir 
            "Looking for super-project Hydro - not found. Is this a super-project?" 
            "Looking for super-project Hydro - found internal config in ${h_found_dir}" 
            1 )

    include( ${h_found_dir}/${h_config_file} )

else( DEFINED IS_SUPER_PROJECT )

    set( hydro_search_path
        # check cmake variable: i.e. specified on the command line
        ${HYDRO_HOME}
        # check environment variable
        $ENV{HYDRO_HOME}
        # check common installation points
        /usr/local
        /opt/hydro
        C:/hydro )
    message( STATUS "Searching for Hydro in: ${hydro_search_path}" )

    find_package( hydro REQUIRED 
        PATHS ${hydro_search_path}
        NO_DEFAULT_PATH )

    ORCA_ASSERT( HYDRO_FOUND 
            "Looking for Hydro - not found." 
            "Looking for Hydro - found package config in ${HYDRO_LINK_DIR}" 
            1 )

endif( DEFINED IS_SUPER_PROJECT )

# # Test Hydro installation
# include( ${ORCA_CMAKE_DIR}/TestHydro.cmake )
# ORCA_ASSERT ( HYDRO_WORKS
#          "Testing Hydro - failed. Please check or reinstall it, then re-run CMake."
#          "Testing Hydro - ok."
#          1 )

#
# Official dependency number 2: Gearbox
#         
         
if( DEFINED IS_SUPER_PROJECT )
    
    set( g_config_file gearbox-config-internal.cmake )
    set( g_search_dir ${CMAKE_SOURCE_DIR}/gearbox/cmake/internal )
    find_path( g_found_dir ${g_config_file} PATHS ${g_search_dir} NO_DEFAULT_PATH )

    ORCA_ASSERT( g_found_dir 
            "Looking for super-project Gearbox - not found. Is this a super-project?" 
            "Looking for super-project Gearbox - found internal config in ${g_found_dir}" 
            1 )

    include( ${g_found_dir}/${g_config_file} )

else( DEFINED IS_SUPER_PROJECT )

    set( gearbox_search_path 
        # Test user-specified installation point (first look in the dir specified
        # with command line CMake variable, then with environment variable)
        ${GEARBOX_HOME}
        $ENV{GEARBOX_HOME}
        # Test standard installation points
        /usr/local
        /opt/gearbox
        C:/gearbox )
    message( STATUS "Searching for Gearbox in: ${gearbox_search_path}" )

    # don't let cmake be too clever and search in standard places
    # before getting to manually specified PATHS.
    # otherwise we cannot manually point it to a particular installation
    # if the standard one also exists.
    find_package( gearbox REQUIRED 
        PATHS ${gearbox_search_path}
        NO_DEFAULT_PATH )

    ORCA_ASSERT( GEARBOX_FOUND 
            "Looking for Gearbox - not found." 
            "Looking for Gearbox - found package config in ${GEARBOX_LINK_DIR}" 
            1 )

endif( DEFINED IS_SUPER_PROJECT )

# Test Gearbox installation or rely on Hydro have already done it ??

#
# Process project name
#
include( ${GEARBOX_CMAKE_DIR}/SetupProjectName.cmake )

#
# process version number
#
include( ${HYDRO_CMAKE_DIR}/SetupVersion.cmake )

#
# Project directories
#
include( ${GEARBOX_CMAKE_DIR}/SetupDirectories.cmake )

#
# Determine OS, and make os-specefic choices
#
include( ${GEARBOX_CMAKE_DIR}/SetupOs.cmake )

#
# Set the build type (affects debugging symbols and optimization)
#
include( ${PROJECT_SOURCE_DIR}/cmake/internal/buildtype.cmake )

#
# check compiler type and version
#
include( ${PROJECT_SOURCE_DIR}/cmake/internal/compiler.cmake )

# 
# Official dependency number 3: ZeroC's Ice
#
include( ${ORCA_CMAKE_DIR}/FindIce.cmake )
ORCA_ASSERT( ICE_FOUND 
        "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake." 
        "Looking for Ice - found in ${ICE_HOME}" 
        1 )

# Test Ice installation
include( ${ORCA_CMAKE_DIR}/TestIce.cmake )
ORCA_ASSERT ( ICE_WORKS
         "Testing Ice - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
         "Testing Ice - ok."
         1 )

# TODO: Check which parts of Ice are actually installed (produce "manifest"?)
# include( ${ORCA_CMAKE_DIR}/ManifestIce.cmake )

set( ICE_BIN_DIR ${ICE_HOME}/bin )
set( ICE_LIB_DIR ${ICE_HOME}/lib )
set( ICE_INCLUDE_DIR ${ICE_HOME}/include )

#
# Include various external and local macro definitions
#
include( ${GEARBOX_CMAKE_DIR}/TargetUtils.cmake )
include( ${GEARBOX_CMAKE_DIR}/DependencyUtils.cmake )
include( ${ORCA_CMAKE_DIR}/NameUtils.cmake )

#
# Defaults for big source code switches 
# (these are defaults. after the user modifies these in ccmake, the values stay in cache)
#
option( ORCA_BUILD_SERVICES "Enables compilation of all IceBox services" OFF )
option( ORCA_BUILD_JAVA     "Enables compilation of all Java interfaces and components" OFF  )
option( ORCA_BUILD_PYTHON   "Enables compilation of all Python interfaces and components" OFF  )
option( ORCA_BUILD_TESTS    "Enables compilation of all tests" ON  )
option( ORCA_BUILD_EXAMPLES "Enables compilation of all examples" ON  )
option( ORCA_BUILD_SANDBOX  "Enables compilation of everything in the sandbox" OFF )
option( ORCA_BUILD_LICENSE  "Enables writing LICENCE file. For admins only." OFF )
option( ORCA_BUILD_XML      "Enables generation of XML file for IceGrid" ON )

#                                                         
# Look for low-level C headers, write defines to config.h 
#                                                         
include( ${GEARBOX_CMAKE_DIR}/WriteConfigH.cmake )

#                                                         
# Look for dependencies required by individual components 
# alexm: everything or most of what's currently in this file
# will move into individual comps and libs, only the global dependencies
# will be checked globally.
#                                                         
include( ${ORCA_CMAKE_DIR}/CheckCommonDependencies.cmake )

#
# Installation preferences
# see: \http://www.cmake.org/Wiki/CMake_RPATH_handling
#
# CMake defaults

# use, i.e. don't skip the full RPATH for the build tree
# set(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, don't use the install RPATH already
# (but later on when installing)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

# the RPATH to be used when installing
set( CMAKE_INSTALL_RPATH 
        ${ICE_LIB_DIR}
        ${GEARBOX_LINK_DIR}
        ${HYDRO_LINK_DIR} 
        ${ORCA_LINK_DIR}        # this is where satellite projects found Orca
        ${GBX_LIB_INSTALL_DIR}  # this is where Orca project will install libs
    )
# message( STATUS CMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH} )

#
# Some setup options are different for Orca itself and the satellite projects
#
if( ORCA_MOTHERSHIP )

    set( DEFTOOLS_HOME ${PROJECT_BINARY_DIR}/src/utils/deftools/${CMAKE_CFG_INTDIR} )
    set( SLICE2ORCA_HOME ${PROJECT_BINARY_DIR}/src/utils/slice2orca/${CMAKE_CFG_INTDIR} )

    # For orca project only, install CMake scripts
    add_subdirectory( cmake )

else( ORCA_MOTHERSHIP )

    set( DEFTOOLS_HOME ${ORCA_DEFTOOLS_BIN_DIR} )
    set( SLICE2ORCA_HOME ${ORCA_SLICE2ORCA_BIN_DIR} )

    # other project-specific stuff
    include( ${PROJECT_SOURCE_DIR}/cmake/project/ProjectSetup.cmake )

endif( ORCA_MOTHERSHIP )

#
# Store the location of utilities in cache.
# These are special because they are used for code and config file generation.
#
set( ORCA_DEF2CFG_COMMAND ${DEFTOOLS_HOME}/def2cfg${EXE_EXTENSION} 
        CACHE PATH "Path to def2cfg executable." FORCE )
message( STATUS "Using ${ORCA_DEF2CFG_COMMAND}" )

set( ORCA_DEF2XML_COMMAND ${DEFTOOLS_HOME}/def2xml${EXE_EXTENSION} 
        CACHE PATH "Path to def2xml executable." FORCE )
message( STATUS "Using ${ORCA_DEF2XML_COMMAND}" )

set( ORCA_SLICE2ORCA_COMMAND ${SLICE2ORCA_HOME}/slice2orca${EXE_EXTENSION} 
        CACHE PATH "Path to slice2log$ executable." FORCE )
message( STATUS "Using ${ORCA_SLICE2ORCA_COMMAND}" )

#
# Testing with CTest
#
# Enable testing by including the Dart module
# (must be done *before* entering source directories )
include(${CMAKE_ROOT}/Modules/Dart.cmake)
enable_testing()

#                                                         
# Enter the source tree                                   
#                                                         
add_subdirectory( src )

#
# Some scripts need to be installed
#
add_subdirectory( scripts )

#
# Experimental
#
if( NOT DEFINED IS_SUPER_PROJECT )
    # disable this and add <package>-config-version.cmake
    # if you want the satellite project to install <package>-config.cmake
    if( ORCA_MOTHERSHIP )
        include( ${GEARBOX_CMAKE_DIR}/WritePackageConfig.cmake )
    endif()
endif()

#
# Write installation manifest in CMake format
#
# GBX_WRITE_MANIFEST()   
# GBX_WRITE_OPTIONS()

#
# Print license information to a file
#
if( ORCA_BUILD_LICENSE )
    GBX_WRITE_LICENSE()
endif()

#                                                         
# Print results of CMake activity                         
#                                                  
GBX_CONFIG_REPORT( "Ice version       ${ICE_VERSION}" )

#
# House-keeping, clear all lists
#
GBX_RESET_ALL_TARGET_LISTS()
GBX_RESET_ALL_DEPENDENCY_LISTS()
