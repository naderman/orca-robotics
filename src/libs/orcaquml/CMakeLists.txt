IF ( NOT WIN32 AND QT42_FOUND )

    INCLUDE( ${ORCA_CMAKE_DIR}/UseQt.cmake )

    FILE( GLOB HUMAN_SRCS *.cpp )
    
    INCLUDE_DIRECTORIES(
        # not including standard configs, have to do it manually
        ${PROJECT_BINARY_DIR}/src/interfaces/cpp
        ${PROJECT_SOURCE_DIR}/src/libs
    )

    GLOBAL_ADD_LIBRARY( OrcaQUml SHARED ${HUMAN_SRCS} )
    
    TARGET_LINK_LIBRARIES ( OrcaQUml OrcaDef )

ELSE  ( NOT WIN32 AND QT42_FOUND )

    MESSAGE( STATUS "Not Planning to Build Library: libOrcaQuml -- Qt 4.2 not found")

ENDIF ( NOT WIN32 AND QT42_FOUND )
