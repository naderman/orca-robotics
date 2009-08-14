#
# Give feedback on custom entries
#
message( STATUS "Setting project name to ${PROJECT_NAME}" )
message( STATUS "Setting project version to ${GBX_PROJECT_VERSION}" )

#
# Set CMake policies
# For help on policy CMPxxxx: $ cmake --help-policy CMPxxxx
#
if( COMMAND cmake_policy )
    if( POLICY CMP0003 )
        cmake_policy( SET CMP0003 OLD )
    endif( POLICY CMP0003 )
    if( POLICY CMP0011 )
        cmake_policy( SET CMP0011 NEW )
    endif( POLICY CMP0011 )
endif( COMMAND cmake_policy )


set( HYDRO_CMAKE_DIR ${${PROJECT_NAME}_SOURCE_DIR}/cmake CACHE
      PATH "Location of CMake scripts" )
# we need this macro early
include( ${HYDRO_CMAKE_DIR}/Assert.cmake )

#
# Official dependency number 1: Gearbox
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

    # set base locations search
    # find_package will automatically search in /lib directories
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

#
# Test Gearbox installation
#
# include( ${HYDRO_CMAKE_DIR}/TestGearbox.cmake )
# ORCA_ASSERT ( GEARBOX_WORKS
#          "Testing Gearbox - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
#          "Testing Gearbox - ok."
#          1 )

#
# Official dependency number 2: ZeroC's IceUtil
#

include( ${GEARBOX_CMAKE_DIR}/FindIceUtil.cmake )
ORCA_ASSERT( ICEUTIL_FOUND
        "Looking for IceUtil - not found. Please install IceUtil, ** delete CMakeCache.txt **, then re-run CMake."
        "Looking for IceUtil - found in ${ICEUTIL_HOME}"
        1 )


# Test IceUtil installation
# include( ${HYDRO_CMAKE_DIR}/TestIceUtil.cmake )
# ORCA_ASSERT ( ICEUTIL_WORKS
#          "Testing IceUtil - failed. Please check or reinstall it, ** delete CMakeCache.txt **, then re-run CMake."
#          "Testing IceUtil - ok."
#          1 )

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
include( ${HYDRO_CMAKE_DIR}/internal/buildtype.cmake )


#
# Include local macro definitions
#
include( ${GEARBOX_CMAKE_DIR}/TargetUtils.cmake )
include( ${GEARBOX_CMAKE_DIR}/DependencyUtils.cmake )
include( ${HYDRO_CMAKE_DIR}/NameUtils.cmake )
include( ${HYDRO_CMAKE_DIR}/AddLinkTest.cmake )
# include( ${HYDRO_CMAKE_DIR}/messages.cmake )

#
# Defaults for big source code switches
# (these are defaults. after the user modifies these in GUI they stay in cache)
#
option( ORCA_BUILD_LICENSE  "Enables writing LICENCE file. For admins only." OFF )

#
# check compiler type and version
#
include( ${HYDRO_CMAKE_DIR}/internal/compiler.cmake )

#
# Defaults for big source code switches
# (these are defaults. after the user modifies these in GUI they stay in cache)
#
option( ORCA_BUILD_TESTS    "Enables compilation of all tests" ON  )

#
# Look for low-level C headers, write defines to config.h
#
include( ${GEARBOX_CMAKE_DIR}/WriteConfigH.cmake )

#
# Installation preferences
#
set( CMAKE_INSTALL_RPATH ${GBX_LIB_INSTALL_DIR} )

# Enable testing by including the Dart module
# (must be done *before* entering source directories )
include(${CMAKE_ROOT}/Modules/Dart.cmake)
enable_testing()

#
# Look for dependencies required by several libraries.
#
include( ${HYDRO_CMAKE_DIR}/internal/CheckCommonDependencies.cmake )

#
# Enter the source tree
#
add_subdirectory( src )

# Some cmake and shell scripts need to be installed
add_subdirectory( cmake )
add_subdirectory( scripts )


# During the install we want to 'label' the install. The SYSTEM_HYDRO_SOURCE
# Variable is needed by the install script as this will most likely be run from
# Root and not have the compelete environment. For some reason I do not understand
# The 'SET - CACHED' variables are not available in the install scripts (djlm)
# install( CODE "set(SYSTEM_HYDRO_SOURCE ${PROJECT_SOURCE_DIR})")
# install( SCRIPT ${PROJECT_SOURCE_DIR}/cmake/internal/labelInstall.cmake)

#
# Experimental
#
if( NOT DEFINED IS_SUPER_PROJECT )
    include( ${GEARBOX_CMAKE_DIR}/WritePackageConfig.cmake )
endif()

#
# Print results of CMake activity
#
# GBX_WRITE_MANIFEST()
# GBX_WRITE_OPTIONS()

#
# Print license information to a file
#
if( ORCA_BUILD_LICENSE )
    GBX_WRITE_LICENSE()
endif()

GBX_CONFIG_REPORT( "Nothing special" )

#
# House-keeping, clear all lists
#
GBX_RESET_ALL_TARGET_LISTS()
GBX_RESET_ALL_DEPENDENCY_LISTS()
