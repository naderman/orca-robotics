IF ( NOT OS_WIN AND QT4_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseComponentRules.cmake )
    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )

    GLOBAL_ADD_LIBRARY( OrcaQUml SHARED ${HUMAN_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQUml OrcaDef )

ELSE  ( NOT OS_WIN AND QT4_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQuml -- Windows or Qt not found")

ENDIF ( NOT OS_WIN AND QT4_FOUND )
