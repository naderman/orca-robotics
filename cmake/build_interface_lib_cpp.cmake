MESSAGE( STATUS "Planning to build C++ interfaces." )

#
# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
#
GET_FILENAME_COMPONENT( INTERFACE_NAMESPACE ${CMAKE_CURRENT_SOURCE_DIR} NAME )

#
# All Slice source files are defined in one place.
#
INCLUDE( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${INTERFACE_NAMESPACE}/slice_sources.cmake )

#
# Defines macros for compiling Slice source files.
#
INCLUDE( ${ORCA_CMAKE_DIR}/slice2cpp_rules.cmake )

#
# Work out the list of generated files from the list of slice sources
#
GENERATE_SLICE2CPP_RULES( CPP_FILES HEADER_FILES ${SLICE_SOURCE_FILES} )

INCLUDE_DIRECTORIES( BEFORE
    ${ICE_HOME}/include
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/${INTERFACE_NAMESPACE}
)

# for satelite projects, add the orca install directory
IF ( DEFINED ORCA2_HOME )
    INCLUDE_DIRECTORIES(
        ${ORCA2_HOME}/include
    )
ENDIF ( DEFINED ORCA2_HOME )

LINK_DIRECTORIES( ${ICE_HOME}/lib )

ADD_DEFINITIONS( "-Wall" )

IF( NOT WIN32 )
    GLOBAL_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} SHARED ${CPP_FILES} )
    TARGET_LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB} Ice IceUtil )
ELSE( NOT WIN32 )
    GLOBAL_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} STATIC ${CPP_FILES} )
    TARGET_LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB}
                optimized Ice optimized IceUtil 
                debug Iced debug IceUtild )
ENDIF( NOT WIN32 ) 

INSTALL_FILES( /include/${INTERFACE_NAMESPACE} FILES ${HEADER_FILES} )
MESSAGE ( STATUS "Will install C++ header files into ${PROJECT_INSTALL_DIR}/install/${INTERFACE_NAMESPACE}" )
