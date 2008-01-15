MESSAGE( STATUS "Planning to build C++ version of Slice interfaces." )

#
# Install sub-directory will be the same as the current sub-directory
# which is typically the same as the name of the namespace, e.g. 'orca'
#
GET_FILENAME_COMPONENT( INTERFACE_NAMESPACE ${CMAKE_CURRENT_SOURCE_DIR} NAME )

#
# All Slice source files are defined in one place.
#
INCLUDE( ${PROJECT_SOURCE_DIR}/src/interfaces/slice/${INTERFACE_NAMESPACE}/slice_sources.cmake )

INCLUDE( ${ORCA_CMAKE_DIR}/UseBasicRules.cmake )
INCLUDE( ${ORCA_CMAKE_DIR}/UseZeroC.cmake )

#
# Defines macros for compiling Slice source files.
#
INCLUDE( ${ORCA_CMAKE_DIR}/slice2cpp_rules.cmake )

#
# Work out the list of generated files from the list of slice sources
#
GENERATE_SLICE2CPP_RULES( SLICE_GENERATED_CPP_FILES SLICE_GENERATED_HEADER_FILES ${SLICE_SOURCE_FILES} )

INCLUDE_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp
    ${PROJECT_BINARY_DIR}/src/interfaces/cpp/${INTERFACE_NAMESPACE}
)

# for satelite projects, add the orca install directory
IF ( DEFINED ORCA_HOME )
    INCLUDE_DIRECTORIES( ${ORCA_HOME}/include )
ENDIF ( DEFINED ORCA_HOME )


IF( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} SHARED ${SLICE_GENERATED_CPP_FILES} )
    TARGET_LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB} Ice IceUtil )
ELSE( NOT OS_WIN )
    GLOBAL_ADD_LIBRARY( ${PROJECT_INTERFACE_LIB} STATIC ${SLICE_GENERATED_CPP_FILES} )
    TARGET_LINK_LIBRARIES( ${PROJECT_INTERFACE_LIB}
                optimized Ice optimized IceUtil 
                debug Iced debug IceUtild )
ENDIF( NOT OS_WIN ) 

INSTALL_FILES( /include/${INTERFACE_NAMESPACE} FILES ${SLICE_GENERATED_HEADER_FILES} )
