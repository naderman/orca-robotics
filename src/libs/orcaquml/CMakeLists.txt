IF ( NOT OS_WIN AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/component_rules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseOrcaIce.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        # not including standard configs, have to do it manually
        ${PROJECT_BINARY_DIR}/src/interfaces/cpp
        ${PROJECT_SOURCE_DIR}/src/libs
    )

    GLOBAL_ADD_LIBRARY( OrcaQUml SHARED ${HUMAN_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQUml OrcaDef )

ELSE  ( NOT OS_WIN AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQuml -- Windows or Qt not found")

ENDIF ( NOT OS_WIN AND QT4_FOUND )
