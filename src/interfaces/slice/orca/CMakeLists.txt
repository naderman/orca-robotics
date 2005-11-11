#
# Do not include /config/compile_flags.cmake here. It will lead to circular dependencies.
#

INCLUDE( ${PROJ_SOURCE_DIR}/config/slice2cpp_rules.cmake )

#
# Since CMake doesn't know how to automatically track dependencies for .ice files, 
# these have to be entered manually such that the depended-on files are listed first.
#
SET( SLICE_SOURCES
# definitions
    orca.ice

    bros1.ice
# objects
    image.ice
    ogmap.ice
    opaque.ice
#interfaces
    binaryindicator.ice
    rangescanner.ice
    laser.ice
    position2d.ice
    platform2d.ice
    polarfeature2d.ice
    power.ice
    status.ice
    ptz.ice 
    hand.ice
    )

#
# Work out the list of generated files from the list of slice sources
#
GENERATE_SLICE_RULES( SLICE_CPP_FILES SLICE_HEADER_FILES ${SLICE_SOURCES} )

#Temporary measure until we get the shared stuff working under win32
IF(NOT WIN32)
  ADD_LIBRARY( OrcaInterfaces SHARED ${SLICE_CPP_FILES} )
ELSE (NOT WIN32)
  ADD_LIBRARY( OrcaInterfaces STATIC ${SLICE_CPP_FILES} )
ENDIF(NOT WIN32) 

INSTALL_TARGETS( /lib OrcaInterfaces )
INSTALL_FILES( /include/orca FILES ${SLICE_HEADER_FILES} )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJ_BINARY_DIR}/src/interfaces/slice
    ${PROJ_BINARY_DIR}/src/interfaces/slice/orca
)

# Note: Under Windows, if in debug mode, you must link against debug
# versions of the libraries.
# Windows needs to  know the symbols at compile time; no run-time linking
# a-la Linux. 

IF( WIN32 )
  TARGET_LINK_LIBRARIES ( OrcaInterfaces Ice IceUtil) 
ENDIF( WIN32 ) 
ADD_DEFINITIONS( "-Wall" )
