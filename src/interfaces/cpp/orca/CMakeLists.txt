MESSAGE( STATUS "Planning to build C++ interfaces." )

#
# Special project-specific stuff
#
INCLUDE( ${PROJ_SOURCE_DIR}/config/project/project_rules.cmake )

#
# Do not include /config/compile_flags.cmake here. It will lead to circular dependencies.
#
INCLUDE( ${PROJ_SOURCE_DIR}/config/slice2cpp_rules.cmake )

#
# All Slice source files are defined in one place.
#
INCLUDE( ${PROJ_SOURCE_DIR}/src/interfaces/slice/orca/slice_sources.cmake )

#
# Work out the list of generated files from the list of slice sources
#
GENERATE_SLICE2CPP_RULES( CPP_FILES HEADER_FILES ${SLICE_SOURCE_FILES} )

#
# Temporary measure until we get the shared stuff working under win32
#
IF( NOT WIN32 )
  ADD_LIBRARY( OrcaInterfaces SHARED ${CPP_FILES} )
ELSE( NOT WIN32 )
  ADD_LIBRARY( OrcaInterfaces STATIC ${CPP_FILES} )
ENDIF( NOT WIN32 ) 

INSTALL_TARGETS( /lib OrcaInterfaces )
INSTALL_FILES( /include/orca FILES ${HEADER_FILES} )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJ_BINARY_DIR}/src/interfaces/cpp
    ${PROJ_BINARY_DIR}/src/interfaces/cpp/orca
)

IF( WIN32 )
    # Windows needs to  know the symbols at compile time; no run-time linking
    # a-la Linux. 
    TARGET_LINK_LIBRARIES( OrcaInterfaces
                optimized Ice optimized IceUtil 
                debug Iced debug IceUtild )
ENDIF( WIN32 )

ADD_DEFINITIONS( "-Wall" )
