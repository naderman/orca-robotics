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

#
# We often use different variants of the project name.
# E.g. for project "orca" we want "ORCA", "orca", "Orca".
#
include( ${ORCA_CMAKE_DIR}/StringToCapital.cmake )
ORCA_STRING_TO_CAPITAL( ${PROJECT_NAME} PROJECT_NAME_CAP )
# message( STATUS "Setting capitalized project name to ${PROJECT_NAME_CAP}" )
string( TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER )
string( TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER )

#
# Official dependency number 1: Hydro
#

# if( DEFINED HYDRO_FOUND AND HYDRO_FOUND )
# 
#     message( STATUS "Hydro was previously found - includes are in ${HYDRO_INCLUDE_DIR}" )
# 
# else( DEFINED HYDRO_FOUND AND HYDRO_FOUND )
    
    if( DEFINED IS_SUPER_PROJECT )
        
        find_path( super_hydro_manifest_dir hydro_manifest.cmake ${CMAKE_BINARY_DIR}/hydro NO_DEFAULT_PATH )
    
        ASSERT( super_hydro_manifest_dir 
                "Looking for super-project Hydro - not found. Is this a super-project?" 
                "Looking for super-project Hydro - found manifest in ${super_hydro_manifest_dir}" 
                1 )
    
        set( HYDRO_FOUND 1 CACHE BOOL "Do we have Hydro?" FORCE )   
    
        set( HYDRO_MANIFEST_DIR ${CMAKE_BINARY_DIR}/hydro )
        # This is potentially problematic: installed directory structure is different from in-source one.
        set( HYDRO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/hydro/src/libs 
                            ${CMAKE_SOURCE_DIR}/hydro/src/hydrodrivers 
                            ${CMAKE_SOURCE_DIR}/hydro/src )
        set( HYDRO_CMAKE_DIR ${CMAKE_SOURCE_DIR}/hydro/cmake )
        # unnecessary: cmake will find the targets without any directory specified.
    #     set( HYDRO_LIB_DIR ${CMAKE_BINARY_DIR}/hydro/??? )
        # this is where Hydro libs will be installed 
        set( HYDRO_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/hydro )
    
    else( DEFINED IS_SUPER_PROJECT )
    
        include( ${ORCA_CMAKE_DIR}/FindHydro.cmake )
        ASSERT( HYDRO_FOUND 
                "Looking for Hydro - not found. Please install Hydro, then re-run CMake." 
                "Looking for Hydro - includes found in ${HYDRO_INCLUDE_DIR}" 
                1 )

        set( HYDRO_MANIFEST_DIR ${HYDRO_HOME} )    
        set( HYDRO_INCLUDE_DIR ${HYDRO_HOME}/include/hydro )
        set( HYDRO_CMAKE_DIR ${HYDRO_HOME}/share/hydro/cmake )
        # this is where Hydro libs are installed and to where the *built* Orca libs will be linked
        set( HYDRO_LIB_DIR ${HYDRO_HOME}/lib/hydro )
        # this is where Hydro libs are installed and to where the *installed* Orca libs will be linked
        set( HYDRO_LINK_DIR ${HYDRO_HOME}/lib/hydro ) 
    
    endif( DEFINED IS_SUPER_PROJECT )

# endif( DEFINED HYDRO_FOUND AND HYDRO_FOUND )

# Load Hydro manifest
include( ${HYDRO_MANIFEST_DIR}/hydro_manifest.cmake )
ASSERT( HYDRO_MANIFEST_LOADED 
        "Loading Hydro manifest - failed. Please reinstall Hydro, then re-run CMake." 
        "Loading Hydro manifest - loaded." 
        1 )

# Test Hydro installation
include( ${ORCA_CMAKE_DIR}/TestHydro.cmake )
ASSERT ( HYDRO_WORKS
         "Testing Hydro - failed. Please check or reinstall it, then re-run CMake."
         "Testing Hydro - ok."
         1 )

#
# Official dependency number 2: Gearbox
#         
         
