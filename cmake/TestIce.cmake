# Test that Ice works

# This module assumes that Ice is already found and ICE_HOME is defined
# Defines the following variables:
# ICE_WORKS : 1 if test passed, 0 otherwise.

include( ${CMAKE_ROOT}/Modules/CheckIncludeFileCXX.cmake )

if( NOT DEFINED ICE_HOME )
    message( STATUS "Testing Ice - CANNOT test because ICE_HOME is not defined." )
    set( ICE_WORKS 0 )
endif( NOT DEFINED ICE_HOME )

if( NOT ORCA_OS_WIN )
    # On 64-bit systems, Ice libraries are installed in lib64.  Is there a way to tell automatically?
    check_include_file_cxx( "Ice/Ice.h" ICE_WORKS "-I${ICE_HOME}/include -L${ICE_HOME}/lib -L${ICE_HOME}/lib64 -lIce -lIceUtil" )
    if( ICE_WORKS )
        set( ICE_WORKS 1 )
    else( ICE_WORKS )
        set( ICE_WORKS 0 )
    endif( ICE_WORKS )
else(NOT ORCA_OS_WIN)
    # Windows: there is no easy way to pass separate compile and link options to the macro, 
    # so assume we are told the truth when
    set( ICE_WORKS 1 )
endif(NOT ORCA_OS_WIN)

# Check Ice version
exec_program ( ${ICE_HOME}/bin/slice2cpp ARGS --version OUTPUT_VARIABLE ICE_VERSION )
#message( STATUS "DEBUG: Ice version: '${ICE_VERSION}'")

# notice that this allows beta versions, e.g. 3.3b
if( ICE_VERSION MATCHES "3\\.[3-9]" )
    set( ICE_VERSION_OK 1 )
else( ICE_VERSION MATCHES "3\\.[3-9]" )
    set( ICE_VERSION_OK 0 )
endif( ICE_VERSION MATCHES "3\\.[3-9]" )

ORCA_ASSERT ( ICE_VERSION_OK
            "Checking Ice version - ${ICE_VERSION} - failed. Orca requires Ice v. 3.3.0 or newer."
            "Checking Ice version - ${ICE_VERSION} - ok"
            1 )