# if( DEFINED GEARBOX_FOUND AND GEARBOX_FOUND )
# 
#     message( STATUS "Gearbox was previously found - includes are in ${GEARBOX_INCLUDE_DIR}" )
# 
# else( DEFINED GEARBOX_FOUND AND GEARBOX_FOUND )
    
    if( DEFINED IS_SUPER_PROJECT )
        
        find_path( super_gearbox_manifest_dir gearbox_manifest.cmake ${CMAKE_BINARY_DIR}/gearbox NO_DEFAULT_PATH )
    
        ASSERT( super_gearbox_manifest_dir 
                "Looking for super-project Gearbox - not found. Is this a super-project?" 
                "Looking for super-project Gearbox - found manifest in ${super_gearbox_manifest_dir}" 
                1 )
    
        set( GEARBOX_FOUND 1 CACHE BOOL "Do we have Gearbox?" FORCE )   
    
        set( GEARBOX_MANIFEST_DIR ${CMAKE_BINARY_DIR}/gearbox )
        # This is potentially problematic: installed directory structure is different from in-source one.
        set( GEARBOX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/gearbox/src )
        set( GEARBOX_CMAKE_DIR ${CMAKE_SOURCE_DIR}/gearbox/cmake )
        # unnecessary: cmake will find the targets without any directory specified.
    #     set( GEARBOX_LIB_DIR ${CMAKE_BINARY_DIR}/gearbox/??? )
        # this is where Hydro libs will be installed 
        set( GEARBOX_LINK_DIR ${CMAKE_INSTALL_PREFIX}/lib/gearbox )
    
    else( DEFINED IS_SUPER_PROJECT )
    
        include( ${HYDRO_CMAKE_DIR}/FindGearbox.cmake )
        ASSERT( GEARBOX_FOUND 
                "Looking for Gearbox - not found. Please install Gearbox, then re-run CMake." 
                "Looking for Gearbox - includes found in ${GEARBOX_INCLUDE_DIR}" 
                1 )

        set( GEARBOX_MANIFEST_DIR ${GEARBOX_HOME} )    
        set( GEARBOX_INCLUDE_DIR ${GEARBOX_HOME}/include/gearbox )
        set( GEARBOX_CMAKE_DIR ${GEARBOX_HOME}/share/gearbox/cmake )
        # this is where Hydro libs are installed and to where the *built* Orca libs will be linked
        set( GEARBOX_LIB_DIR ${GEARBOX_HOME}/lib/gearbox )
        # this is where Hydro libs are installed and to where the *installed* Orca libs will be linked
        set( GEARBOX_LINK_DIR ${GEARBOX_HOME}/lib/gearbox ) 
    
    endif( DEFINED IS_SUPER_PROJECT )

# endif( DEFINED GEARBOX_FOUND AND GEARBOX_FOUND )
      
# Load Gearbox manifest
include( ${GEARBOX_MANIFEST_DIR}/gearbox_manifest.cmake )
ASSERT( GEARBOX_MANIFEST_LOADED 
        "Loading Gearbox manifest - failed. Please reinstall Gearbox, then re-run CMake." 
        "Loading Gearbox manifest - loaded." 
        1 )

# Test Gearbox installation ??

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
ASSERT( ICE_FOUND 
        "Looking for Ice - not found. Please install Ice, ** delete CMakeCache.txt **, then re-run CMake." 
        "Looking for Ice - found in ${ICE_HOME}" 
        1 )

# Test Ice installation
include( ${ORCA_CMAKE_DIR}/TestIce.cmake )
ASSERT ( ICE_WORKS
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
include( ${ORCA_CMAKE_DIR}/check_depend.cmake )

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
message( STATUS GEARBOX_LIB_DIR=${GEARBOX_LIB_DIR} )
message( STATUS HYDRO_LIB_DIR=${HYDRO_LIB_DIR} )
message( STATUS CMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH} )

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

    # other projet-specific stuff
    include( ${PROJECT_SOURCE_DIR}/cmake/internal/project_setup.cmake )

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
ENABLE_TESTING()

#                                                         
# Enter the source tree                                   
#                                                         
add_subdirectory( src )

#
# Some scripts need to be installed
#
add_subdirectory( scripts )


#
# Write installation manifest in CMake format
#
GBX_WRITE_MANIFEST()   
GBX_WRITE_OPTIONS()

#
# Print license information to a file
#
if( ORCA_BUILD_LICENSE )
    GBX_WRITE_LICENSE()
endif( ORCA_BUILD_LICENSE )

#                                                         
# Print results of CMake activity                         
#                                                  
GBX_CONFIG_REPORT( "Ice version       ${ICE_VERSION}" )

#
# House-keeping, clear all lists
#
GBX_RESET_ALL_TARGET_LISTS()
GBX_RESET_ALL_DEPENDENCY_LISTS()
